/* pbmtosub - convert raw PBM file to .SUB using `bmp'. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc,char *argv[])
{
static unsigned char buf[128],*ptr;
FILE *in,*out;
unsigned char *image;
int w,h,w8,w4,h6,w8x8;
int x,y,c;
int i,mask;
int linelen;

in=stdin;
out=stdout;

if(argc>3 || (argc==2 && strcmp(argv[1],"-h")==0))
  {
  fprintf(stderr,"usage: pbmtosub <in.pbm >out.sub\n");
  fprintf(stderr,"   or  pbmtosub  in.pbm >out.sub\n");
  fprintf(stderr,"   or  pbmtosub  in.pbm  out.sub\n");
  exit(1);
  }

if(argc==2 || argc==3)
  {
  if((in=fopen(argv[1],"rb"))==NULL)
    fprintf(stderr,"pbmtosub: couldn't open `%s'.\n",argv[1]),exit(1);
  if(argc==3)
    if((out=fopen(argv[2],"wb"))==NULL)
      fprintf(stderr,"pbmtosub: couldn't open `%s'.\n",argv[2]),exit(1);
  }

fgets(buf,sizeof(buf),in);

if(strncmp(buf,"P4\n",3)!=0)
  fprintf(stderr,"pbmtosub: not a raw PBM file.\n"),exit(1);

do
  fgets(buf,sizeof(buf),in);
while(buf[0]=='#');

sscanf(buf,"%d%d\n",&w,&h);

/* w4 is rounded-up nibble width, h6 rounded-up height in six-pix-high chars */
w4=(w+3)/4;
h6=(h+5)/6;
w8=(w+7)/8;		/* bytes per PBM line */
w8x8=w8*8;

if((image=calloc(w8*8*h6*6,1))==NULL)
  {
  fprintf(stderr,"pbmtosub: out of memory.\n");
  if(in!=stdin) fclose(in);
  if(out!=stdout) fclose(out);
  exit(1);
  }

/* get bytemap image */

for(y=0;y<h;y++)
  {
  for(x=0;x<w8;x++)
    {
    c=fgetc(in);
    
    for(i=0,mask=128;i<8 && x*8+i<w;i++,mask>>=1)
      image[y*w8x8+x*8+i]=((c&mask)?1:0);
    }
  }

if(in!=stdin) fclose(in);

/* now convert to nibbles etc. */

printf("crlf 0\r\n");

if(w4>480/4) w4=480/4;

strcpy(buf,"bmp ");
linelen=4;

for(y=0;y<h6;y++)
  {
  for(x=0;x<w4;x++)
    {
    /* static uninitialised is zero anyway, but what the heck */
    static unsigned char newnibs[6],oldnibs[6]={0,0,0,0,0,0};
    
    ptr=image+y*6*w8x8+x*4;
    for(i=0;i<6;i++,ptr+=w8x8)
      newnibs[i]=8*ptr[0]+4*ptr[1]+2*ptr[2]+ptr[3];
    
    if(memcmp(newnibs,"\0\0\0\0\0\0",6)==0)
      buf[linelen++]='_';
    else
      if(memcmp(newnibs,oldnibs,6)==0)
        buf[linelen++]='=';
      else
        {
        /* at any point when the rest is zero, we can stop.
         * But since we'll need a space after to end the char,
         * this is only worth it if i is less than 5.
         * Also, we've already effectively tested it for i==0.
         */
        for(i=0;i<6;i++)
          {
          if(i>0 && i<5 && memcmp(newnibs+i,"\0\0\0\0\0\0",6-i)==0)
            {
            buf[linelen++]=' ';
            break;
            }
          
          buf[linelen++]='0'+newnibs[i]+7*(newnibs[i]>9);
          }
        }
    
    memcpy(oldnibs,newnibs,6);
    
    if(linelen>70)
      {
      buf[linelen]=0;
      fprintf(out,"%s\r\n",buf);
      strcpy(buf,"!! ");
      linelen=3;
      memset(oldnibs,0,6);	/* make sure next one can't match */
      }
    }
  
  buf[linelen++]=((w4==480/4)?' ':'.');
  }

buf[linelen]=0;
fprintf(out,"%s\r\n",buf);

printf("crlf 1\r\n");

free(image);

if(out!=stdout) fclose(out);

exit(0);
}
