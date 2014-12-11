/* srbmpgen - generate the 480x64 slide rule bitmap. */

#include <stdio.h>
#include <string.h>
#include <math.h>

#define RULE_XPOS	0
#define RULE_YPOS	0
#define RULE_WIDTH	480

/* the static bits above/below the slider don't move.
 * the separating pixel lines are part of the slider, but
 * the empty space left behind has lines in the same vertical position.
 */
#define STATIC_HEIGHT	12
#define SLIDER_HEIGHT	40

#define SCALE_XPOS	(4*4)
#define SCALE_WIDTH	(RULE_WIDTH-4*8)

#define SLIDER_TOP_YPOS	(RULE_YPOS+STATIC_HEIGHT)
#define SLIDER_BOT_YPOS	(RULE_YPOS+STATIC_HEIGHT+SLIDER_HEIGHT-1)

/* the -1 is because these define a line's endpoint */
#define MARK_BIG	(4-1)
#define MARK_HALF	(3-1)
#define MARK_MID	(2-1)
#define MARK_SMALL	(1-1)


static unsigned char bmp[512*64];		/* bytemap */

static unsigned char zcnfont[(128-32)*6];	/* font data */




/* stuff for mrf output */

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


void write_bmp(void)
{
FILE *out;
int w,h,w64,h64;
int x,y;

if((out=fopen("slidebmp.mrf","wb"))==NULL)
  fprintf(stderr,"couldn't write slide-rule bitmap\n"),exit(1);

/* w64 is units-of-64-bits width, h64 same for height */
w=512; h=64; w64=w/64; h64=h/64;

fprintf(out,"MRF1");
fprintf(out,"%c%c%c%c",w>>24,w>>16,w>>8,w&255);
fprintf(out,"%c%c%c%c",h>>24,h>>16,h>>8,h&255);
fputc(0,out);

bit_init(out);

for(y=0;y<h64;y++)
  for(x=0;x<w64;x++)
    do_square(bmp,x*64,y*64,w64*64,64);

bit_flush();

fclose(out);
}




void read_font(void)
{
FILE *in;
int f;

if((in=fopen("../src/4x6font.dat","rb"))==NULL)
  fprintf(stderr,"couldn't read ZCN font!\n"),exit(1);

fread(zcnfont,1,sizeof(zcnfont),in);
fclose(in);

/* fix a few bits */
/* first, make pipe solid and shift it left */
for(f=0;f<5;f++)
  zcnfont[('|'-32)*6+f]=0x88;

/* we can use lowercase for non-ZCN chars */
/* a small x (for right-hand labels) */
zcnfont[('x'-32)*6+0]=0;
zcnfont[('x'-32)*6+1]=0;
zcnfont[('x'-32)*6+2]=0;
zcnfont[('x'-32)*6+3]=0xa;
zcnfont[('x'-32)*6+4]=0x4;
zcnfont[('x'-32)*6+5]=0xa;

/* s is the squared symbol (superscripted 2) */
zcnfont[('s'-32)*6+0]=0x8;
zcnfont[('s'-32)*6+1]=0x4;
zcnfont[('s'-32)*6+2]=0x8;
zcnfont[('s'-32)*6+3]=0xc;
zcnfont[('s'-32)*6+4]=0;
zcnfont[('s'-32)*6+5]=0;

/* o is the one-over symbol (as in 1/x) */
zcnfont[('o'-32)*6+0]=0x8;
zcnfont[('o'-32)*6+1]=0x9;
zcnfont[('o'-32)*6+2]=0xa;
zcnfont[('o'-32)*6+3]=0x2;
zcnfont[('o'-32)*6+4]=0x4;
zcnfont[('o'-32)*6+5]=0;
}


void drawpixel(int x,int y)
{
bmp[512*y+x]=0;
}


/* only copes with vertical and horizontal lines, but that's all it's
 * needed for.
 */
void drawline(int x1,int y1,int x2,int y2)
{
int f,tmp;

if(x1==x2)
  {
  /* vertical */
  if(y2<y1) tmp=y1,y1=y2,y2=tmp;
  for(f=y1;f<=y2;f++) drawpixel(x1,f);
  return;
  }

if(y1==y2)
  {
  /* horizontal */
  if(x2<x1) tmp=x1,x1=x2,x2=tmp;
  for(f=x1;f<=x2;f++) drawpixel(f,y1);
  return;
  }

fprintf(stderr,"drawline() only supports vert/horiz lines!\n");
exit(1);
}


void drawtext(int ox,int oy,char *text)
{
int f,x,y,c,dat,mask;
unsigned char *datptr;

for(f=0;f<strlen(text);f++)
  {
  c=text[f];
  if(c<32 || c>126) c='_';
  datptr=zcnfont+(c-32)*6;
  for(y=0;y<6;y++)
    {
    dat=*datptr++;
    for(x=0,mask=8;x<4;x++,mask>>=1)
      if(dat&mask) drawpixel(ox+f*4+x,oy+y);
    }
  }
}


void draw_logscale(int ox,int oy,int axes_point_up)
{
int f,x,orient;
double a;

orient=(axes_point_up?-1:1);

/* de facto rules judging from my pocket slide rule:
 * - scales from 1 to 10 :-)
 * - halves all the way
 * - tenths all the way
 * - 1/20 for 2..5
 * - 1/50 for 1..2
 * so we work in hundredths and figure things out as needed.
 */
for(f=0,a=1.;f<=900;f++,a+=0.01)
  {
  x=(int)(log10(a)*SCALE_WIDTH);
  if(x>=SCALE_WIDTH) x=SCALE_WIDTH-1;
  x+=ox;
  
  if(f<100 && f%2==0)
    drawline(x,oy,x,oy+orient*MARK_SMALL);
  if(f>=100 && f<4*100 && f%5==0)
    drawline(x,oy,x,oy+orient*MARK_SMALL);
  if(f%10==0)
    drawline(x,oy,x,oy+orient*MARK_MID);
  if(f%50==0)
    drawline(x,oy,x,oy+orient*MARK_HALF);
  if(f%100==0)
    {
    static char buf[10];
    drawline(x,oy,x,oy+orient*MARK_BIG);
    sprintf(buf,"%d",1+f/100);
    drawtext(x-1-2*(f==900),oy+orient*MARK_BIG+(orient==1?2:-6),buf);
    }
  }
}


void draw_revlogscale(int ox,int oy,int axes_point_up)
{
int f,x,orient;
double a;

orient=(axes_point_up?-1:1);

for(f=0,a=1.;f<=900;f++,a+=0.01)
  {
  x=(int)(log10(a)*SCALE_WIDTH);
  if(x>=SCALE_WIDTH) x=SCALE_WIDTH-1;
  x=ox+SCALE_WIDTH-1-x;
  
  if(f<100 && f%2==0)
    drawline(x,oy,x,oy+orient*MARK_SMALL);
  if(f>=100 && f<4*100 && f%5==0)
    drawline(x,oy,x,oy+orient*MARK_SMALL);
  if(f%10==0)
    drawline(x,oy,x,oy+orient*MARK_MID);
  if(f%50==0)
    drawline(x,oy,x,oy+orient*MARK_HALF);
  if(f%100==0)
    {
    static char buf[10];
    drawline(x,oy,x,oy+orient*MARK_BIG);
    sprintf(buf,"%d",1+f/100);
    /* no extra spacing for 10, too tight if we do that */
    drawtext(x-1,oy+orient*MARK_BIG+(orient==1?2:-6),buf);
    }
  }
}


void draw_2logscale(int ox,int oy,int axes_point_up)
{
int f,x,orient;
double a;

orient=(axes_point_up?-1:1);

/* alternate rules for x^2...
 * - scales from 1 to 100
 * for 1..10:
 * - each one labelled
 * - 1/20 for 1..3
 * - tenths for 1..6
 * - fifths for 1..10
 * - halves for 1..6
 * for 10..100:
 * - every 10th one labelled
 * - halves for 10..30
 * - ones :-) for 10..60
 * - every two for 10..100
 * - every five (effective halves) for 10..60
 * here we work in 1/20ths. to simpify things (as if) we start at 20 (for 1).
 */
for(f=20,a=1.;f<=100*20;f++,a+=0.05)
  {
  x=(int)(log10(a)/2*SCALE_WIDTH);
  if(x>=SCALE_WIDTH) x=SCALE_WIDTH-1;
  x+=ox;
  
  if(f<10*20)
    {
    /* for 1..9.999 */
    if(f<3*20)
      drawline(x,oy,x,oy+orient*MARK_SMALL);
    if(f<6*20 && f%2==0)
      drawline(x,oy,x,oy+orient*MARK_MID);
    if(f%4==0)
      drawline(x,oy,x,oy+orient*MARK_MID);
    if(f<6*20 && f%10==0)
      drawline(x,oy,x,oy+orient*MARK_HALF);
    }
  else
    {
    /* for 10..100 */
    if(f<30*20 && f%10==0)
      drawline(x,oy,x,oy+orient*MARK_SMALL);
    if(f<60*20 && f%20==0)
      drawline(x,oy,x,oy+orient*MARK_MID);
    if(f%40==0)
      drawline(x,oy,x,oy+orient*MARK_MID);
    if(f<60*20 && f%100==0)
      drawline(x,oy,x,oy+orient*MARK_HALF);
    }
  
  if((f<10*20 && f%20==0) || f%200==0)
    {
    static char buf[10];
    drawline(x,oy,x,oy+orient*MARK_BIG);
    sprintf(buf,"%d",f/20);
    drawtext(x-1-2*(f>=10*20)-2*(f==100*20),
    		oy+orient*MARK_BIG+(orient==1?2:-6),buf);
    }
  }
}


int main()
{
read_font();

memset(bmp,1,512*64);	/* set bytemap to white initially */

drawline(RULE_XPOS,SLIDER_TOP_YPOS,RULE_XPOS+RULE_WIDTH-1,SLIDER_TOP_YPOS);
drawline(RULE_XPOS,SLIDER_BOT_YPOS,RULE_XPOS+RULE_WIDTH-1,SLIDER_BOT_YPOS);

draw_2logscale(SCALE_XPOS,SLIDER_TOP_YPOS-1,1);
drawtext(SCALE_XPOS-4*3,SLIDER_TOP_YPOS-8,"A");
drawtext(SCALE_XPOS+SCALE_WIDTH+4*2,SLIDER_TOP_YPOS-8,"xs");
draw_2logscale(SCALE_XPOS,SLIDER_TOP_YPOS+1,0);
drawtext(SCALE_XPOS-4*3,SLIDER_TOP_YPOS+3,"B");
drawtext(SCALE_XPOS+SCALE_WIDTH+4*2,SLIDER_TOP_YPOS+3,"xs");

draw_revlogscale(SCALE_XPOS,SLIDER_TOP_YPOS+24,1);
drawtext(SCALE_XPOS-4*3,SLIDER_TOP_YPOS+24-7,"C|");
drawtext(SCALE_XPOS+SCALE_WIDTH+4*2-2,SLIDER_TOP_YPOS+24-7,"ox");

draw_logscale(SCALE_XPOS,SLIDER_BOT_YPOS-1,1);
drawtext(SCALE_XPOS-4*3,SLIDER_BOT_YPOS-8,"C");
drawtext(SCALE_XPOS+SCALE_WIDTH+4*2,SLIDER_BOT_YPOS-8,"x");
draw_logscale(SCALE_XPOS,SLIDER_BOT_YPOS+1,0);
drawtext(SCALE_XPOS-4*3,SLIDER_BOT_YPOS+3,"D");
drawtext(SCALE_XPOS+SCALE_WIDTH+4*2,SLIDER_BOT_YPOS+3,"x");

/* lines at edges */
/* these should probably use XPOS etc. to be consistent, but sod it :-) */
drawline(0,0,0,63);
drawline(479,0,479,63);

write_bmp();

exit(0);
}
