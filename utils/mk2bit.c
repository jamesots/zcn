/* make 2-bit sample from 8-bit one */

#include <stdio.h>

int main()
{
int c1,c2,c3,c4,total=0;

while((c1=getchar())!=EOF)
  {
  c2=getchar(); if(c2==EOF) c2=128;
  c3=getchar(); if(c3==EOF) c2=128;
  c4=getchar(); if(c4==EOF) c2=128;
  
#define MKSAM(x)	((x)>>6)
  
  putchar(MKSAM(c1)*64+MKSAM(c2)*16+MKSAM(c3)*4+MKSAM(c4));
  total++;
  }

/* round up to nearest 128 boundary to avoid rubbish at end */
while(total%128>0)
  {
  putchar(0xaa);
  total++;
  }
}
