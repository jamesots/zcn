# ZCN `kernel' makefile

all: zcn

# main.z and eof.z must stay first and last, respectively

ZMAC=zmac

zcn: ../bin/zcn.bin ../bin/zcn200.bin

SRCFILES=main.z start.z \
	init.z inthndl.z bdos.z bios.z zcnfunc.z ccp.z \
	misc.z card.z cardboot.z internal.z \
	powrhndl.z keyread.z serial.z term.z \
	eof.z

# the other files in SRCFILES are included from nc100.z via main.z;
# asmver.z is included from internal.z.
# boot.z is assembled separately.
#
# Don't change the stuff which makes asmver.z without updating
# ../dosutils/mkasmver.c (and the exe) to match!

../bin/zcn.bin: boot.z 4x6font.dat nc100.z $(SRCFILES)
	$(RM) asmver.z
	echo "defb 'ZCN v'" >asmver.z
	grep '^zcnver.*equ' start.z | \
	  awk '{ printf("defb '\''%d.%d'\''\n",int($$3/100),$$3%100) }' \
	  >>asmver.z
	echo ';build date/time' >>asmver.z
	date "+defb ' (%Y-%m-%d %H:%M)'" >>asmver.z
	$(ZMAC) nc100.z
	$(ZMAC) boot.z
	cat boot.bin nc100.bin 4x6font.dat >../bin/zcn.bin
	ls -l ../bin/zcn.bin
	@printf 'Free bytes left below E600h for NC100: '
	@echo 16 i E600 `grep 'zcn_eof:$$' nc100.lst|cut -f 2` - f | dc


../bin/zcn200.bin: boot.z 4x6font.dat nc200.z $(SRCFILES)
	$(RM) asmver.z
	echo "defb 'ZCN200 v'" >asmver.z
	grep '^zcnver.*equ' start.z | \
	  awk '{ printf("defb '\''%d.%d'\''\n",int($$3/100),$$3%100) }' \
	  >>asmver.z
	echo ';build date/time' >>asmver.z
	date "+defb ' (%Y-%m-%d %H:%M)'" >>asmver.z
	$(ZMAC) nc200.z
	$(ZMAC) boot.z
	cat boot.bin nc200.bin 4x6font.dat >../bin/zcn200.bin
	ls -l ../bin/zcn200.bin
	@printf 'Free bytes left below E600h for NC200: '
	@echo 16 i E600 `grep 'zcn_eof:$$' nc200.lst|cut -f 2` - f | dc

clean:
	$(RM) *~ *.lst *.bin asmver.z

spotless: clean
	$(RM) ../bin/zcn.bin ../bin/zcn200.bin
