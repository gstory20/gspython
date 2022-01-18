/*
  lscrn.c
  Glenn Story - 1/14/84

  This is a package of routines to interface to the screen-handling
  routines in BIOS ROM.  The routines are:

  Linux Version - This version uses hard-coded ANSI strings

    gotoxy (x,y) - direct cursor addressing
    clrscreen () - clear screen
    normal () - sets normal output
    reverse () - sets reverse output
    blink () - sets blinking output
    rev_blink () - sets reverse blinking output
    erase_line (line) - erase the named line
    screen_size () - number of rows in screen

*/

#include "gs.h"
#include "lscrn.h"

#include <sys/ioctl.h>
/*#include "termios.h"*/

#define VERSION

static int already_saved = FALSE;
int lscrn_saved_x = 1;
int lscrn_saved_y = 1;

static void my_exit(void)
{
	restore_screen();
	normal();
}

static void set_exit(void)
{
	static int first_time = TRUE;

	if (first_time)
	{
		atexit(my_exit);
		first_time = FALSE;
	};
}

void restore_screen(void)
{
	set_exit();

	if (already_saved)
	{
		clrscrn();
		printf ("\033[?47l");
		printf ("\0338");
		printf ("\n");
		already_saved = FALSE;
	}
}

void save_screen(void)
{
	set_exit();

	if (already_saved)
		return;

	printf ("\0337");
	printf ("\033[?47h");

	already_saved = TRUE;
}

void    my_putchar (int c)
{
        putchar (c);
}

void make_absolute (int *x, int *y)
{
	/*  Negative or zero values for x and y mean to use values
	 *  relative to the previous values of x and y.  E.g. y == -3
	 *  means use a line 3 lines down from the prevous line.
	 *  y == 0 means same line as previous.
	 *
	 *  x and y are reset by clrscreen (in this file).
	 *  x is also reset by functions in hscrn.c to be equal to the
	 *  end of the currnetly displayed field.
	 */

	/*  If changing lines reset x to 1 */
	if (*y != 0)
		lscrn_saved_x = 1;

	/*  Handle relative x */
	if (*x < 1)
		*x = lscrn_saved_x - *x;

	/*  Handle relative y */
	if (*y < 1)
		*y = lscrn_saved_y - *y;

	lscrn_saved_x = *x;
	lscrn_saved_y = *y;
}	

void gotoxy (x, y) /* direct cursor addressing */

int x,y;

{
	make_absolute (&x, &y);
	printf ("\033[%d;%dH", y, x);
	fflush (stdout);
}

void clrscrn ()

{
    static int reentered = FALSE;
    int i;
    int size;

    lscrn_saved_x = lscrn_saved_y = 1;
    
    if (reentered)
        return;

    reentered = TRUE;

    printf ("\033[H\033[2J");
    fflush (stdout);

    reentered = FALSE;
}

void set_color (enum lscrn_colors foreground, enum lscrn_colors background)
{
	set_exit();

	switch (foreground)
	{
		case LSCRN_BLACK:   printf ("\033[30m");  break;
		case LSCRN_RED:     printf ("\033[31m");  break;
		case LSCRN_GREEN:   printf ("\033[32m");  break;
		case LSCRN_YELLOW:  printf ("\033[33m");  break;
		case LSCRN_BLUE:    printf ("\033[34m");  break;
		case LSCRN_PURPLE:  printf ("\033[35m");  break;
		case LSCRN_AQUA:    printf ("\033[36m");  break;
		case LSCRN_GRAY:    printf ("\033[37m");  break;
        case LSCRN_NOCHANGE:                      break;  /*  make compile happy */
	}

	switch (background)
	{
		case LSCRN_BLACK:   printf ("\033[40m");  break;
		case LSCRN_RED:     printf ("\033[41m");  break;
		case LSCRN_GREEN:   printf ("\033[42m");  break;
		case LSCRN_YELLOW:  printf ("\033[43m");  break;
		case LSCRN_BLUE:    printf ("\033[44m");  break;
		case LSCRN_PURPLE:  printf ("\033[45m");  break;
		case LSCRN_AQUA:    printf ("\033[46m");  break;
		case LSCRN_GRAY:    printf ("\033[47m");  break;
        case LSCRN_NOCHANGE:                      break;  /*  make compile happy */
	}

	fflush (stdout);
}

void normal (void)

{
  printf ("\033[m");
  fflush (stdout);
}

void reverse ()

{
	set_exit();

	printf ("\033[7m");
	fflush (stdout);
}

void blink ()

{
	set_color (LSCRN_RED, LSCRN_NOCHANGE);
}

void rev_blink ()

{
	set_color (LSCRN_BLACK, LSCRN_RED);
}


void erase_line (int line_number)
{
        gotoxy (1, line_number);

	printf ("\033[K");
	fflush (stdout);
}

int screen_size_no_clear (void)
{
	int height;
	struct winsize window;


	if ((ioctl (2, TIOCGWINSZ, &window) == 0) && (window.ws_row > 0))
		height = window.ws_row;
	else
		height = 24;

	return height;
}

int screen_size (void)
{
	int height;
	
	height = screen_size_no_clear ();
	
	clrscrn ();

	return height;
}

int line_size (void)

{
	int width;
	struct winsize window;


	if ((ioctl (2, TIOCGWINSZ, &window) == 0) && (window.ws_row > 0))
		width = window.ws_col;
	else
		width = 80;

	return width;
}

void lputs (const char *s)
{
    printf ("%s", s);
}

void lputc (int c)
{
    putchar (c);
}

VERSION_PROC (lscrn_version)
