/*  HSCRN.C

    Glenn Story - 1/22/84

    This program contains a set of subroutines for doing high-level
    I/O to the screen.  The concept of these routines is borrowed
    heavily from dBASE II.  The entrypoints are as follows:

        clear - clears input variables
        erase - resets the system
        geta - gets a string
        getn - gets a number
        getl - get a logical value
	getl_no_default - same as getl except initial value is blank
        saya - write output to the screen
        sayn - write a number to the screen
        sayl - write a logical value
        rd - read data previously specified
        set_field - set cursor in specified field

        13:49:28  8/10/1985 - Modified for C86 on IBM PC

        20:33:51  12/31/1985 - Make it work with new GETACHAR

        11/15/87 - Change Mac ENTER key to = ^w

        7/24/88 - Add return value from RD (FALSE if ESC pressed)

        1/1/90 - Add INS, DEL, HOME, END keys

        7/17/90 - Correct cursor position for END key

        2/12/92 - Change highlighting of fields on clear

        5/28/92 - Back out preceding fix

        12/2/94 - "e" doesn't beep in logical fields

        4/12/95 - handle Unix function keys

        1/15/96 - Add ^K to delete to end of line

        9/5/96 - Add set_field()

	9/3/9 - Version 2.0 - Change control characters to be emacs (rather than
   	        wordstar) compatible

	10/25/10 - Version 3.0 - Add support for python

	11/4/10  - Version 3.1 - Make center a function so it can
		   dynamically center based on current screen width

	10/22/11 - Version 3.1.1 - Fix pointer dereference in Python
				   interface

	11/12/12 - Version 3.1.2 - Fix buffer overrun in my_itob by
				   range checking input
	8/3/13   - Version 3.1.3 - Buffer overrun in saya
	3/20/14  - Version 3.2   - Allow "/" in float fields
	3/21/14  - Version 3.2.1  - "/" didn't work on char fields
	1/23/15  - Version 3.3 - Add getl_no_default
	11/24/15 - Version 3.4 - Don't filter out ESC characters from
                                 saya
	3/19/15  - Version 4.0 - Support x and y < 1 as relative
	4/13/16  - Version 4.0.1 - Fix seg fault when ^E pressed in
				   floating-point field
	8/8/16   - Versioin 4.1 - Add rd_extended (for pbase)
	3/17/17  - Version 4.1.1 - Fix bug in python get_one
	12/23/19 - Version 4.1.2 - Doesn't accept leading blanks for
				   floats
	12/16/19 - Version 4.1.3 - Fix buffer overrun
	4/5/21   - Version 4.2 - clear must repaint all input fields
	5/19/21  - Version 4.2.1 - password field should not be displayed on clear
*/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"
#include "lscrn.h"

#define VERSION "4.2.1"

#ifdef IBM
#include <dos.h>
#endif

#define echo lputc (c)
#define ring lputc (7)

#ifdef PYTHON
#define next_char next_x (input_table[next_read].s)
#else
#define next_char next_x (input_table[next_read].addr)
#endif

#define next_num next_x (num_string)
#define TABSIZE 50
#define TYPE_A 1
#define TYPE_N 2
#define TYPE_L 3
#define TYPE_F 4
#define TYPE_P 5
#define TYPE_LN 6 /* logical with no default */
#define TYPE_LT 7 /* logical wwith tristate ('T', 'F', '?') */

/*
 *  Define BSISDEL if the backspace key sends the DEL key code
 */

#ifdef UNIX
#define BSISDEL
#endif

/* The following table contains one entry for each GETx call */

struct
{
        int x, y, type;
        int decimals;
	int n;
	double f;
	char *s;
	char *addr;
	int str_length;
}
        input_table [TABSIZE];

static int rd_internal(int return_on_change, const char* keys);
static int next_read = 0;       /* subscript for next field to be read */
static int next_get = 0;        /* subscript for next field to be written */
static int field_pos = 0;
static char num_string[17];
static char state_string[17];
static char *ptr;
static int cur_type;
static int c;
static int ins_mode = FALSE;
static int float_state;
static int float_slash = FALSE;
static void float_digit (void);
extern int lscrn_saved_x;

        /*                                                                 */
        /*  Floating states correspond to the following positions          */
        /*   within a floating point number:                               */
        /*                                                                 */
        /*    +3.14E+12                                                    */
        /*   01234 56789    <- float_state                                 */
        /*                                                                 */


static void next_field (void);
static void new_field (void);
static void my_itob (int n);
static void my_itof (double n, int decimals);
static void end_num (void);  /*  internal routine used by rd */
static void end_float (void);
static void float_digit (void);
static void next_x (char addr[]);  /* internal routine used by rd */
static void re_display (char *field, int incr);
static void ins_mode_off (void);
static void del_char (char *addr);
int gchs (void);

static int getx (int x, int y, char *ptr, int type, int decimals);
/* internal routine for common get handling */

#ifdef PYTHON
static int getx_return(int y, int x, int type);
/* internal routine for returning python values */
#endif

/*  INT_NAN has too many digits to be entered by a user */
#define INT_NAN -3141529

void erase (void)

/*  The erase routine clears the screen and sets the table of input
    fields to be empty.
*/

{
        clear ();
        clrscrn ();
}

void clear (void)

/*  The clear routine clears the internal table of input fields.  */

{
	int i;
	char temp_str[256];
	int j;

	normal ();

	for (i = 0; i < next_get; i++)
	{
		switch (input_table[i].type)
		{
			case TYPE_P:
			  for (j = 0; j < input_table[i].str_length; ++j)
			    temp_str[j] = '*';

			  temp_str[input_table[i].str_length]  = 0;

			  saya(input_table[i].y, input_table[i].x, temp_str);

			  free (input_table[i].s);
			  input_table[i].s = NULL;
			  break;

			case TYPE_A:
#ifdef PYTHON
				strcpy(temp_str, input_table[i].s);
#else
				strcpy(temp_str, input_table[i].addr);
#endif
				rpad(temp_str, input_table[i].str_length);
				saya(input_table[i].y, input_table[i].x,
				     temp_str);
				free (input_table[i].s);
				input_table[i].s = NULL;
				break;

			case TYPE_N:
				sayn(input_table[i].y, input_table[i].x,
				     input_table[i].n);
				break;

			case TYPE_LT:
			case TYPE_LN:
			case TYPE_L:
				sayl(input_table[i].y, input_table[i].x,
				     input_table[i].n);
				break;

			case TYPE_F:
				sayf(input_table[i].y, input_table[i].x,
				     input_table[i].f, input_table[i].decimals);
				break;
		}
	}
        next_read = next_get = 0;
}

/*  The get routines are used for input.  Each routine will display
    the current or default value and store information about the field
    for future use by the rd routine.

    There are five routines, one each for alphanumeric (string) data,
    numeric data, and logical data.
*/

void geta (int y, int x, char *s)

{
    reverse ();
    make_absolute(&x, &y);
    saya (y, x, s);
    normal ();
    getx (x, y, s, TYPE_A, 0);
}

void getp (int y, int x, char *s)

{
    char temp[256];
    temp[0] = 0;
    blanks (temp, (int) strlen (s));
    strcpy (s, temp);
    reverse ();
    make_absolute(&x, &y);
    saya (y, x, s);
    normal ();
    getx (x, y, s, TYPE_P, 0);
}

void getn (int y, int x, int *n)

{
        reverse ();
	make_absolute(&x, &y);
        sayn (y, x, *n);
        normal ();
        getx (x, y, ( char *) n, TYPE_N, 0);
}

void getl (int y, int x, int *l)

{
	reverse ();
	make_absolute(&x, &y);
	sayl (y, x, *l);
	normal ();
	getx (x, y, (char *) l, TYPE_L, 0);
}

void getl_tristate (int y, int x, int *l)

{
	reverse ();
	make_absolute(&x, &y);
	sayl (y, x, *l);
	normal ();
	getx (x, y, (char *) l, TYPE_LT, 0);
}

void getl_no_default (int y, int x, int *l)

{
	int i;
	reverse ();
	make_absolute(&x, &y);
	saya (y, x, "?");
	normal ();
	i = getx (x, y, (char *) l, TYPE_LN, 0);
#ifdef PYTHON
	input_table[i].n = 2;
#endif
}

void getf (int y, int x, double *n, int decimals)
{
	/*  If "decimals" is negative, then the number of decimal places
	 *  will be computed.
	 */

        reverse ();
	make_absolute(&x, &y);
        sayf (y, x, *n, decimals);
        normal ();
	getx (x, y, (char *) n, TYPE_F, decimals);
#ifdef PYTHON
	input_table[next_get-1].f = *n;
#endif
}

static int getx (int x, int y, char *ptr, int type, int decimals)
/* internal routine for common get handling */

{
	int i;

#ifdef PYTHON
	/*  For python code we have to not have duplicate x,y positions
	 *  in input_table, so we first see if we need to re-use the
	 *  existing entry
	 */
	i = getx_return (y, x, type);
	if ((i == -1) || (i > next_get))
#endif
		/*  If entry not in table, or not in Python mode then
		 *  use the next available entry
		 */
		i = next_get;

        if (i < TABSIZE)    /* gets are ignored if table is full */
        {
                input_table[i].x = x;
                input_table[i].y = y;
                input_table[i].type = type;
                input_table[i].decimals = decimals;

		if ((type == TYPE_N) || (type == TYPE_L) || (type == TYPE_LN) ||
			(type == TYPE_LT))
			input_table[i].n = *((int*) ptr);

		else if (type == TYPE_F)
			input_table[i].f = *((double*) ptr);

		else if ((type == TYPE_A) || (type == TYPE_P))
		{
			input_table[i].s = strdup (ptr);
			input_table[i].str_length = strlen(ptr);
		}
		else
			printf ("*** Invalid type ***\n");

		input_table[i].addr = ptr;
	}

	/*  If we used a new entry, then bump the next pointer */
	if (i == next_get)
		next_get++;

	return i;
}

#ifdef PYTHON
static int getx_return(int y, int x, int type)
{
	int i;

	for (i = 0; i < TABSIZE; i++)
	{
		if ((input_table[i].x == x) &&
		    (input_table[i].y == y) &&
		    (input_table[i].type == type))
			return i;
	}

	return -1;
}

char* geta_return(int y, int x)
{
	int i = getx_return(y, x, TYPE_A);
	if (i == -1)
		return NULL;
	else
		return input_table[i].s;
}

char* getp_return(int y, int x)
{
	int i = getx_return(y, x, TYPE_P);
	if (i == -1)
		return NULL;
	else
		return input_table[i].s;
}

int getn_return(int y, int x)
{
	int i = getx_return(y, x, TYPE_N);
	if (i == -1)
		return INT_NAN;
	else
		return input_table[i].n;
}

int getl_return(int y, int x)
{
	int i = getx_return(y, x, TYPE_L);
	if (i == -1)
	{
		i = getx_return(y, x, TYPE_LN);
		if (i == -1)
			return INT_NAN;
	}
	return input_table[i].n;
}

void getf_return2(int y, int x, double* value)
{
	int i = getx_return(y, x, TYPE_F);
	if (i == -1)
		/*  Return a real NAN */
		*value = 0.0 / 0.0;
	else
		*value = input_table[i].f;
}

void pysayf(int y, int x, double* value, int decimals)
{
	sayf (y, x, *value, decimals);
}
#endif

/* The set of say routines will display data on the screen.  Each
   one will handle one type of data:  alphanumeric, numeric, and logical.
*/

void saya (int y, int x, const char *s)

{
	static char* s2 = NULL;
	static int s2_len = -1;
	int s1_len = strlen(s) + 1;

	if ((s1_len*2) > s2_len)
	{
		if (s2 != NULL)
			free(s2);

		s2_len = s1_len * 2;
		s2 = malloc (s2_len);
		if (s2 == NULL)
		{
			erase ();
			fprintf (stderr, "*** OUT OF MEMORY ***\n");
			exit(1);
		}
	}


	char* p2 = s2;

	gotoxy (x, y);

	while (*s != 0)
	{
		if ((*s < ' ') && (*s != '\n') && (*s != '\t') && (*s != '\r')
		   && (*s != 27))
		{
			*p2++ = '^';
			*p2++ = *s++  + 'A' - 1;
		}
		else
			*p2++ = *s++;
	}

	*p2 = 0;

	lputs (s2);

	lscrn_saved_x += strlen(s2);

}


void center (int y, const char* s)
{
	int width = line_size();

	int x = strlen(s);
	x = width - x;
	x /= 2;

	if (x < 1)
	    x = 1;

	saya (y, x, s);
}

void sayn (int y, int x, int n)

{
        my_itob (n);  /* converts n to string in num_string */

        saya (y, x, num_string);
}

void sayl (int y, int x, int l)

{
        gotoxy (x, y);
        if (l == 1)
                lputc ('Y');
        else if (l == 0)
                lputc ('N');
	else
		lputc ('?');

	lscrn_saved_x += 1;
}

void sayf (int y, int x, double n, int decimals)

{
        my_itof (n, decimals);  /* converts n to string in num_string */

        saya (y, x, num_string);
}

int rd (void)

/*  The rd routine input data from the screen.  The screen locations to
    be used as input fields, and the format and location of the internal
    data must have been specified by preceding get calls.
*/

{
	return (rd_internal (FALSE, "") != 27);
}

/*
 *  rd_one works like rd except it returns immediately when a value has
 *  been set
 */

int rd_one(void)
{
	int result =  rd_internal (TRUE, "");
	normal ();
	return result;
}

int rd_extended(const char* keys)
{
	int result = rd_internal(FALSE, keys);
	normal ();
	return result;
}

static int rd_internal(int return_on_change, const char* keys)
{
        int *num_pointer;
        char *temp;
        int i, j;

#ifdef IBM
#ifndef WIN32API
        union REGS regs;
#endif
#endif
	/*  Hold window changes pending  */
	enum winch saved_winch = handle_winch(WINCH_QUEUE);

        ins_mode = FALSE;
        float_state = 0;
        strcpy (state_string, "\011\011\011\011\011\011\011\011\011");
        reverse ();

        new_field ();

        while (next_read < next_get)
        {
                /* read a char. from the console with no echo */

#ifdef IBM
                  c = gchs ();
#else
                  c = getachar ();

		  /* For reasons I don't understand, the changes I just
		   * made to getachar are causing the reverse video to
		   * be lost
		   */
		  reverse();
#endif

                 switch (c)
                 {
                        case 27:  normal ();
                                  ins_mode_off ();
				  handle_winch(saved_winch);
				  next_read = 0;
                                  return (c);  /* ESC */

                        case 'Y':
                        case 'y':
                        case 'T':
                        case 't':
				if ((cur_type == TYPE_LT)
				    || (cur_type == TYPE_LN)
				    || (cur_type == TYPE_L))
                                {
                                        echo;
#ifdef PYTHON
					input_table[next_read].n = TRUE;
#else
                                        num_pointer = (int *)
						input_table[next_read].addr;
					*num_pointer = TRUE;
#endif
                                        next_field ();
					if (return_on_change)
					{
						handle_winch(saved_winch);
/*****						next_read = 0;  *****/
						return (c);
					}
				}
                                else if (cur_type == TYPE_N)
                                        ring;
                                else if (cur_type == TYPE_F)
                                        ring;
                                else if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                        next_char;
                                break;

                        case 'N':
                        case 'n':
                        case 'F':
                        case 'f':
				if ((cur_type == TYPE_L)
				    || (cur_type == TYPE_LN)
				    || (cur_type == TYPE_LT))
                                {
                                        echo;
#ifdef PYTHON
					input_table[next_read].n = FALSE;
#else
					num_pointer = (int *)
						input_table[next_read].addr;
					*num_pointer = FALSE;
#endif
                                        next_field ();
					if (return_on_change)
					{
						handle_winch(saved_winch);
/*****						next_read = 0;  *****/
						return (c);
					}
                                }
                                else if (cur_type == TYPE_N)
                                        ring;
                                else if (cur_type == TYPE_F)
                                        ring;
                                else if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                break;

                        case '0':
				if ((cur_type == TYPE_L)
				    || (cur_type == TYPE_LT)
				    || (cur_type == TYPE_LN))
                                {
                                        echo;
#ifdef PYTHON
					input_table[next_read].n = FALSE;
#else
                                        num_pointer = (int *)
                                            input_table[next_read].addr;
                                        *num_pointer = FALSE;
#endif
                                        next_field ();
					if (return_on_change)
					{
						handle_winch(saved_winch);
/*****						next_read = 0;  *****/
						return (c);
					}
				}
                                else if (cur_type == TYPE_N)
                                        next_num;
                                else if (cur_type == TYPE_F)
                                    float_digit ();
                                else if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                break;

                        case '1':
				if ((cur_type == TYPE_L)
				    || (cur_type == TYPE_LT)
				    || (cur_type == TYPE_LN))
                                {
                                        echo;
#ifdef PYTHON
					input_table[next_read].n = TRUE;
#else
                                        num_pointer = (int *)
						input_table[next_read].addr;
					*num_pointer = TRUE;
#endif
                                        next_field ();
					if (return_on_change)
					{
						handle_winch(saved_winch);
/*****						next_read = 0;  *****/
						return (c);
					}
                                }
                                else if (cur_type == TYPE_N)
                                        next_num;
                                else if (cur_type == TYPE_F)
                                    float_digit ();
                                else if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                break;

                        case '+':
                        case '-':
                                if (cur_type == TYPE_F)
                                {
                                    switch (float_state)
                                    {
                                        case 0:
                                        case 5: next_num;
                                                ++float_state;
                                                break;
                                        default:    ring;
                                    }
                                    break;
                                }

                                if ((field_pos != 0) && (cur_type == TYPE_N))
                                {
                                        ring;
                                        break;
                                }

                                /* fall through to digit routine */

                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                                if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                else if (cur_type == TYPE_N)
                                        next_num;
                                else if (cur_type == TYPE_F)
                                    float_digit ();
				else if (cur_type == TYPE_L)
					ring;
				else if (cur_type == TYPE_LN)
					ring;
				else if (cur_type == TYPE_LT)
					ring;
                                break;

                        case 16:  /* ^P */
                        case 0x4800:  /* up arrow */
                                ins_mode_off ();
                                if (cur_type == TYPE_N)
                                        end_num ();
                                if (cur_type == TYPE_F)
                                        end_float ();
                                if (next_read == 0)
                                        ring;
                                else
                                {
                                        next_read--;
                                        new_field ();
                                }
                                break;

                        case 14:  /* ^N */
                        case 13:  /* CR */
                        case 10:  /* LF */
                        case 9:   /* Tab */
                        case 0x5000:  /* down arrow */
                                next_field ();
				if (return_on_change)
				{
					handle_winch(saved_winch);
					/* Re-instated next statement
					 * to try to fix bug in Python
					 * version of get_one */
					next_read = 0;
					return (c);
				}
                                break;

#ifdef BSISDEL
			case 0x7f:  /* DEL */
#endif
			case 0x4E00:  /*  left arrow key */
			case 8:   /* backspace */
                        case 2:  /* ^B */
                                if (field_pos == 0)
                                        ring;
                                else
                                {
                                        field_pos--;
                                        float_state = state_string[field_pos];
					lputc (8);
				}

				/*
				 *  If the user pressed backspace, fall
				 *  through to the delete routine;
				 *  otherwise break
				 */

#ifdef BSISDEL
				if ((c != 0x7f) && (c != 8))
#else
				if (c != 8)
#endif
					break;


				/*
				 *  If we're not in insert mode then
				 *  treat backspace like left arrrow
				 */

				if (!ins_mode)
					break;

			case 0x5300:    /*  Delete key                        */
			case 4:         /*  ^D */
#ifndef BSISDEL
			case 127: /* DEL */
#endif
				if (cur_type == TYPE_A)
#ifdef PYTHON
					del_char (input_table[next_read].s);
#else
					del_char (input_table[next_read].addr);
#endif
				else if (cur_type == TYPE_N)
					del_char (num_string);
				else
					ring;
				break;


                        case 11: /* ^K */
                                if ((cur_type == TYPE_N)
                                ||  (cur_type == TYPE_F))
				{
					ptr = num_string;
					float_slash = FALSE;
				}
                                else
#ifdef PYTHON
					ptr = input_table[next_read].s;
#else
					ptr = input_table[next_read].addr;
#endif

                                for (i = field_pos; ptr[i] != 0; ++i)
                                    ptr[i] = ' ';

                                re_display (ptr, 0);
                                break;

                        case 6:  /* ^F */
                        case 0x4D00:  /*  right arrow key */
                                if (cur_type == TYPE_A)
                                {
                                        field_pos++;
#ifdef PYTHON
					ptr = input_table[next_read].s;
#else
					ptr = input_table[next_read].addr;
#endif
                                        if (ptr[field_pos] == 0)
					{
						next_field ();
						if (return_on_change)
						{
							handle_winch(saved_winch);
/*****							next_read = 0;
 *****							*****/
							return (c);
						}
					}
                                        else
					{
                                      /* move cursor forward */
                                                gotoxy
                                      (input_table[next_read].x + field_pos,
				       input_table[next_read].y);
					}
                                }
                                if (cur_type == TYPE_N)
                                {
                                        if (field_pos == 9)
					{
                                                next_field ();
						if (return_on_change)
						{
							handle_winch(saved_winch);
/*****							next_read = 0;
 *****							*****/
							return (c);
						}
					}
                                        else
                                        {
                                                field_pos++;
                                      /* move cursor forward */
                                                gotoxy
                                      (input_table[next_read].x + field_pos,
                                         input_table[next_read].y);
                                        }
                                }
                                if (cur_type == TYPE_F)
                                {
                                        if (field_pos == 15)
					{
                                                next_field ();
						if (return_on_change)
						{
							handle_winch(saved_winch);
/*****							next_read = 0;
 *****							*****/
							return (c);
						}
					}
                                        else
                                        {
                                                field_pos++;

                                                float_state =
                                                    state_string[field_pos];
                                        /* move cursor forward */
                                                gotoxy
                                      (input_table[next_read].x + field_pos,
                                         input_table[next_read].y);
                                        }
                                }
				if ((cur_type == TYPE_L)
				    || (cur_type == TYPE_LT)
				    || (cur_type == TYPE_LN))
				{
                                        next_field ();
					if (return_on_change)
					{
						handle_winch(saved_winch);
/*****						next_read = 0;  *****/
						return (c);
					}
				}
                                break;

                        case 17:  /* ^Q */
                        case 23:  /* ^W */

                                next_field ();
                                next_read = next_get;
                                break;

                        case ' ':
                                if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                if (cur_type == TYPE_N)
                                        next_num;
                                else if (cur_type == TYPE_F)
                                {
                                        next_num;
					if (float_state != 0)
						float_state = 9;
                                }
                                if (cur_type == TYPE_L)
					ring;
				if (cur_type == TYPE_LT)
					ring;
				if (cur_type == TYPE_LN)
					ring;
                                break;

                        case 0x5200:    /*  INS key                        */
                        case 20:        /*  ^T */

                                if (ins_mode)
                                    ins_mode_off ();
                                else
                                {  /*  Turn ins-mode on  */
                                    ins_mode = TRUE;
                                    ring;
#ifdef IBM
#ifndef WIN32API
                                    regs.h.ah = 1;
                                    regs.h.ch = 4;
                                    regs.h.cl = 7;
                                    int86 (0x10, &regs, &regs);
#endif
#endif
                                }  /*  Turn ins-mode on  */

                                break;

                        case 0x4700:    /*  HOME key                       */
                        case 1:         /* ^A */
                                field_pos = 0;
				float_state = 0;
				float_slash = FALSE;
                                gotoxy (input_table[next_read].x,
                                    input_table[next_read].y);
                                break;

                        case 0x4F00:    /*  END key                        */
                        case 5:         /*  ^E  */
                                if (cur_type == TYPE_A)
#ifdef PYTHON
					temp = input_table[next_read].s;
#else
					temp = input_table[next_read].addr;
#endif
				else if ((cur_type == TYPE_N) ||
					 (cur_type == TYPE_F))
                                    temp = num_string;
                                else
				{
					ring;
					break;
				}

                                for (i = 0, j = 0; temp[i] != 0;  ++i)
                                    if (temp[i] != ' ')
                                        j = i;

				if (temp[j+1] == 0)
					j--;

                                float_state = state_string[j];
                                field_pos = ++j;
                                gotoxy (input_table[next_read].x + j,
                                    input_table[next_read].y);
                                break;

			case '/':   if (cur_type == TYPE_F)
				    {
					    if (float_slash == FALSE)
					    {
						    float_slash = TRUE;
						    float_state = 0;
						    next_num;
					    }
					    else
						    ring;
				    }
				    else if (cur_type == TYPE_P)
					    next_char;
				    else if (cur_type == TYPE_A)
					    next_char;
				    else
					    ring;
				    break;

                        case '.':   if (cur_type == TYPE_F)
                                        if ((float_state == 2)
                                        ||  (float_state == 0)
                                        ||  (float_state == 1))
                                        {
                                            next_num;
                                            float_state = 3;
                                        }
                                        else
                                            ring;
				    else if (cur_type == TYPE_P)
					    next_char;
				    else if (cur_type == TYPE_A)
					    next_char;
                                    else
                                        ring;
                                    break;

			case '?':   if ((cur_type == TYPE_LN) ||
				        (cur_type == TYPE_LT))
				    {
					    echo;
#ifdef PYTHON
					    input_table[next_read].n = -1;
#else
					    num_pointer = (int *)
						    input_table[next_read].addr;
					    *num_pointer = -1;
#endif

					    next_field();
				    }
				    else if (cur_type == TYPE_A)
					    next_char;
				    else
					    ring;
				    break;

                        case 'e':
                        case 'E':   if (cur_type == TYPE_F)
                                        if ((float_state > 0)
                                        &&  (float_state < 5))
                                        {
                                            next_num;
                                            float_state = 5;
                                        }
                                        else
                                            ring;
                                    else if (cur_type == TYPE_P)
                                        next_char;
                                    else if (cur_type == TYPE_A)
                                        next_char;
                                    else
                                        ring;
                                    break;

                        default:
				temp = index(keys, c);
				if (temp != NULL)
					return c;

                                if (cur_type == TYPE_A)
                                        next_char;
                                else if (cur_type == TYPE_P)
                                    next_char;
                                else
                                        ring;
                }  /* end of switch */
        }

        normal ();
	handle_winch(saved_winch);
	/* Re-instated next statement
	 * to try to fix bug in Python
	 * version of get_one */
	next_read = 0;
        return(0);
}

/*                                                                         */
/*  set_field is used to specify which field the cursor whould be in       */
/*                                                                         */

void set_field (int y, int x)
{
    int i;

    for (i = 0; i < next_get; ++i)
        if ((input_table[i].x == x) && (input_table[i].y == y))
        {
            next_read = i;
            return;
        }
}

static void float_digit (void)
{
                                    switch (float_state)
                                    {
                                        case 0:
                                        case 1:
                                        case 2: next_num;
                                                float_state = 2;
                                                break;
                                        case 3:
                                        case 4: next_num;
                                                float_state = 4;
                                                break;
                                        case 5:
                                        case 6:
                                        case 7: next_num;
                                                float_state = 8;
                                                break;
                                        case 8: next_num;
                                                float_state = 9;
                                                break;
                                        case 9: ring;
                                    }

}
static void next_field (void)  /*  internal routine  used by rd */

/* This routine will bump to the next field in input_table */

{
        ins_mode_off ();
        field_pos = 0;
        float_state = 0;
        strcpy (state_string, "\011\011\011\011\011\011\011\011\011");

        if (cur_type == TYPE_A)
#ifdef PYTHON
		ptr = input_table[next_read].s + field_pos;
#else
		ptr = input_table[next_read].addr + field_pos;
#endif
        if (cur_type == TYPE_N)
                end_num ();
        if (cur_type == TYPE_F)
                end_float ();
        next_read++;
        if (next_read < next_get)
                new_field ();
}

static void new_field (void)  /* internal routine used by rd */

/*  This routine will handle a new field.  It is called directly
    by rd for the first field and by next_field for subsequent fields */

{
        gotoxy (input_table[next_read].x, input_table[next_read].y);
#ifdef PYTHON
	if ((cur_type = input_table[next_read].type) == TYPE_N)
		my_itob (input_table[next_read].n);
	if ((cur_type = input_table[next_read].type) == TYPE_F)
		my_itof (input_table[next_read].f,
			 input_table[next_read].decimals);
#else
	int *n_ptr = (int*) input_table[next_read].addr;
	double *f_ptr = (double*) input_table[next_read].addr;
	if ((cur_type = input_table[next_read].type) == TYPE_N)
		my_itob (*n_ptr);
	if ((cur_type = input_table[next_read].type) == TYPE_F)
		my_itof (*f_ptr, input_table[next_read].decimals);
#endif
	field_pos = 0;
	float_slash = FALSE;
}

static void my_itob (int n)  /* internal routine used by rd */

/*  This routine will convert its argument, n, into a fixed-length string
    in the global variable, num_string */

{
        int i;
        char s[11];

	if (n > 2147483647)
	{
		printf ("\nNumber out of range:  %d\n", n);
		exit(1);
	}

        strcpy (num_string, "         ");

#ifdef QC
        itob (n, s, -10);

        i = strlen (s);
#endif

#ifdef C86
        i = itoa (n, s);
#endif

#ifdef vax
        sprintf (s, "%d", n);
        i = strlen (s);
#endif

#ifdef IBM
        sprintf (s, "%d", n);
        i = strlen (s);
#endif

#ifdef UNIX
        sprintf (s, "%d", n);
        i = (int) strlen (s);
#endif

#ifdef OLDAC
        sprintf (s, "%d", n);
        i = strlen (s);
#endif

        strcpy (&num_string [10-i], s);
}

static void my_itof (double n, int decimals)  /* internal routine used by rd */

/*  This routine will convert its argument, n, into a fixed-length string
    in the global variable, num_string */

{
        int i;
        char mask[116];
        char s[70];
        double n_prime;

        blanks (num_string, 16);
	if (decimals < 0)
		strcpy(mask, "%16f");
	else
		sprintf (mask, "%%16.%df", decimals);

	sprintf (s, mask, (double) n);

	if (decimals < 0)
	{
		while (TRUE)
		{
			int last_char = strlen(s) - 1;
			if (s[last_char] == '0')
			{
				s[last_char] = 0;
				continue;
			}
			break;
		}
	}

        i = (int) strlen (s);
        n_prime = (double) atof (s);

        if ((i > 16) || ((n_prime == 0.0) && (n != 0.0)))
        {  /*  f-format number is too long;  use e-format  */
            sprintf (s, "%16.3e", (double) n);
            i = (int) strlen (s);
            if (i > 16)
                i = 16;
        }  /*  f-format number is too long;  use e-format  */

        strcpy (&num_string [16-i], s);
}

static void end_num (void)  /*  internal routine used by rd */

/*  This routine handles end-of-field cleanup for numeric fields.
    It converts the number being entered to binary form and stores it
    in the user variable.  It also re-displays the field with the
    number right-justified
*/

{
	input_table[next_read].n = atoi (num_string);

#ifdef PYTHON
	sayn (input_table[next_read].y, input_table[next_read].x,
	      input_table[next_read].n);
#else
	int *n_ptr;

	n_ptr = (int *) input_table[next_read].addr;

	*n_ptr = atoi (num_string);
	sayn (input_table[next_read].y, input_table[next_read].x, *n_ptr);
#endif
}

static void end_float (void)  /*  internal routine used by rd */

/*  This routine handles end-of-field cleanup for floating-point fields.
    It converts the number being entered to binary form and stores it
    in the user variable.  It also re-displays the field with the
    number right-justified
*/

{
	double value;
	double numerator;
	double denominator;
	char* slash;

	if (float_slash)
	{
		slash = index(num_string, '/');
		if (slash == NULL)
			value = (double) atof(num_string);
		else
		{
			*slash = 0;
			numerator = (double) atof(num_string);
			slash++;
			denominator = (double) atof(slash);
			value = numerator / denominator;
		}
	}
	else
		value = (double) atof(num_string);

	float_slash = FALSE;

	input_table[next_read].f = value;

#ifdef PYTHON
	sayf (input_table[next_read].y, input_table[next_read].x,
	      input_table[next_read].f, input_table[next_read].decimals);
#else
	double *f_ptr;

	f_ptr = (double *) input_table[next_read].addr;

	*f_ptr = value;
	sayf (input_table[next_read].y, input_table[next_read].x, *f_ptr,
	      input_table[next_read].decimals);
#endif
}

static void next_x (char addr[])  /* internal routine used by rd */

/*  This routine puts the character just entered into the appropriate
    string (user-specified for alpha fields, num_string for numeric fields)
    and also puts the character to the screen
*/

/*  note that access to this routine is via the macros next_char and
    next_num */

{
    int j;

    if (ins_mode)
    {  /*  ins_mode is on  */
        for (j = field_pos; addr[j] != 0; ++j) /* null body */ ;

        for (--j; j > field_pos; --j)
            addr[j] = addr[j - 1];      /*  Move chars. right              */

        addr[field_pos] = (char) c;
        re_display (addr, 1);
    }  /*  ins_mode is on  */
    else
    {  /*  ins_mode is off  */
        if (cur_type == TYPE_P)
            lputc ('*');
        else
            echo;
        addr[field_pos] = (char) c;
    }  /*  ins_mode is off  */

    if (field_pos < 11)
        state_string[field_pos] = (char) float_state;

    if (addr[++field_pos] == 0)
    {
	    /*
	     *  For single-character fields we advance to the next
	     *  field at the end of a field.  For everything else we
	     *  beep and stay where we were
	     */

	    if (field_pos == 1)
		    next_field ();
	    else
	    {
		    ring;
		    --field_pos;
		    lputc (8);
	    }
    }
}

static void re_display (char *field, int incr)

{
    char mask[256];
    unsigned int i;

    strcpy (mask, &field[field_pos]);

    if (cur_type == TYPE_P)
    {
        ztrim (mask);

        for (i = 0; i < strlen (mask); ++i)
            mask[i]= '*';

        rpad (mask, (int) strlen (&field[field_pos]));

    }

    saya (input_table[next_read].y, input_table[next_read].x + field_pos,
          mask);

    gotoxy (input_table[next_read].x + field_pos + incr,
        input_table[next_read].y);
}


static void ins_mode_off ()

{
#ifdef IBM
#ifndef WIN32API
    union REGS regs;
#endif
#endif

    ins_mode = FALSE;

#ifdef IBM
#ifndef WIN32API
    regs.h.ah = 1;                      /*  Set cursor type                */
    regs.h.ch = 6;                      /*  Begin line                     */
    regs.h.cl = 7;                      /*  End line                       */
    int86 (0x10, &regs, &regs);         /*  Video BIOS routine             */
#endif
#endif
}

static void del_char (char *addr)

{
    int j;

    for (j = field_pos; addr[j] != 0; ++j)
    {
	    if (addr[j] == '/')
		    float_slash = FALSE;

	    addr[j] = addr[j + 1];      /*  Move chars. left               */
    }

    addr[j-1] = ' ';
    addr[j] = 0;
    re_display (addr, 0);
}

VERSION_PROC (hscrn_version)
