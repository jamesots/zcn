# top-level ZCN makefile


# see `config.mk' for configurable stuff.
include config.mk


# for `make zip' - this is the version number without the dot (to keep
# the zip's filename 8.3-friendly), e.g. `1.2' becomes `12'.
VERS=13


# `all' dirs get mentioned here even if they have no source.
# (Notable exception is zmac, which has its own Makefile which doesn't
# do what we'd want.)
#
DIRS=bin cpmtris doc dosutils man src support utils \
	zap zcnclock zcnlib zcnpaint zselx

ZIPFILE=../zcn$(VERS).zip


all: zcn

# for any BSD types :-)
World: world
world: zcn

zcn:
	for i in $(DIRS); do $(MAKE) -C $$i; done

install:
	@echo 'See doc/zcn.txt for how to install ZCN.'


# zmac has to be dealt with specially, since we don't actually want to
# make that automatically. That means it's omitted from DIRS, and
# has to be mentioned here explicitly.
clean:
	for i in $(DIRS); do $(MAKE) -C $$i clean; done
	$(MAKE) -C zmac clean
	$(RM) *~


# I used to have the rather bad habit of compressing ZCN stuff when I
# ran short of disk space :-), so this was done before making a
# distribution to fix that. It's not needed any more, but I've left it
# in just in case.
#
# (This doesn't require you to have gzip or bzip2, as errors are
# ignored.)
#
ungz:
	@echo unzipping files - may give errors, don\'t panic...
	-gzip -d `find . -name '*.gz'`
	-bzip2 -d `find . -name '*.bz2'`


dist: zip

zip: $(ZIPFILE)

# The exclusion of any `sav' dir is because I sometimes make a
# copy of stuff in such a dir before changing things, in case
# I screw it up. Version control? What's that? :-)
#
# Note also that the make clean deletes temp files, `*~', etc. rather
# than the generated binaries.
#
# The `-follow' is needed so zmac is included, as I don't actually
# keep zmac in the `zcn' dir on my machine. (Well, I do nowadays,
# but it can't hurt. :-))
#
$(ZIPFILE): ungz zcn clean
	$(RM) $(ZIPFILE)
	zip -r $(ZIPFILE) \
	  `find . -type f -follow -print|\
	   sed 's,./,,'|sort -f|grep -vE '/sav(/|$)'`
	ls -l $(ZIPFILE)
	sync
