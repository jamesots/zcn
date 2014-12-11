# ZCN utils makefile

ZCNBROT_SRC=zcnbrot.z \
	../zcnlib/maths.z ../zcnlib/int32.z \
	../zcnlib/conio.z ../zcnlib/graph.z

BBCBAS_SRC=bbcbas.z bbc_io.z
BBCMIN_SRC=bbcmin.z bbc_io.z

SPELL_SRC=spell.z \
	../zcnlib/stdio.z ../zcnlib/conio.z \
	../zcnlib/string.z ../zcnlib/ctype.z \
	../zcnlib/maths.z

EXPR_SRC=expr.z \
	../zcnlib/int32.z \
	../zcnlib/string.z ../zcnlib/ctype.z


all: utils games

# This is a pretty boneheaded way to do this. Better approaches
# gratefully accepted...
#
utils: \
	../bin/bbcbas.com \
	../bin/bbcmin.com \
	../bin/bigrun.com \
	../bin/bigv.com \
	../bin/bmp.com \
	../bin/cal.com \
	../bin/calc.com \
	../bin/codes.com \
	../bin/defrag.com \
	../bin/extra/dial.com \
	../bin/dmp2txt.com \
	../bin/du.com \
	../bin/expr.com \
	../bin/head.com \
	../bin/keyb.com \
	../bin/ls.com \
	../bin/lfconv.com \
	../bin/optdir.com \
	../bin/extra/play1b.com \
	../bin/extra/play2b.com \
	../bin/extra/play4b.com \
	../bin/rrinit.bin \
	../bin/rrxfer.com \
	../bin/ruler.com \
	../bin/runrom.com \
	../bin/semi.com \
	../bin/slide.com \
	../bin/spell.com \
	../bin/spellwd.com \
	../bin/extra/stat.com \
	../bin/submit.com \
	../bin/time.com \
	../bin/timeset.com \
	../bin/extra/zcnbrot.com \
	../bin/zdbe.com \
	../bin/zrx.com

games: \
	../bin/invaders.com

../bin/extra/zcnbrot.com: $(ZCNBROT_SRC)
	cat $(ZCNBROT_SRC) >out.z
	zmac out.z
	mv out.bin ../bin/extra/zcnbrot.com

../bin/rrinit.bin: rrinit.z
	zmac rrinit.z
	mv rrinit.bin ../bin

# bbc_cat.z is `include'd by bbcbas.z
../bin/bbcbas.com: $(BBCBAS_SRC) bbc_cat.z
	cat $(BBCBAS_SRC) >out.z
	zmac out.z
	@printf 'Free bytes in block1: '
	@echo 3072 16i `grep block1_siz.equ out.lst|cut -f 2` -f|dc|tee tmp.lst
	@if [ `cat tmp.lst` -lt 0 ]; then echo '*** failed'; exit 1; fi
	@printf 'Free bytes in block2: '
	@echo 1024 16i `grep block2_siz.equ out.lst|cut -f 2` -f|dc|tee tmp.lst
	@if [ `cat tmp.lst` -lt 0 ]; then echo '*** failed'; exit 1; fi
	mv out.bin ../bin/bbcbas.com

../bin/bbcmin.com: $(BBCMIN_SRC)
	cat $(BBCMIN_SRC) >out.z
	zmac out.z
	@printf 'Free bytes in block1: '
	@echo 3072 16i `grep block1_siz.equ out.lst|cut -f 2` -f|dc|tee tmp.lst
	@if [ `cat tmp.lst` -lt 0 ]; then echo '*** failed'; exit 1; fi
	mv out.bin ../bin/bbcmin.com

../bin/bigrun.com: brsetup.z brpatch.z
	zmac brsetup.z
	zmac brpatch.z
	cat brsetup.bin brpatch.bin >../bin/bigrun.com

../bin/lfconv.com: lfconv.z ../zcnlib/stdio.z
	sed -e '/^dmabuf:/d' -e 's/^fseek:/if 0/' <../zcnlib/stdio.z >tmpstd.z
	echo endif >>tmpstd.z
	cat lfconv.z tmpstd.z >out.z
	zmac out.z
	$(RM) tmpstd.z
	mv out.bin ../bin/lfconv.com

../bin/spell.com: $(SPELL_SRC)
	cat $(SPELL_SRC) >out.z
	echo eof: >>out.z
	zmac out.z
	mv out.bin ../bin/spell.com

../bin/cal.com: cal.z cal_end.z ../zcnlib/args.z
	sed -e '/^argline:/d' -e '/^argv:/d' <../zcnlib/args.z >tmpargs.z
	cat cal.z tmpargs.z cal_end.z >out.z
	zmac out.z
	$(RM) tmpargs.z
	mv out.bin ../bin/cal.com

../bin/expr.com: $(EXPR_SRC)
	cat $(EXPR_SRC) >out.z
	zmac out.z
	mv out.bin ../bin/expr.com

../bin/slide.com: slide.z slidebmp.mrf
	zmac slide.z
	cat slidebmp.mrf >>slide.bin
	mv slide.bin ../bin/slide.com

../bin/bigv.com: bigv.z bigvfont.z bigvdat.z \
		../zcnlib/stdio.z ../zcnlib/conio.z
	sed '/^dmabuf:/d' <../zcnlib/stdio.z >tmpstd.z
	cat bigv.z tmpstd.z >out.z
	zmac out.z
	$(RM) tmpstd.z
	mv out.bin ../bin/bigv.com


slidebmp.mrf: slidebmp.c
	$(CC) -o slidebmp slidebmp.c -lm
	./slidebmp
	$(RM) slidebmp

bigvdat.z: bdf2z.c
	$(CC) -o bdf2z bdf2z.c
	./bdf2z 1 <helvR10.bdf >bigvdat.z
	$(RM) bdf2z

clean:
	$(RM) *~ *.lst *.bin out.z tmpargs.z slidebmp bdf2z


# rules
# the format may make them specific to GNU make
../bin/%.com: %.z
	zmac $(subst ../bin/,,$<)
	mv `basename $< .z`.bin $@
../bin/extra/%.com: %.z
	zmac $(subst ../bin/,,$<)
	mv `basename $< .z`.bin $@
