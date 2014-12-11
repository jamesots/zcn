/* generate circ60.z, containing array of positions for clock face */

#include <stdio.h>
#include <math.h>


/* CIRCLE_OX + CIRCLE_RAD[1234] must be less than 256 */
#define CIRCLE_OX	64.
#define CIRCLE_OY	32.
#define CIRCLE_RAD1	 9.	/* hour hand */
#define CIRCLE_RAD2	15.	/* minute hand */
#define CIRCLE_RAD3	17.	/* second hand */
#define CIRCLE_RAD4	20.	/* clock `face' lines - one line end */
#define CIRCLE_RAD5	23.	/* ...and the other */
#define ECCENTRICITY	1.75	/* it's an ellipse really, but how wide? */


main()
{
double pi=3.1415927,f,stp,x,y;
int g;

printf(";circ60.z - array of positions for clock face.\n");
printf(";automatically generated (edits will be lost!)\n\n");

stp=pi/30;

/* hour hand */
printf("\n\nhour60:\n");

for(g=0,f=-pi/2.;g<60;g++,f+=stp)
  {
  x=CIRCLE_OX+ECCENTRICITY*cos(f)*CIRCLE_RAD1;
  y=CIRCLE_OY+sin(f)*CIRCLE_RAD1;
  printf("defb %02d,%02d\n",(int)(x+0.5),(int)(y+0.5));
  }

/* minute hand */
printf("\n\nmin60:\n");

for(g=0,f=-pi/2.;g<60;g++,f+=stp)
  {
  x=CIRCLE_OX+ECCENTRICITY*cos(f)*CIRCLE_RAD2;
  y=CIRCLE_OY+sin(f)*CIRCLE_RAD2;
  printf("defb %02d,%02d\n",(int)(x+0.5),(int)(y+0.5));
  }

/* second hand */
printf("\n\nsec60:\n");

for(g=0,f=-pi/2.;g<60;g++,f+=stp)
  {
  x=CIRCLE_OX+ECCENTRICITY*cos(f)*CIRCLE_RAD3;
  y=CIRCLE_OY+sin(f)*CIRCLE_RAD3;
  printf("defb %02d,%02d\n",(int)(x+0.5),(int)(y+0.5));
  }

/* clock face dots etc. */
printf("\n\nface12:\n");

for(g=0,f=-pi/2.;g<60;g+=5,f+=stp*5)
  {
  x=CIRCLE_OX+ECCENTRICITY*cos(f)*CIRCLE_RAD4;
  y=CIRCLE_OY+sin(f)*CIRCLE_RAD4;
  printf("defb %02d,%02d\n",(int)(x+0.5),(int)(y+0.5));
  
  x=CIRCLE_OX+ECCENTRICITY*cos(f)*CIRCLE_RAD5;
  y=CIRCLE_OY+sin(f)*CIRCLE_RAD5;
  printf("defb %02d,%02d\n",(int)(x+0.5),(int)(y+0.5));
  }

exit(0);
}
