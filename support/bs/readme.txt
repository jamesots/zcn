This is a version of Battleships played on a 10x10 grid, with you
playing against the computer. There's a reasonable amount of onscreen
help, so you should be able to figure out how to work it pretty
quickly.

It's a port of a Unix curses version hacked by a few different people.
A formatted version of the man page (documentation) is given below,
and the 'readme' from the last person to hack it is in 'esr.txt'.

-Rus.


BATTLESHIPS(6)                                  BATTLESHIPS(6)

NAME
       bs - battleships game

SYNOPSIS
       bs [ -b | -s ] [ -c ]

DESCRIPTION
       This  program  allows you to play the familiar Battleships
       game against the computer on a 10x10 board. The  interface
       is  visual  and  largely self-explanatory; you place your
       ships and pick your shots by moving the cursor around  the
       `sea'  with  the rogue/hack motion keys hjklyubn.  If your
       UNIX has a modern (non-BSD) curses, your arrow  keys  will
       also work.

       Note  that  when selecting a ship to place, you must type
       the capital letter (these are, after all, capital  ships).
       During  ship  placement, the  `r'  command may be used to
       ignore the current position and randomly place  your  cur-
       rently  selected ship.   The  `R'  command  will place all
       remaining ships randomly. The ^L command (form feed, ASCII
       12) will force a screen redraw).

       The command-line arguments control game modes.

            -b selects a `blitz' variant
            -s selects a `salvo' variant
            -c permits ships to be placed adjacently

       The  `blitz' variant allows a side to shoot for as long as
       it continues to score hits.

       The `salvo' game allows a player one  shot  per  turn  for
       each  of his/her ships still afloat.  This puts a premium
       scoring hits early and knocking out some ships   and  also
       makes  much harder the situation where you face a superior
       force with only your PT-boat.

       Normally, ships must be separated by at least  one  square
       of  open water.  The  -c  option  disables this check and
       allows them to close-pack.

       The algorithm the computer uses once it has found  a  ship
       to sink is provably optimal.  The dispersion criterion for
       the random-fire algorithm may not be.

AUTHORS
       Originally written by one Bruce Holloway in  1986.  Salvo
       mode  added  by Chuck A. DeGaul (cbosgd!cad). Visual user
       interface, `closepack' option,  code  rewrite  and  manual
       page  by Eric  S.  Raymond <esr@snark.thyrsus.com> August
       1989.  Keypad support and ANSI/POSIX conformance, November
       '93.   See http://www.ccil.org/~esr/home.html for updates,
       also other software and resources by ESR.

                           Nov 15 1993                          1
