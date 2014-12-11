/* pbmtozap - convert 32x24 PBM file to skeletal ZAP file */

#include <stdio.h>
#include <string.h>


int main()
{
static char buf[1024];
int w,h,x,y;

fgets(buf,sizeof(buf),stdin);
if(strcmp(buf,"P4\n")!=0)
  fprintf(stderr,"pbmtozap: must be raw PBM file\n"),exit(1);

fgets(buf,sizeof(buf),stdin);
sscanf(buf,"%d %d",&w,&h);
if(w!=32 || h!=24)
  fprintf(stderr,"pbmtozap: must be exactly 32x24\n"),exit(1);

printf("progname XXX\n");
printf("progfile XXX\n");
printf("filetype XXX\n");
for(y=0;y<h;y++)
  {
  printf("bmphex ");
  for(x=0;x<4;x++)
    printf("%02x",getchar());
  printf("\n");
  }

exit(0);
}
