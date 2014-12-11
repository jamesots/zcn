# makefile for dclock/zcnclock

include ../config.mk	# for HAVE_XEARTH_AND_NETPBM


all: ../bin/dclock.com ../bin/zcnclock.com

COMMON=common.z clock.z digits.z

../bin/dclock.com: $(COMMON) dclock.z
	zmac dclock.z
	mv dclock.bin ../bin/dclock.com

../bin/zcnclock.com: \
		$(COMMON) zcnclock.z readmrf.z circ60.z citydat.z \
		graph_zc.z ../zcnlib/maths.z cities.mrf
	zmac zcnclock.z
	cat cities.mrf >>zcnclock.bin
	mv zcnclock.bin ../bin/zcnclock.com

circ60.z: gen60.c
	$(CC) -o gen60 gen60.c -lm
	./gen60 >circ60.z
	$(RM) gen60

cities.mrf: mkworld.sh pbmtomrf.c
ifeq ($(HAVE_XEARTH_AND_NETPBM),yes)
	$(CC) -o pbmtomrf pbmtomrf.c
	sh ./mkworld.sh
	$(RM) pbmtomrf
else
	@echo '=============================================================='
	@echo 'WARNING: cities.mrf cannot be built without xearth and netpbm!'
	@echo '=============================================================='
endif

# cities.mrf isn't removed as you need xearth/netpbm to make it.
clean:
	$(RM) *~ *.o gen60 pbmtomrf *.lst *.bin out.z
