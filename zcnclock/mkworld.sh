#!/bin/sh
#
# mkworld - make city-location bitmap for zcnclock, using xearth.
# (may be assuming /bin/sh is ksh or bash)

do_xearth()
{
echo $i/12...
xearth -ppm -pos fixed/$1/$2 \
  -mag 2 -noshade -nostars -nomarkers -size 64/32 |\
  ppmtopgm |pgmtopbm -th |pnminvert >mkwtmp.$i
i=`expr $i + 1`
}


trap 'rm -f mkwtmp.*' 0 1 3 15

echo making cities.mrf...

i=1

# london
do_xearth 51 0

# paris
do_xearth 49 2

# cairo
do_xearth 30 31

# moscow
do_xearth 56 38

# new delhi
do_xearth 29 77

# hong kong
do_xearth 22 114

# tokyo
do_xearth 36 140

# sydney
do_xearth -34 151

# auckland
do_xearth -38 175

# los angeles
do_xearth 34 -118

# new york
do_xearth 41 -74

# rio de janeiro
do_xearth -22 -43

# combine them all
pnmcat -lr mkwtmp.[123456] >mkwtmp.a
pnmcat -lr mkwtmp.{7,8,9,10,11,12} >mkwtmp.b
pnmcat -tb mkwtmp.a mkwtmp.b |./pbmtomrf >cities.mrf

echo done.
