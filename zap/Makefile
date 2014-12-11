# makefile for zap

include ../config.mk	# for HAVE_CPM_EMU


all: ../bin/zap.com ../bin/makezapd.com zapdesc

SRC=zap.z msort.z \
	../zcnlib/conio.z ../zcnlib/string.z ../zcnlib/maths.z

MZSRC=makezapd.z mz_args.z \
	../zcnlib/ctype.z ../zcnlib/stdio.z

../bin/zap.com: $(SRC)
	zmac zap.z
	mv zap.bin ../bin/zap.com

../bin/makezapd.com: $(MZSRC)
	zmac makezapd.z
	mv makezapd.bin ../bin/makezapd.com

zapdesc: ../bin/zapdesc.bin

../bin/zapdesc.bin: zapfiles/*.zap ../bin/makezapd.com
ifeq ($(HAVE_CPM_EMU),yes)
	cd zapfiles;ln -sf ../../bin/makezapd.com .;cpm makezapd '*.zap';echo
	$(RM) zapfiles/makezapd.com
	mv zapfiles/zapdesc.bin ../bin
else
	@echo '==================================================='
	@echo 'WARNING: zapdesc.bin cannot be built without "cpm"!'
	@echo '==================================================='
endif

clean:
	$(RM) *~ *.lst *.bin
