�-Code Hex Dec Description			Code Hex Dec Description		      ZCN
^A   01h  1  clear screen			^R   12h 18  insert line		terminal driver
^B   02h  2  bold off				^T   14h 20  delete line		 control codes
^C   03h  3  cursor on				^U   15h 21  scroll up
^D   04h  4  cursor off				^W   17h 23  scroll down
^E   05h  5  bold on				^X   18h 24  true video
^N   0Eh 14  italics off			^Y   19h 25  reverse video
^O   0Fh 15  italics on				^^   1Eh 30  home cursor
^P   10h 16  move cursor (then 20h+y and 20h+x) ^_   1Fh 31  clear to end of line$	� 