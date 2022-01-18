/*

     getachar.c

     Glenn Story - 5/28/86

     This routine gets a single character without echo from the terminal.

     UNIX Version (POSIX)

*/

#include "gs.h"
#include "lscrn.h"
#include <fcntl.h>
#ifdef SOLARIS
#include <sys/ttold.h>
#endif
#include <errno.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "gsvers.h"
#include <sys/ioctl.h>

#define VERSION

#define USEC_TO_WAIT 500

#define ERROR_RETURN 1
#define ERROR -1
#define FALSE 0

static enum getachar_ctrlc getachar_ctrlc_state = GETACHAR_CTRLC_RETURN;
static int ctrlc_pending = FALSE;

static struct termios save_status;
static int save_status_set = FALSE;
static int terminal;
static int first_time = TRUE;
static int queued_character = 0;
static enum winch winch_state = WINCH_IGNORE;
static int winch_pending = FALSE;

static void catch_winch_signal (int sig_type);

typedef void (*sighandler_t)(int);
static sighandler_t old_sigint_handler = NULL;

static int mode_set = FALSE;
static char last_char = -1;

void dump_termios(void)
{
	struct termios term_status;

	if (tcgetattr (terminal, &term_status))
	{
		perror
	   	      ("*** ERROR *** Unexpected return code from tcgetattr");
		exit (ERROR_RETURN);
	}

	printf("\n");
	printf("termios structure:\n");
	printf("c_iflag:  0x%lx\n", term_status.c_iflag);
	printf("c_oflag:  0x%lx\n", term_status.c_oflag);
	printf("c_cflag:  0x%lx\n", term_status.c_cflag);
	printf("c_lflag:  0x%lx\n", term_status.c_lflag);
	printf("\n");
	printf("c_cc:\n");
        printf("   VINTR:     0x%x\n", term_status.c_cc[0]);
        printf("   VQUIT:     0x%x\n", term_status.c_cc[1]);
        printf("   VERASE:    0x%x\n", term_status.c_cc[2]);
        printf("   VKILL:     0x%x\n", term_status.c_cc[3]);
        printf("   VEOF:      0x%x\n", term_status.c_cc[4]);
        printf("   VTIME:     0x%x\n", term_status.c_cc[5]);
        printf("   VMIN:      0x%x\n", term_status.c_cc[6]);
        printf("   VSWTC:     0x%x\n", term_status.c_cc[7]);
        printf("   VSTART:    0x%x\n", term_status.c_cc[8]);
        printf("   VSTOP:     0x%x\n", term_status.c_cc[9]);
        printf("   VSUSP:     0x%x\n", term_status.c_cc[10]);
        printf("   VEOL:      0x%x\n", term_status.c_cc[11]);
        printf("   VREPRINT:  0x%x\n", term_status.c_cc[12]);
        printf("   VDISCARD:  0x%x\n", term_status.c_cc[13]);
        printf("   VWERASE:   0x%x\n", term_status.c_cc[14]);
        printf("   VLNEXT:    0x%x\n", term_status.c_cc[15]);
        printf("   VEOL2:  0x%x\n", term_status.c_cc[16]);
	printf("\n");
	printf("c_ispeed:  0x%lx\n", term_status.c_ispeed);
	printf("c_ospeed:  0x%lx\n", term_status.c_ospeed);
}

void dump_getachar_state(void)
{
	if (save_status.c_iflag == 0)
		printf ("save_status has not been set.\n");
	else
		printf ("save_status has been set.\n");

	printf("save_status_set = %d\n", save_status_set);
	printf(" terminal = %d\n", terminal);
	printf("first_time = %d\n", first_time);
	printf("queued_character = %d\n", queued_character);
	printf("winch winch_state = %d\n", winch_state);
	printf("winch_pending = %d\n", winch_pending);
	printf("Last char = '%c' (0x%x) \n", last_char, last_char);

	dump_termios();
}

int getachar_set_mode(int fcntl_flag)
{
	struct termios term_status;

	if (mode_set)
		return FALSE;
    /*
     *  NOW PUT THE TERMINAL INTO CBREAK MODE
     */

	if (tcgetattr (terminal, &term_status))
	{
		perror
				("*** ERROR *** Unexpected return code from tcgetattr");
		exit (ERROR_RETURN);
	}

	if (!save_status_set)
	{
		memcpy (&save_status, &term_status, sizeof (term_status));
		save_status_set = TRUE;
	}

	term_status.c_iflag |= INLCR;
	term_status.c_lflag &= ~(IEXTEN | ECHO | ICANON);
	term_status.c_lflag |= ISIG;
	term_status.c_cc[VMIN] = 1; /* 1 char mininum to return */

	if (tcsetattr (terminal, TCSANOW, &term_status))
	{
		perror
				("*** ERROR *** Unexpected return code from tcsetattr");
		exit (ERROR_RETURN);
	}

	/*
         * Set the flag as requested by the caller
         */

	if (fcntl (terminal, F_SETFL, fcntl_flag) == ERROR)
	{
		perror ("Error from fcntl F_SETFL");
		exit (ERROR_RETURN);
	}

	mode_set = TRUE;
	return TRUE;
}
	
static int getachar_common (int fcntl_flag)
{
    static int save_flag = -1;
    
        int i;
        char c;
        int r;                  /* Return value */
        char escape_sequence[3];
	int mode_set_by_this_call = FALSE;
	
        /*
         *  FIRST FLUSH THE OUTPUT STREAM
         */

        fflush (stdout);
        fflush (stderr);

	/*  If we are handling winch signals and one was received, turn
	 *  it into a ^L */
	if ((winch_state == WINCH_NOTIFY) && winch_pending)
	{
		winch_pending = FALSE;
		return 0x0c;
	}

	/*  If we have a queued character, just return that */

	if (queued_character != 0)
	{
		int temp_char = queued_character;
		queued_character = 0;
		return temp_char;
	}
	
        /*
         * If first time, initialize the terminal
         */

        if (first_time)
        {   /* Set signals */
/*	    terminal = open ("/dev/tty", O_RDONLY);*/
	    terminal = 0;

	    save_flag = -1;

	    if (terminal == -1)
	      {
		perror (
                  "*** ERROR *** Unexpected return code from open of tty");
		exit (ERROR_RETURN);
	      }

	    signal (SIGWINCH, catch_winch_signal);
	    atexit (reset_terminal);

	    first_time = FALSE;
	}   /* Set signals */


	mode_set_by_this_call = getachar_set_mode(fcntl_flag);
	
        /*
         *  NOW READ THE CHARACTER
         */

        if ((i = read (terminal, &c, 1)) == 0)
	    c = 0;
        else if (i == -1)
        {  /* error from first read */
		if ((errno == EWOULDBLOCK) ||
		    (errno == EAGAIN) ||
		    (errno == EINTR))
                c = 0;
            else
            {
                perror ("*** ERROR *** Unexpected error from read of STDIN");
                exit (ERROR_RETURN);
            }
        }  /* error from first read */

	/*  Do we want to return a ^C? */
	if ((getachar_ctrlc_state == GETACHAR_CTRLC_RETURN) && ctrlc_pending)
	{
		ctrlc_pending = FALSE;
		if (mode_set_by_this_call)
			reset_terminal();
		
		return 3;
	}

	/*  change ^Y to ESC  */
	if (c == 25)
		c = 27;

	/*  Debugging code */
	if (c == 24) /* ^X */
	{
		/*  Issue diagnostics */
		printf ("\nfcntl_flag = %d\n", fcntl_flag);
		dump_getachar_state();

		if (mode_set_by_this_call)
			reset_terminal();

		/*  Try again */
		return getachar_common( fcntl_flag);
	}
	/* End debugging code */

	if (c != 0)
		last_char = c;

        if (c == 033 /* ESC */)
        {  /* Handle escape sequences */
            if (fcntl (terminal, F_SETFL, O_NDELAY) == ERROR)
            {
                perror ("Error from fcntl F_SETFL");
                exit (ERROR_RETURN);
            }

	    save_flag = -1;

            if ((i = read (terminal, escape_sequence, 2)) == 0)
	        r = 033;  /* ESC */
	    else if (i == -1)
            {  /* error from second read */
                if (errno == EWOULDBLOCK)
                    r = 033;    /* ESC */
                else
                {
                  perror ("*** ERROR *** Unexpected error from read of STDIN");
                  exit (ERROR_RETURN);
                }
            }  /* error from second read */
            else
            {  /* decode escape sequence */
                if ((escape_sequence[0] == '[') || (escape_sequence[0] == 'O'))
                {  /* Good ANSI sequence */
                    switch (escape_sequence[1])
                    {
                    case '2':  r = 0x5200;  break; /* INS */
                    case '6':  r = 0x5100;  break; /* page down */
                    case '5':  r = 0x4900;  break; /* page up */
                    case 'A':  r = 0x4800;  break; /* up arrow*/
                    case 'B':  r = 0x5000;  break; /* down arrow */
		    case '1':  r = 0x4700;  break; /* home */
		    case '3':  r = 0x5300;  break; /* DEL */
		    case '4':  r = 0x4f00;  break; /* end */
		    case 'C':  r = 0x4d00;  break; /* right arrow */
		    case 'D':  r = 0x4e00;  break; /* left arrow */
		    case 'H':  r = 0x4700;  break; /* home */
		    case 'F':  r = 0x4f00;  break; /* end */
		    case 'E':  r = 0x9876;  break; /* Numeric keypad 5 */
		    case 'M':  r = 0x5432;  break; /* Numeric keypad enter */
		    case 'm':  r = 0x5431;  break; /* numeric - */
		    case 'o':  r = 0x5433;  break; /* numeric / */
		    case 'j':  r = 0x5434;  break; /* numeric * */
		    case 'k':  r = 0x5435;  break; /* numeric + */

		    default:
			    fprintf (stderr,
		     "*** ERROR *** Unexpected escape sequence: %s (%c)\n",
				     escape_sequence, escape_sequence[1]);

			    r = -1; /* try again */
			    break;
		    }
		}  /* Good ANSI sequence */
                else
                {  /* Bad ANSI sequence */
                    fprintf (stderr,
                             "*** ERROR *** Unexpected escape sequence: %s\n",
                             escape_sequence);
		    for (i = 0; escape_sequence[i] != 0; i++)
		    {
		      fprintf (stderr, "0x%x ", escape_sequence[i]);
		    }

                    r = -1;     /* try again */

                }  /* Bad ANSI sequence */

		/*  Swallow extra escape-sequence characters */
		while ((i =  read (terminal, escape_sequence, 1)) == 1)
			/*  empty loop body */ ;

            }  /* decode escape sequence */

            /*
             *  Go back to blocking mode
             */

        }  /* Handle escape sequences */
        else
            r = c;              /* Not ESC so use original character*/

        if (r == '\n')
            r = '\r';

        if (r == -1)
            r = getachar_common(fcntl_flag);    /* Try again */

	if (mode_set_by_this_call)
		reset_terminal();
	
        return (r);

}
int getachar ()
{
	int rc;

	if ((winch_state == WINCH_IGNORE)
         || (winch_state == WINCH_QUEUE))
	{
		rc =  getachar_common (0);
		if (rc == 0)
		{
			/*  Should not recieve 0 -- try to reset the
			 *  terminal
			 */
			reset_terminal();
			rc =  getachar_common(0);
			if (rc == 0)
			{
				fprintf(stderr, "\nCan't reset terminal\n");
				abort();
			}
		}
		return rc;
	}

	getachar_set_mode(O_NONBLOCK);

	while (TRUE)
	{
		rc = getachar_common(O_NONBLOCK);
		if (rc == 0)
			usleep(CHARWAIT_TIME);
		else
		{
			reset_terminal();
			return rc;
		}
	}
}

int getachar_nowait ()
{
	int c;
	
	getachar_set_mode(O_NONBLOCK);
	
	c = getachar_common (O_NONBLOCK);
	if (c != 0)
		reset_terminal();
	return c;
	  
}


/**********************************************************************
*
* FUNCTION: queueachar
*
* DESCRIPTION:
*       Queue a character to be passed by getachar.
*       Current implementation only saves one character, but this could
*       be changed to a real queue if needed.
*
* CALLING SEQUENCE:
*       queueachar(character);
*
* INPUT PARAMETERS:
*       character    The character to be queued
*
*
* OUTPUT PARAMETERS:
*       None
*
* RETURNED VALUE:
*       None
*
* EXTERNAL FUNCTIONS CALLED:
*       {@external_functions_called...@}
*
* GLOBAL DATA MODIFIED:
*       {@global_data_modified...@}
*
* PSEUDO CODE:
*       {@detailed_pseudo_code@}
*
**********************************************************************/

void queueachar(int character)
{
	queued_character = character;
}

void reset_terminal (void)
{
	first_time = TRUE;  /*  reset so next time through will re-init */

	if (fcntl (terminal, F_SETFL, 0) == ERROR)
	{
		if (errno != 9)
			perror ("Error from fcntl F_GETFL"); 
	}

	if (save_status_set)
		if (tcsetattr (terminal, TCSANOW, &save_status))
		{
			if (errno != 9)
				perror (
		     "*** ERROR *** Unexpected return code from tcsetattr");
		}

/*    close (terminal); */

	normal ();
	mode_set = FALSE;
}

/**********************************************************************
*
* FUNCTION: catch_winch_signal
*
* DESCRIPTION:
*       Catch and handle the winch signal
*
* CALLING SEQUENCE:
*       catch_signal (sig_type)
*
* INPUT PARAMETERS:
*       sig_type              Type of signal
*
*
* OUTPUT PARAMETERS:
*       None
*
* RETURNED VALUE:
*       None
*
* EXTERNAL FUNCTIONS CALLED:
*       {@external_functions_called...@}
*
* GLOBAL DATA MODIFIED:
*       {@global_data_modified...@}
*
* PSEUDO CODE:
*       {@detailed_pseudo_code@}
*
**********************************************************************/


static void catch_winch_signal (int sig_type)
{
	if (winch_state != WINCH_IGNORE)
		winch_pending = TRUE;
}


int window_size_changed(void)
{
	static int old_window_area = -1;
	int new_window_area;
	struct winsize window;

	ioctl (2, TIOCGWINSZ, &window);
	new_window_area = window.ws_row * window.ws_col;

	if (new_window_area == old_window_area)
		return FALSE;
	else
	{
		old_window_area = new_window_area;;
		return TRUE;
	}
}

enum winch handle_winch(enum winch new_value)
{
	enum winch old_value = winch_state;
	winch_state = new_value;
	return old_value;
}

VERSION_PROC (getachar_version)
