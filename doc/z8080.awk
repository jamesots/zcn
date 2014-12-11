#!/usr/bin/awk -f
#
#* z8080 - Z80 to 8080-compatible Z80 filter
#
# By Russell Marks. This program is public domain. You can do anything
#  you want with it.
#
# See `z8080.txt' for bugs, caveats, etc.

#* change log
#
# 960919 02:13 - made readable as an Emacs outline (with outline minor mode).
# 960916 13:01 - a few bugfixes regarding ix/iy, labels, and comments.
# 960916 01:23 - first (hopefully...) working version.


#* BEGIN action

BEGIN {
labelprefix="z8_"

# this works on gawk...
errdest="/dev/stderr"

# for other awks or systems without a /dev/stderr, try this:
#errdest="/dev/tty"
# it's crude and sometimes wrong, but should generally suffice.
}


#* hack input line for further processing
#
# we need this to remove labels, comments, etc. and effectively use
# a comma as a secondary FS, to make the rest of the processing easier.

{
# set label
label=labelprefix NR

# print any label
if($1 ~ /^[^;'"]*:$/) print $1

# remove any label from $0
sub(/^[^;'"]*:/,"")

# save $0 now (to be output if no replacement is required)
orig=$0

# remove comments
# this is ok even for comments like `;;; this' as sub matches the
# leftmost longest match.
# it can screw up on some defb strings, but defb etc. lines are dealt
# with specially below to prevent this and other similar lossage.
sub(/;.*$/,"")

# this converts commas to spaces, important for parsing later
# yes I do mean sub not gsub, as there should only ever be one comma
sub(/,/," ")

# get it to ignore defb/defw etc. which could otherwise screw things up
# (it will then default to just printing `orig')
# the regexp matches db/dw/ds/dm/defb/defw/defs/defm, all the data-
# inserting etc. pseudo-ops I know of in various Z80 assemblers
if($1~/^d(ef)?[bwsm]$/) { $0="" }
}


#* main code, to translate unsupported instructions

#** impossible-on-8080 ops

# check for illegal register use
# the way this is done is a bit bogus but happens to be good enough

# need the hairy ix/iy matching as a label could contain `ix' or `iy';
# the regexp tests for ix/iy alone, or ix/iy in (ix) or (ix[+-]n) contexts.
# These are the only ways ix/iy can be used, and this way of spotting
# them doesn't clash with labels.

$2~/^i[xy]$|\(i[xy][)+-]/ || $3~/^i[xy]$|\(i[xy][)+-]/ {
print NR ": no ix/iy!" > errdest; exit 1
}

# I and R can only be used in ld a,[ir] and ld [ir],a - much easier :-)

$2=="i" || $3=="i"	{ print NR ": no `i'!" > errdest; exit 1 }
$2=="r" || $3=="r"	{ print NR ": no `r'!" > errdest; exit 1 }

# im [012]
$1=="im"	{ print NR ": impossible!" > errdest; exit 1 }

# in ?,(c) and out (c),?
# yes, with self-modifying code these are possible, but:
# - it could be code to run from ROM.
# - this filter is really for CP/M code, which won't use in/out.
# still, maybe I'll add self-modifying versions later. (XXX)
$1=="in" && $3=="(c)"  { print NR ": impossible!" > errdest; exit 1 }
$1=="out" && $2=="(c)" { print NR ": impossible!" > errdest; exit 1 }

# exx
$1=="exx"	{ print NR ": no alt. regs!" > errdest; exit 1 }

# ex af,af'
$1=="ex" && $2=="af" && $3=="af'" { print NR ": no alt. regs!" > errdest
					exit 1 }

# reti/retn
$1=="reti" || $1=="retn" { print NR ": no reti/retn!" > errdest;exit 1 }


#** block ops

$1=="ldir" {
print "push af"
print label ":"
print "ld a,(hl)"
# no ld (de),a remember...
print "ex de,hl"
print "ld (hl),a"
print "ex de,hl"
print "inc hl"
print "inc de"
print "dec bc"
print "ld a,b"
print "or c"
print "jp nz," label
print "pop af"
next
}

$1=="lddr" {
print "push af"
print label ":"
print "ld a,(hl)"
print "ex de,hl"
print "ld (hl),a"
print "ex de,hl"
print "dec hl"
print "dec de"
print "dec bc"
print "ld a,b"
print "or c"
print "jp nz," label
print "pop af"
next
}


# XXX lots of block ops still to do...

$1=="ldi"	{ print NR ": unimplemented!" > errdest; next }
$1=="cpi"	{ print NR ": unimplemented!" > errdest; next }
$1=="ini"	{ print NR ": unimplemented!" > errdest; next }
$1=="outi"	{ print NR ": unimplemented!" > errdest; next }

$1=="ldd"	{ print NR ": unimplemented!" > errdest; next }
$1=="cpd"	{ print NR ": unimplemented!" > errdest; next }
$1=="ind"	{ print NR ": unimplemented!" > errdest; next }
$1=="outd"	{ print NR ": unimplemented!" > errdest; next }

# ldir done
$1=="cpir"	{ print NR ": unimplemented!" > errdest; next }
$1=="inir"	{ print NR ": unimplemented!" > errdest; next }
$1=="otir"	{ print NR ": unimplemented!" > errdest; next }

# lddr done
$1=="cpdr"	{ print NR ": unimplemented!" > errdest; next }
$1=="indr"	{ print NR ": unimplemented!" > errdest; next }
$1=="otdr"	{ print NR ": unimplemented!" > errdest; next }


#** shift ops (rl, rr, etc.)

# first rl/rr/rlc/rrc a, which can be trivially mapped to 8080 ops
$1=="rl" && $2=="a"	{ print "rla"; next }
$1=="rlc" && $2=="a"	{ print "rlca"; next }
$1=="rr" && $2=="a"	{ print "rra"; next }
$1=="rrc" && $2=="a"	{ print "rrca"; next }

# now the more general cases
$1=="rl" || $1=="rlc" || $1=="rr" || $1=="rrc" {
# the intuitive way to do this is push/pop af and do it via rla etc.,
# but this loses the flag effects, which we want. we need to save
# A some other way. since all we can use is the stack, this is yucky.
# can't swap regs with the three-xor method, as we can only xor
# values into a. this leaves us needing to push af, then somehow
# only read a. the quickest way seems to be push hl/push af/pop hl/
# ld a,h/pop hl.
#
# of course, if we're doing rl h or rl l, we need to avoid push/popping
# hl. :-)

savereg="hl"; savereghi="h"
if($2=="h" || $2=="l") { savereg="bc"; savereghi="b" }

print "push " savereg
print "push af"
print "ld a," $2
print $1 "a"		# to make rla or whatever
print "ld " $2 ",a"
print "pop " savereg
print "ld a," savereghi
print "pop " savereg

next
}


$1=="sla" || $1=="sra" || $1=="srl" {	# `sll' doesn't exist
					# well, it does, but it's undocumented
savereg="hl"; savereghi="h"
if($2=="h" || $2=="l") { savereg="bc"; savereghi="b" }

if($2!="a")
  {
  print "push " savereg
  print "push af"
  print "ld a," $2
  }

# now do the operation on A

if($1=="sla")
  {
  # shift left, bit 7 to carry, bit 0 becomes 0
  print "and a"		# no carry
  print "rla"
  }
else if($1=="sra")
  {
  # shift right, bit 0 to carry, bit 7 remains in bit 7 (as well as
  # being copied to bit 6!). idea here is to get bit 7 into carry, then
  # rra, which has the desired effect.
  
  # this needed if working on A, as won't have pushed savereg
  if($1=="a") print "push " savereg
  print "ld " savereglo ",a"
  print "rla"			# bit 7 -> carry
  print "ld a," savereglo	# restore A
  print "rra"
  if($1=="a") print "pop " savereg
  }
else # must be srl
  {
  # shift right, bit 0 to carry, bit 7 becomes 0
  print "and a"
  print "rra"
  }

if($2!="a")
  {
  print "ld " $2 ",a"
  print "pop " savereg
  print "ld a," savereghi
  print "pop " savereg
  }

next
}


# no rld/rrd for now. They aren't used very much, and they're stupidly hard.

$1=="rld" || $1=="rrd"	{ print NR ": unimplemented!" > errdest; next }


#** ld rr,(NN) for rr in bc, de, sp

$1=="ld" && ($2=="bc" || $2=="de" || $2=="sp") && $3~/^\(.*\)$/ {
# sp is trickier...
if($2=="sp")
  {
  # we need to use hl, but can't save it on the stack if we do it the
  # obvious way with `ld sp,hl'. So, we have to self-modify...
  print "push hl"
  print "ld hl," $3
  print "ld (" label "+1),hl"
  print "pop hl"
  print label ":"
  print "ld sp,0"
  }
else
  {
  print "push hl"
  print "ld hl," $3
  if($2=="bc")
    { print "ld b,h"; print "ld c,l" }
  else
    print "ex de,hl"
  print "pop hl"
  }

next
}


#** ld (NN),rr for rr in bc, de, sp

$1=="ld" && ($3=="bc" || $3=="de" || $3=="sp") && $2~/^\(.*\)$/ {

if($3=="sp")
  {
  print "push hl"
  print "ld hl,2"	# need to compensate for the push
  print "add hl,sp"
  print "ld " $2 ",hl"
  print "pop hl"
  }
else
  {
  if($3=="bc") { reghi="b";reglo="c" } else { reghi="d";reglo="e" }
  print "push hl"
  print "ld h," reghi
  print "ld l," reglo
  print "ld " $2 ",hl"
  print "pop hl"
  }

next
}


#** sbc hl,rr (rr in bc/de/hl/sp)

$1=="sbc" && $2=="hl" {

if($3=="bc") { reghi="b";reglo="c" }
if($3=="de") { reghi="d";reglo="e" }
if($3=="hl") { reghi="h";reglo="l" }

# a 16-bit invert/inc/add method works for value but not flags.
# so instead we use 2 8-bit sbc's.

# as ever, sp is a special case.

if($3=="sp")
  {
  print "push de"
  print "push af"
  
  # put sp in de
  print "ex de,hl"
  print "ld hl,4"	# compensate for the two pushes
  print "add hl,sp"
  print "ex de,hl"
  
  # restore carry
  print "pop af"
  print "push af"
  
  print "ld a,l"
  print "sbc a,e"
  print "ld l,a"
  print "ld a,h"
  print "sbc a,d"
  print "ld h,a"
  
  print "pop de"
  print "ld a,d"
  print "pop de"
  }
else
  {
  # the general case, for bc/de/hl. I s'pose I could do it faster for
  # hl, but it doesn't seem worth it really.
  # again we need the acrobatics to save A but not F.
  
  print "push de"
  print "push af"
  
  print "ld a,l"
  print "sbc a," reglo
  print "ld l,a"
  # would it be quicker to skip next sbc if nc?
  print "ld a,h"
  print "sbc a," reghi
  print "ld h,a"
  
  print "pop de"
  print "ld a,d"
  print "pop de"
  }

next
}


#** adc hl,rr (rr in bc/de/hl/sp)
# based on the above sbc, and much the same

$1=="adc" && $2=="hl" {

if($3=="bc") { reghi="b";reglo="c" }
if($3=="de") { reghi="d";reglo="e" }
if($3=="hl") { reghi="h";reglo="l" }

if($3=="sp")
  {
  print "push de"
  print "push af"
  
  # put sp in de
  print "ex de,hl"
  print "ld hl,4"	# compensate for the two pushes
  print "add hl,sp"
  print "ex de,hl"
  
  # restore carry
  print "pop af"
  print "push af"
  
  print "ld a,l"
  print "adc a,e"
  print "ld l,a"
  print "ld a,h"
  print "adc a,d"
  print "ld h,a"
  
  print "pop de"
  print "ld a,d"
  print "pop de"
  }
else
  {
  print "push de"
  print "push af"
  
  print "ld a,l"
  print "adc a," reglo
  print "ld l,a"
  print "ld a,h"
  print "adc a," reghi
  print "ld h,a"
  
  print "pop de"
  print "ld a,d"
  print "pop de"
  }

next
}


#** neg
# y'know, I wonder why Zilog added this - it's no faster or smaller than
# cpl/inc a, and has the same results unless you use `daa' after...
$1=="neg" { print "cpl";print "inc a";next }


#** bit/res/set
# these are... fun...

# for `bit', we have to set Z according to bit's value, but preserve
# carry! AAAAARRRGGHHH!!! Oh well. At least only carry has to be
# preserved, and all flags other than C and Z can be ignored.
#
# fair warning then, this is horribly inefficient so as to preserve C
# but still have the right Z value.

$1=="bit" {

mask=2^$2

print "push hl"
print "push af"		# to save A
if($3=="a") print "ld h,a"	# need to save/rstr before test...
print "ld a,0"		#  0==nop
print "jp c," label "2"
print "ld a,63"		# 63==ccf
print label "2:"
print "ld (" label "1),a"
if($3=="a") print "ld a,h"	# ...this is faster than push/pop

# do the actual test
if($3!="a")
  print "ld a," $3
print "and " mask

# now fix the carry flag.
# I'd prefer doing either `and a' or `scf', but this is the only way
# to modify carry and not the Z flag.
print "scf"
print label "1:"
print "nop"		# this becomes nop or ccf as required

# restore A and HL
print "pop hl"
print "ld a,h"
print "pop hl"

next
}


# set/res are pretty easy in comparison.

$1=="set" || $1=="res" {

mask=2^$2

if($3=="a") print "push hl"
print "push af"

if($3!="a") print "ld a," $3
if($1=="set")
  print "or " mask
else
  # must be res.
  # There's no xor in awk, but a simple `255-mask' is good enough here.
  print "and " 255-mask
if($3!="a") print "ld " $3 ",a"

# if A was modified, be careful to restore the flags but *not* A.
if($3=="a") print "ld h,a"
print "pop af"
if($3=="a")
  {
  print "ld a,h"
  print "pop hl"
  }

next
}


#** djnz
$1=="djnz" { print "dec b";print "jp nz," $2;next }


#** other relative jumps
$1=="jr" {

$1="jp"
if($3!="") $2=$2 ","	# needed for correct conversion of `jr nz,foo' etc.
print $0

next
}


#** ld a,(rr) for rr in bc/de

$1=="ld" && $2=="a" && ($3=="(bc)" || $3=="(de)") {

if($3=="(de)")
  {
  print "ex de,hl"
  print "ld a,(hl)"
  print "ex de,hl"
  }
else
  {
  print "push hl"
  print "ld h,b"
  print "ld l,c"
  print "ld a,(hl)"
  print "pop hl"
  }

next
}


#** ld (rr),a for rr in bc/de

$1=="ld" && $3=="a" && ($2=="(bc)" || $2=="(de)") {

if($2=="(de)")
  {
  print "ex de,hl"
  print "ld (hl),a"
  print "ex de,hl"
  }
else
  {
  print "push hl"
  print "ld h,b"
  print "ld l,c"
  print "ld (hl),a"
  print "pop hl"
  }

next
}



#** if it doesn't match any of the above, just echo it
{ print orig }


# ta-daaa! all done.


#* setup for emacs
#
# Local Variables:
# mode: outline-minor
# outline-regexp: "#\\*+"
# End:
