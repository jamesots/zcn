/* pbmtomrf - convert pbm to mrf
 * public domain by RJM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int bitbox,bitsleft;
FILE *bit_out;


void bit_init(FILE *out)
{
bitbox=0; bitsleft=8;
bit_out=out;
}


void bit_output(int bit)
{
bitsleft--;
bitbox|=(bit<<bitsleft);
if(!bitsleft)
  {
  fputc(bitbox,bit_out);
  bitbox=0;
  bitsleft=8;
  }
}


void bit_flush()
{
/* there are never 0 bits left outside of bit_output, but
 * if 8 bits are left here there's nothing to flush, so
 * only do it if bitsleft!=8.
 */
if(bitsleft!=8)
  {
  bitsleft=1;
  bit_output(0);	/* yes, really. This will always work. */
  }
}


void do_square(unsigned char *image,int ox,int oy,int w,int size)
{
int x,y,t=0;

/* check square to see if it's all black or all white. */

for(y=0;y<size;y++)
  for(x=0;x<size;x++)
    t+=image[(oy+y)*w+ox+x];

/* if the total's 0, it's black. if it's size*size, it's white. */
if(t==0 || t==size*size)
  {
  if(size!=1)		/* (it's implicit when size is 1, of course) */
    bit_output(1);	/* all same colour */
  bit_output(t?1:0);
  return;
  }

/* otherwise, if our square is greater than 1x1, we need to recurse. */
if(size>1)
  {
  bit_output(0);	/* not all same */
  size>>=1;
  do_square(image,ox,oy,w,size);
  do_square(image,ox+size,oy,w,size);
  do_square(image,ox,oy+size,w,size);
  do_square(image,ox+size,oy+size,w,size);
  }
}


/* the aim of this routine is play around with the edges which
 * are compressed into the mrf but thrown away when it's decompressed,
 * such that we get the best compression possible.
 * If you don't see why this is a good idea, consider the simple case
 * of a 1x1 white pixel. Placed on a black 64x64 this takes several bytes
 * to compress. On a white 64x64, it takes two bits.
 * (Clearly most cases will be more complicated, but you should get the
 * basic idea from that.)
 */
void fiddle_edges(unsigned char *image,int w,int h,int w64,int h64)
{
int flipped=0,x,y,pw=w64*64,ph=h64*64,t;

/* first, if both w and h are multiples are 64, quit now. */
if((w&63)==0 && (h&63)==0) return;

/* there are many possible approaches to this problem, and this one's
 * certainly not the best, but at least it's quick and easy, and it's
 * better than nothing. :-)
 *
 * So, all we do is flip the runoff area of an edge to white if more than
 * half of the pixels on that edge are white. Then for the bottom-right
 * runoff square (if there is one), we flip it if we flipped both edges.
 */

if(w&63)
  {
  for(y=t=0;y<h;y++) t+=image[y*pw+w-1];
  if(t*2>h)
    {
    flipped++;
    for(y=0;y<h;y++)
      for(x=w;x<pw;x++)
        image[y*pw+x]=1;
    }
  }

if(h&63)
  {
  for(x=t=0;x<w;x++) t+=image[(h-1)*pw+x];
  if(t*2>w)
    {
    flipped++;
    for(y=h;y<ph;y++)
      for(x=0;x<w;x++)
        image[y*pw+x]=1;
    }
  }

if(flipped==2)	/* also implies (w&64) && (h&64) */
  {
  for(y=h;y<ph;y++)
    for(x=w;x<pw;x++)
        image[y*pw+x]=1;
  }
}




int main(int argc,char *argv[])
{
static unsigned char buf[128];
FILE *in,*out;
unsigned char *image;
int w,h,w8,w64,h64;
int x,y,c;
int i,mask;

in=stdin;
out=stdout;

if(argc>3 || (argc==2 && strcmp(argv[1],"-h")==0))
  {
  fprintf(stderr,"usage: pbmtomrf <in.pbm >out.mrf\n");
  fprintf(stderr,"   or  pbmtomrf  in.pbm >out.mrf\n");
  fprintf(stderr,"   or  pbmtomrf  in.pbm  out.mrf\n");
  exit(1);
  }

if(argc==2 || argc==3)
  {
  if((in=fopen(argv[1],"rb"))==NULL)
    fprintf(stderr,"pbmtomrf: couldn't open `%s'.\n",argv[1]),exit(1);
  if(argc==3)
    if((out=fopen(argv[2],"wb"))==NULL)
      fprintf(stderr,"pbmtomrf: couldn't open `%s'.\n",argv[2]),exit(1);
  }

fgets(buf,sizeof(buf),in);

if(strncmp(buf,"P4\n",3)!=0)
  fprintf(stderr,"pbmtomrf: not a raw PBM file.\n"),exit(1);

do
  fgets(buf,sizeof(buf),in);
while(buf[0]=='#');

sscanf(buf,"%d%d\n",&w,&h);

/* w64 is units-of-64-bits width, h64 same for height */
w64=(w+63)/64;
h64=(h+63)/64;
w8=(w+7)/8;		/* bytes per PBM line */

if((image=calloc(w64*h64*64*64,1))==NULL)
  {
  fprintf(stderr,"pbmtomrf: out of memory.\n");
  if(in!=stdin) fclose(in);
  if(out!=stdout) fclose(out);
  exit(1);
  }

fprintf(out,"MRF1");
fprintf(out,"%c%c%c%c",w>>24,w>>16,w>>8,w&255);
fprintf(out,"%c%c%c%c",h>>24,h>>16,h>>8,h&255);
fputc(0,out);	/* option byte, unused for now */

/* get bytemap image rounded up into mod 64x64 squares */

for(y=0;y<h;y++)
  {
  for(x=0;x<w8;x++)
    {
    c=fgetc(in);
    
    for(i=0,mask=128;i<8 && x*8+i<w;i++,mask>>=1)
      image[y*(w64*64)+x*8+i]=((c&mask)?0:1);
    }
  }

if(in!=stdin) fclose(in);

/* if necessary, alter the unused outside area to aid compression of
 * edges of image.
 */

fiddle_edges(image,w,h,w64,h64);

/* now recursively check squares. */

/* init bit output */
bit_init(out);

for(y=0;y<h64;y++)
  for(x=0;x<w64;x++)
    do_square(image,x*64,y*64,w64*64,64);

bit_flush();

free(image);

if(out!=stdout) fclose(out);

exit(0);
}
