The following changes were made to `z8e.com' relative to the original,
so that it would run reasonably on a ten-line screen:

0e41=6		length of disasm made for `j' cmd
0db0=6		ditto
0e33=8		row no. above prompt after `j'
0de7=9		length of disasm plus 3 (needed for calc.)
1004=80		set mem. dump size for `d addr' to 128 bytes
2db8=80		}__ set mem. dump size for `d' to 128 bytes
2db9=0		}
1f0b=8		length of disasm made for `z' cmd

The relevant changes documented in `z8e.man' were also made, among
them changing the usual `rst 38h' breakpoint to `rst 28h', meaning
there's no need to run it in IM2.

-Rus 980514.
