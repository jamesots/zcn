# makefile for zcnlib

# Unlike the rest of ZCN, the (demo) programs generated here
# are *not* put in ../bin. This is because I consider zcnlib
# somewhat `separate' - and also the demos are pretty lame. :-)

all: floodemo.com mousedem.com tridemo.com zmfilt.com \
	zcnlibh.z zcnlibh.asm

ZCNLIB_SRC=args.z conio.z ctype.z getopt.z graph.z graph2.z \
	int32.z maths.z mouse.z qsort.z rand.z sqrt.z stdio.z \
	string.z

FLD_SRC=floodemo.z rand.z sqrt.z conio.z maths.z graph.z graph2.z
TRI_SRC=tridemo.z rand.z sqrt.z conio.z maths.z graph.z graph2.z
MOUSE_SRC=mousedem.z graph.z mouse.z
ZMFILT_SRC=zmfilt.z stdio.z string.z conio.z

floodemo.com: $(FLD_SRC)
	cat $(FLD_SRC) >out.z
	zmac out.z
	mv out.bin floodemo.com

mousedem.com: $(MOUSE_SRC)
	cat $(MOUSE_SRC) >out.z
	zmac out.z
	mv out.bin mousedem.com

tridemo.com: $(TRI_SRC)
	cat $(TRI_SRC) >out.z
	zmac out.z
	mv out.bin tridemo.com

zmfilt.com: $(ZMFILT_SRC)
	cat $(ZMFILT_SRC) >out.z
	zmac out.z
	mv out.bin zmfilt.com

zcnlibh.z: $(ZCNLIB_SRC)
	grep '^;ZZ' $(ZCNLIB_SRC) |cut -f 2- -d: |\
	  awk '{ print $$1 " " $$2 }'|sort|uniq >zcnlibh.z

zcnlibh.asm: zcnlibh.z
	awk '{ sub(/^;ZZ/,""); print $$0 "\r" }; END { printf "\032" }' \
		<zcnlibh.z >zcnlibh.asm

clean:
	$(RM) *~ *.lst *.bin out.z
