# ZCN man makefile

include ../config.mk	# for HAVE_CPM_EMU


SRC=man.z view.z ../zcnlib/maths.z


all: man

man: ../bin/man.com ../bin/manpages.pma

../bin/man.com: $(SRC) ../zcnlib/stdio.z man_end.z ../support/pmarc/pmexe2.com
	sed '/^dmabuf:/d' <../zcnlib/stdio.z >tmpstdio.z
	cat $(SRC) tmpstdio.z man_end.z >out.z
	zmac out.z
	$(RM) tmpstdio.z
	cat ../support/pmarc/pmexe2.com >>out.bin
	mv out.bin ../bin/man.com

# the `mkdir tmp' junk below sorts the files in the dir, so that pmarc
# will archive them in alphabetical order.
../bin/manpages.pma: pages/*.man
ifeq ($(HAVE_CPM_EMU),yes)
	$(RM) manpages.pma
	$(RM) pages/*.man~
	cd pages;mkdir tmp;mv *.man tmp;mv tmp/* .;rmdir tmp
	cd pages;ln -sf ../../support/pmarc/pmarc.com .
	cd pages;cpm pmarc manpages.pma '*.man/t'
	mv pages/manpages.pma ../bin
	$(RM) pages/pmarc.com
else
	@echo '===================================================='
	@echo 'WARNING: manpages.pma cannot be built without "cpm"!'
	@echo '===================================================='
endif

clean:
	$(RM) *~ *.lst *.bin out.z tmpstdio.z pages/*~
