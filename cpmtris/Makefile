# makefile for cpmtris

all: ../bin/cpmtris.com

SRC=cpmtris.z rand.z conio.z

../bin/cpmtris.com: $(SRC)
	cat $(SRC) >out.z
	zmac out.z
	mv out.bin ../bin/cpmtris.com

clean:
	$(RM) *~ *.lst *.bin out.z
