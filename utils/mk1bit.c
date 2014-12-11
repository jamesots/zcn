/* make 1-bit sample from 8-bit one */

#include <stdio.h>

int main()
{
int c,mask=128,out=0,total=0;

while((c=getchar())!=EOF)
  {
  if(c>=128) out|=mask;
  if(mask!=1)
    mask>>=1;
  else
    {
    mask=128;
    putchar(out);
    total++;
    out=0;
    }
  }

/* round up to nearest 128 boundary to avoid rubbish at end */
while(total%128>0)
  {
  putchar(0);
  total++;
  }
}
