/* make 4-bit sample from 8-bit one */

#include <stdio.h>

int main()
{
int c1,c2,total=0;

while((c1=getchar())!=EOF)
  {
  c2=getchar(); if(c2==EOF) c2=7;
  
  putchar((c1>>4)*16+(c2>>4));
  total++;
  }

/* round up to nearest 128 boundary to avoid rubbish at end */
while(total%128>0)
  {
  putchar(0x77);
  total++;
  }
}
