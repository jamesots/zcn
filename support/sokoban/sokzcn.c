/* A fairly hacked-up version of sokoban, for CP/M.
 * ported 95/7/28 by Russell Marks
 *
 * It compiles ok (with the usual warnings :-)) under Hitech C.
 * All the source files were basically cat'ted together to make compliation
 * less awkward.
 *
 * You'll need to patch the 'clear' and 'move' routines with your
 * machine/terminal's clear screen and cursor move codes. (They're currently
 * set for ZCN.) For very simple terminals, you may even be able to patch
 * the binary directly and not bother recompiling.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define refresh()
#define printw printf


void clear()
{
putchar(1);
}


void move(int y,int x)
{
printf("%c%c%c",16,'F'+y,32+x);
}

short readscreen();


/**/
/* OBJECT: this typedef is used for internal and external representation */
/*         of objects                                                    */
/**/
typedef struct {
   char obj_intern;	/* internal representation of the object */
   char obj_display1;	/* first  display char for the object		 */
   char obj_display2;	/* second display char for the object		 */
   short invers;	/* if set to 1 the object will be shown invers */
} OBJECT;

/**/
/* You can now alter the definitions below.
/* Attention: Do not alter `obj_intern'. This would cause an error */
/*            when reading the screenfiles                         */
/**/
static OBJECT 
   player = 	 { '@', '*', '*', 0 },
   playerstore = { '+', '*', '*', 1 },
   store = 	 { '.', '.', '.', 0 },
   packet = 	 { '$', '[', ']', 0 },
   save = 	 { '*', '<', '>', 1 },
   ground = 	 { ' ', ' ', ' ', 0 },
   wall = 	 { '#', '#', '#', 1 };

/*************************************************************************
********************** DO NOT CHANGE BELOW THIS LINE *********************
*************************************************************************/
#define MAXROW		20
#define MAXCOL		40

typedef struct {
   short x, y;
} POS;

#define E_FOPENSCREEN	1
#define E_PLAYPOS1	2
#define E_ILLCHAR	3
#define E_PLAYPOS2	4
#define E_TOMUCHROWS	5
#define E_TOMUCHCOLS	6
#define E_ENDGAME	7
#define E_NOUSER	9
#define E_FOPENSAVE	10
#define E_WRITESAVE	11
#define E_STATSAVE	12
#define E_READSAVE	13
#define E_ALTERSAVE	14
#define E_SAVED		15
#define E_TOMUCHSE	16
#define E_FOPENSCORE	17
#define E_READSCORE	18
#define E_WRITESCORE	19
#define E_USAGE		20
#define E_ILLPASSWORD	21
#define E_LEVELTOOHIGH	22
#define E_NOSUPER	23
#define E_NOSAVEFILE	24

/* defining the types of move */
#define MOVE 		1
#define PUSH 		2
#define SAVE 		3
#define UNSAVE 		4
#define STOREMOVE 	5
#define STOREPUSH 	6

/* defines for control characters */
#define CNTL_L		'\014'
#define CNTL_K		'\013'
#define CNTL_H		'\010'
#define CNTL_J		'\012'
#define CNTL_R		'\022'
#define CNTL_U		'\025'

static POS   tpos1,		   /* testpos1: 1 pos. over/under/left/right */
             tpos2,		   /* testpos2: 2 pos.  "                    */
             lastppos,		   /* the last player position (for undo)    */
             last_p1, last_p2; /* last test positions (for undo)         */
static char lppc, ltp1c, ltp2c;    /* the char for the above pos. (for undo) */
static char action, lastaction;

/** For the temporary save **/
static char  tmp_map[MAXROW+1][MAXCOL+1];
static short tmp_pushes, tmp_moves, tmp_savepack;
static POS   tmp_ppos;

short scoring = 1;
short level, packets, savepack, moves, pushes, rows, cols;
short scorelevel, scoremoves, scorepushes;
char  map[MAXROW+1][MAXCOL+1];
POS   ppos;
char  username[]="Player", *prgname;


short play() {

   short c;
   short ret;
   short undolock = 1;		/* locked for undo */

   showscreen();
   tmpsave();
   ret = 0;
   while( ret == 0) {
      c=getch();
      switch(c) { case '8': c = 'k'; break;
                  case '2': c = 'j'; break;
                  case '4': c = 'h'; break;
		  case '6': c = 'l'; break;
		  case '5': c = 'u'; break;
                  case 'E'&0x3f: c='k'; break;
                  case 'S'&0x3f: c='h'; break;
                  case 'D'&0x3f: c='l'; break;
                  case 'X'&0x3f: c='j'; break;

                  default: break; };

      switch(c) {
	 case 'q':    /* quit the game 					*/
	              ret = E_ENDGAME; 
	              break;
	 case CNTL_R: /* refresh the screen 				*/
		      clear();
		      showscreen();
		      break;
	 case 'c':    /* temporary save					*/
         case 's':
		      tmpsave();
		      break;
	 case CNTL_U: /* reset to temporary save 			*/
         case 'r':
		      tmpreset();
		      undolock = 1;
		      showscreen();
		      break;
	 case 'U':    /* undo this level 				*/
		      moves = pushes = 0;
		      if( (ret = readscreen()) == 0) {
		         showscreen();
			 undolock = 1;
		      }
		      break;
	 case 'u':    /* undo last move 				*/
		      if( ! undolock) {
		         undomove();
		         undolock = 1;
		      }
		      break;

    	 case 'k':    /* up 						*/
	 case 'K':    /* run up 					*/
	 case CNTL_K: /* run up, stop before object 			*/
	 case 'j':    /* down 						*/
	 case 'J':    /* run down 					*/
	 case CNTL_J: /* run down, stop before object 			*/
	 case 'l':    /* right 						*/
	 case 'L':    /* run right 					*/
	 case CNTL_L: /* run right, stop before object 			*/
	 case 'h':    /* left 						*/
	 case 'H':    /* run left 					*/
	 case CNTL_H: /* run left, stop before object 			*/
		      do {
		         if( (action = testmove( c)) != 0) {
			    lastaction = action;
		            lastppos.x = ppos.x; lastppos.y = ppos.y;
		            lppc = map[ppos.x][ppos.y];
		            last_p1.x = tpos1.x; last_p1.y = tpos1.y; 
		            ltp1c = map[tpos1.x][tpos1.y];
		            last_p2.x = tpos2.x; last_p2.y = tpos2.y; 
		            ltp2c = map[tpos2.x][tpos2.y];
		            domove( lastaction); 
		            undolock = 0;
		         }
		      } while( (action != 0) && (! islower( c))
			      && (packets != savepack));
		      break;
	 default:     helpmessage(); break;
      }
      if( (ret == 0) && (packets == savepack)) {
	 scorelevel = level;
	 scoremoves = moves;
	 scorepushes = pushes;
	 break;
      }
   }
   return( ret);
}

testmove( action)
register short action;
{
   register short ret;
   register char  tc;
   register short stop_at_object;

   if( (stop_at_object = iscntrl( action))) action = action + 'A' - 1;
   action = (isupper( action)) ? tolower( action) : action;
   if( (action == 'k') || (action == 'j')) {
      tpos1.x = (action == 'k') ? ppos.x-1 : ppos.x+1;
      tpos2.x = (action == 'k') ? ppos.x-2 : ppos.x+2;
      tpos1.y = tpos2.y = ppos.y;
   }
   else {
      tpos1.y = (action == 'h') ? ppos.y-1 : ppos.y+1;
      tpos2.y = (action == 'h') ? ppos.y-2 : ppos.y+2;
      tpos1.x = tpos2.x = ppos.x;
   }
   tc = map[tpos1.x][tpos1.y];
   if( (tc == packet.obj_intern) || (tc == save.obj_intern)) {
      if( ! stop_at_object) {
         if( map[tpos2.x][tpos2.y] == ground.obj_intern)
            ret = (tc == save.obj_intern) ? UNSAVE : PUSH;
         else if( map[tpos2.x][tpos2.y] == store.obj_intern)
            ret = (tc == save.obj_intern) ? STOREPUSH : SAVE;
         else ret = 0;
      }
      else ret = 0;
   }
   else if( tc == ground.obj_intern)
      ret = MOVE;
   else if( tc == store.obj_intern)
      ret = STOREMOVE;
   else ret = 0;
   return( ret);
}

domove( moveaction) 
register short moveaction;
{
   map[ppos.x][ppos.y] = (map[ppos.x][ppos.y] == player.obj_intern) 
			       ? ground.obj_intern 
			       : store.obj_intern;
   switch( moveaction) {
      case MOVE:      map[tpos1.x][tpos1.y] = player.obj_intern; 	break;
      case STOREMOVE: map[tpos1.x][tpos1.y] = playerstore.obj_intern; 	break;
      case PUSH:      map[tpos2.x][tpos2.y] = map[tpos1.x][tpos1.y];
		      map[tpos1.x][tpos1.y] = player.obj_intern;	
		      pushes++;						break;
      case UNSAVE:    map[tpos2.x][tpos2.y] = packet.obj_intern;
		      map[tpos1.x][tpos1.y] = playerstore.obj_intern;		
		      pushes++; savepack--;			 	break;
      case SAVE:      map[tpos2.x][tpos2.y] = save.obj_intern;
		      map[tpos1.x][tpos1.y] = player.obj_intern;			
		      savepack++; pushes++;				break;
      case STOREPUSH: map[tpos2.x][tpos2.y] = save.obj_intern;
		      map[tpos1.x][tpos1.y] = playerstore.obj_intern;		
		      pushes++;						break;
   }
   moves++;
   dispmoves(); disppushes(); dispsave();
   mapchar( map[ppos.x][ppos.y], ppos.x, ppos.y);
   mapchar( map[tpos1.x][tpos1.y], tpos1.x, tpos1.y);
   mapchar( map[tpos2.x][tpos2.y], tpos2.x, tpos2.y);
   move( MAXROW+1, 0);
   refresh();
   ppos.x = tpos1.x; ppos.y = tpos1.y;
}

undomove() {

   map[lastppos.x][lastppos.y] = lppc;
   map[last_p1.x][last_p1.y] = ltp1c;
   map[last_p2.x][last_p2.y] = ltp2c;
   ppos.x = lastppos.x; ppos.y = lastppos.y;
   switch( lastaction) {
      case MOVE:      moves--;				break;
      case STOREMOVE: moves--;				break;
      case PUSH:      moves--; pushes--;		break;
      case UNSAVE:    moves--; pushes--; savepack++;	break;
      case SAVE:      moves--; pushes--; savepack--;	break;
      case STOREPUSH: moves--; pushes--;		break;
   }
   dispmoves(); disppushes(); dispsave();
   mapchar( map[ppos.x][ppos.y], ppos.x, ppos.y);
   mapchar( map[last_p1.x][last_p1.y], last_p1.x, last_p1.y);
   mapchar( map[last_p2.x][last_p2.y], last_p2.x, last_p2.y);
   move( MAXROW+1, 0);
   refresh();
}

tmpsave() {

   register short i, j;

   for( i = 0; i < rows; i++) for( j = 0; j < cols; j++)
      tmp_map[i][j] = map[i][j];
   tmp_pushes = pushes;
   tmp_moves = moves;
   tmp_savepack = savepack;
   tmp_ppos.x = ppos.x; tmp_ppos.y = ppos.y;
}

tmpreset() {

   register short i, j;

   for( i = 0; i < rows; i++) for( j = 0; j < cols; j++)
      map[i][j] = tmp_map[i][j];
   pushes = tmp_pushes;
   moves = tmp_moves;
   savepack = tmp_savepack;
   ppos.x = tmp_ppos.x; ppos.y = tmp_ppos.y;
}

short readscreen() {

   FILE *screen;
   short j, c, f, ret = 0;

   if( (screen = fopen( "soklevls.dat", "r")) == NULL)
      ret = E_FOPENSCREEN;
   else {
      if(level>1) {
         for(f=1;f<level;f++) {
            while((c=getc(screen))!=12 && c!=EOF) ;
            getc(screen);	/* get the \n after */
            }
         }

      packets = savepack = rows = j = cols  = 0;
      ppos.x = -1; ppos.y = -1;
      while( (ret == 0) && ((c = getc( screen)) != 12) && c!=EOF) {
         if( c == '\n') {
	    map[rows++][j] = '\0';
	    if( rows > MAXROW) 
	       ret = E_TOMUCHROWS;
	    else {
	       if( j > cols) cols = j;
	       j = 0;
	    }
	 }
	 else if( (c == player.obj_intern) || (c == playerstore.obj_intern)) {
	    if( ppos.x != -1) 
	       ret = E_PLAYPOS1;
	    else { 
	       ppos.x = rows; ppos.y = j;
	       map[rows][j++] = c;
	       if( j > MAXCOL) ret = E_TOMUCHCOLS;
	    }
	 }
	 else if( (c == save.obj_intern) || (c == packet.obj_intern) ||
		  (c == wall.obj_intern) || (c == store.obj_intern) ||
		  (c == ground.obj_intern)) {
	    if( c == save.obj_intern)   { savepack++; packets++; }
	    if( c == packet.obj_intern) packets++;
	    map[rows][j++] = c;
	    if( j > MAXCOL) ret = E_TOMUCHCOLS;
	 }
	 else ret = E_ILLCHAR;
      }
      fclose( screen);
      if( (ret == 0) && (ppos.x == -1)) ret = E_PLAYPOS2;
   }
   return( ret);
}


static int        savedbn;
static char        *sfname;
static FILE        *savefile;

showscreen() {

   register short i, j;

   clear();
   for( i = 0; i < rows; i++)
      for( j = 0; map[i][j] != '\0'; j++)
         mapchar( map[i][j], i, j);
   move( MAXROW, 0);
   printw( "Level:      Packets:      Saved:      Moves:       Pushes:");
   displevel();
   disppackets();
   dispsave();
   dispmoves();
   disppushes();
   move( MAXROW+2,0);
   refresh();
}

mapchar( c, i, j) 
register char c; 
register short i, j;
{
   OBJECT *obj, *get_obj_adr();
   register short offset_row = 0;	/*(MAXROW - rows) / 2;*/
   register short offset_col = MAXCOL - cols;

   obj = get_obj_adr( c);

/*   if( obj->invers) standout();*/
   move( i + offset_row, 2*j + offset_col); 
   printw( "%c%c", obj ->obj_display1, obj ->obj_display2);
/*   if( obj->invers) standend();*/
}

OBJECT *get_obj_adr( c)
register char c;
{
   register OBJECT *ret;

   if(      c == player.obj_intern)		ret = &player;
   else if( c == playerstore.obj_intern)	ret = &playerstore;
   else if( c == store.obj_intern)		ret = &store;
   else if( c == save.obj_intern)		ret = &save;
   else if( c == packet.obj_intern)		ret = &packet;
   else if( c == wall.obj_intern)		ret = &wall;
   else if( c == ground.obj_intern)		ret = &ground;
   else                                         ret = &ground;

   return( ret);
}


displevel() { 
   move( MAXROW, 7); printw( "%3d", level); 
}
   
disppackets() { 
   move( MAXROW, 21); printw( "%3d", packets); 
}
   
dispsave() { 
   move( MAXROW, 33); printw( "%3d", savepack); 
}
   
dispmoves() { 
   move( MAXROW, 45); printw( "%5d", moves); 
}
      
disppushes() { 
   move( MAXROW, 59); printw( "%5d", pushes); 
}

helpmessage() {
#if 0
   move( MAXROW+2, 0); 
   printw( "Press ? for help.");
   refresh();
   sleep( 1);
   move( MAXROW+2, 0); deleteln();
   refresh(); 
#endif
}

showhelp() {
}


static short optshowscore = 0, 
	     optmakescore = 0, 
             optrestore = 0, 
	     optlevel = 1; 
static short superuser = 0;

static short userlevel;

main( argc, argv) 
short argc; 
char *argv[];
{
   short ret, ret2;

   scorelevel = 0;
   moves = pushes = packets = savepack = 0;
   if( (prgname = strrchr( argv[0], '/')) == NULL)
      prgname = argv[0];
   else prgname++;
   
   {
      if( (ret = checkcmdline( argc, argv)) == 0) {
         level=optlevel;
      }
   }
   ret = gameloop();
   errmess( ret);
}

checkcmdline( argc, argv) 
short argc; 
char *argv[];
{
   short ret = 0;

    if(argc==2) {
	 if( (optlevel = atoi(argv[1])) == 0)
	    ret = E_USAGE;
      }
   return( ret);
}

gameloop() {

   short ret = 0;

/*   initscr(); cbreak(); noecho();*/
   if( ! optrestore) ret = readscreen();
   while( ret == 0) {
      if( (ret = play()) == 0) {
         level++;
         moves = pushes = packets = savepack = 0;
         ret = readscreen();
      }
   }
   clear(); refresh(); 
/*   nocbreak(); echo(); endwin();*/
   return( ret);
}

char *message[] = {
   "illegal error number",
   "cannot open screen file",
   "more than one player position in screen file",
   "illegal char in screen file",
   "no player position in screenfile",
   "too much rows in screen file",
   "too much columns in screenfile",
   "quit the game",
   NULL,			/* errmessage deleted */
   "cannot get your username",
   "cannot open savefile",
   "error writing to savefile",
   "cannot stat savefile",
   "error reading savefile",
   "cannot restore, your savefile has been altered",
   "game saved",
   "too much users in score table",
   "cannot open score file",
   "error reading scorefile",
   "error writing scorefile",
   "illegal command line syntax",
   "illegal password",
   "level number too big in command line",
   "only superuser is allowed to make a new score table",
   "cannot find file to restore"
};

errmess( ret) 
register short ret;
{
   if( ret != E_ENDGAME) {
      fprintf( stderr, "%s: ", prgname);
      switch( ret) {
         case E_FOPENSCREEN: case E_PLAYPOS1:   case E_ILLCHAR: 
	 case E_PLAYPOS2:    case E_TOMUCHROWS: case E_TOMUCHCOLS: 
	 case E_ENDGAME:     case E_NOUSER:      
	 case E_FOPENSAVE:   case E_WRITESAVE:  case E_STATSAVE:    
	 case E_READSAVE:    case E_ALTERSAVE:  case E_SAVED:       
	 case E_TOMUCHSE:    case E_FOPENSCORE: case E_READSCORE: 
	 case E_WRITESCORE:  case E_USAGE:	case E_ILLPASSWORD:
	 case E_LEVELTOOHIGH: case E_NOSUPER:	case E_NOSAVEFILE:
			     fprintf( stderr, "%s\n", message[ret]);
                             break;
         default:            fprintf( stderr, "%s\n", message[0]);
                             break;
      }
      if( ret == E_USAGE) usage();
   }
}

usage() {
   fprintf( stderr, "Usage: %s [start_level]", prgname);
}

