/* mkasmver - emulate the grep/awk/date-using code in ZCN's Makefile on MS-DOG.
 * PD by RJM 1999-03-21.
 *
 * usage: mkasmver <start.z >asmver.z
 */

#include <stdio.h>
#include <string.h>
#include <time.h>


/* Get start.z's `zcnver' line, which has this basic format:
 * "zcnver	equ 0102h	;as returned by zfversion"
 * ...that is, "zcnver\tequ 0x0yh", where x and y are major/minor version
 * numbers. start.z is on stdin.
 */

char *get_version(void)
{
static char buf[128];
char *ptr;

while(fgets(buf,sizeof(buf),stdin)!=NULL)
  {
  if(strncmp(buf,"zcnver",6)==0)
    {
    /* we work back from the `h'. */
    if((ptr=strchr(buf+6,'h'))==NULL || ptr[-2]!='0')
      fprintf(stderr,"mkasmver: bad `zcnver' in start.z!\n"),exit(1);
    
    ptr[-2]='.';
    *ptr=0;
    return(ptr-3);
    }
  }

fprintf(stderr,"mkasmver: missing `zcnver' line in start.z!\n");
exit(1);
}


char *get_datetime(void)
{
static char buf[128];
time_t now=time(NULL);
struct tm *ctime;

/* this should be a can't happen, I think... */
if((ctime=localtime(&now))==NULL)
  fprintf(stderr,"mkasmver: couldn't split time_t!\n"),exit(1);

sprintf(buf,"%d-%02d-%02d %02d:%02d",
	1900+ctime->tm_year,ctime->tm_mon+1,ctime->tm_mday,
        ctime->tm_hour,ctime->tm_min);

return(buf);
}



int main()
{
/* 	echo "defb 'ZCN v'" >asmver.z */
printf("defb 'ZCN v'\n");

/*	grep '^zcnver.*equ' start.z | \
	  awk '{ printf("defb '\''%d.%d'\''\n",int($$3/100),$$3%100) }' \
	  >>asmver.z */
printf("defb '%s'\n",get_version());

/*	echo ';build date/time' >>asmver.z
	date "+defb ' (%Y-%m-%d %H:%M)'" >>asmver.z */
printf(";build date/time\n");
printf("defb ' (%s)'\n",get_datetime());

exit(0);
}
