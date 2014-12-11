# makefile for CP/M zselx

all: ../bin/zselx.com

SRC=	zselx.z \
	../zcnlib/conio.z ../zcnlib/string.z \
	../zcnlib/maths.z ../zcnlib/qsort.z \
	data.z	# must be last

../bin/zselx.com: $(SRC)
	cat $(SRC) >out.z
	zmac out.z
	mv out.bin ../bin/zselx.com

clean:
	$(RM) *~ *.lst *.bin out.z
