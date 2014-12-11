/* crapmext - extract pmarc files stored uncompressed (i.e. with `/n')
 * PD by RJM 980316
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define lose(n) for(f=0;f<(n);f++) fgetc(in)


int main(int argc,char *argv[])
{
static unsigned char buf[16384];
FILE *in,*out=NULL;
int f,a,b,c,headersiz,siz,dontwrite;

if(argc!=2)
  {
  printf("usage: crapmext pma_file_to_unpack\n\n");
  printf("NB: crapmext *only works for uncompressed files*. Be sure to use\n");
  printf("e.g. `pmarc foo *.*/n' to create the archive.\n");
  exit(1);
  }

if((in=fopen(argv[1],"rb"))==NULL)
  {
  fprintf(stderr,"couldn't open archive file.\n");
  exit(1);
  }

while((headersiz=fgetc(in))!=EOF)
  {
  if(headersiz==0 || headersiz==0x1a) break;
  
  dontwrite=0;
  
  fread(buf,1,6,in);
  if(strncmp(buf+1,"-pm0-",5)!=0)
    dontwrite=1;
  
  /* next word is num bytes in file */
  siz=fgetc(in);
  siz+=256*fgetc(in);
  
  lose(13);
  
  fread(buf,1,headersiz-22,in);
  buf[headersiz-22]=0;
  
  for(f=0;f<strlen(buf);f++) buf[f]=tolower(buf[f]);
  
  lose(2);
  
  if(dontwrite)
    {
    printf("skipping compressed file `%s' ",buf);
    printf("(files must be uncompressed!)\n");
    while(siz>0)
      siz-=fread(buf,1,sizeof(buf)<siz?sizeof(buf):siz,in);
    continue;
    }
  
  if((out=fopen(buf,"wb"))==NULL)
    printf("couldn't open `%s', skipping...\n",buf);
  else
    {
    printf("writing `%s'\n",buf);
    while(siz>0)
      {
      int output,count=fread(buf,1,sizeof(buf)<siz?sizeof(buf):siz,in);
      if((output=fwrite(buf,1,count,out))!=count)
        {
        printf("error writing output file - %d of %d bytes written\n",
        	output,count);
        if(output==0) fclose(out),fclose(in),exit(1);
        }
      siz-=count;
      }
    fclose(out);
    }
  }

fclose(in);
exit(0);
}
