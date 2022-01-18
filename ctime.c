/**********************************************************************
*
* MODULE: ctime.c
*
* BRIEF DESCRIPTION:
*       convert time to numeric form
*
* MODULE TYPE:
*       Source
*
* FULL DESCRIPTION:
*       This file contains the c_time function which converts an ascii
*       string into a numeric value equal to the number of minutes past
*       midnight which the time represents
*
* FUNCTIONS DEFINED:
*       c_time
*	c_time_2
*	c_time_3
*	ctime_error
*	ctime_remainder
*	ctime_help
*       p_time                          {@description...@}
*       {@functions_defined...@}
*
* GLOBAL DATA DEFINED:
*       {@global_data_defined...@}
*
* REVISION HISTORY:
*       12Mar93      G. Story        Original release
*       19Oct94      G. Story        Added c_time_2
*                                    Added p_time
*       02/26/2000, GS, Allow (ignore) seconds in time
        {@revision_history...@}
*
**********************************************************************/

/* Global Include Files */

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#define VERSION

#include "gs.h"

/* Subsystem Global Include Files */


/* Local Include Files */


/* Local Defines */


/* Global Data Referenced */

static int has_colon;
static char *error_message = "No error has been set";
static char errbuf[500];
static char *in_time;
static char remainder[501];

/* External Functions Referenced */

/***************************************************************************/
/* NAME                                                                    */
/*      c_time_x - Internal routine used by c_time_2 and c_time_3. */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This function will parse an input time and convert it into an      */
/*   internal form which is an integer expressing a number of minutes.     */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      int c_time_x (char * in_time, int use_military)                    */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           in_time  (R) - String to be parsed                            */
/*           use_military (R) - TRUE to accept military time */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           The time or:                                                  */
/*               -1 = invalid input                                        */
/*                0 = blank input                                          */
/*                                                                         */
/***************************************************************************/


   static int c_time_x (const char * callers_time, int use_military)
{
	static char my_time[80];
	int hours;
	int minutes;
	int military_time = FALSE;

	has_colon = FALSE;

	strncpy (my_time, callers_time, 80);
	my_time[79] = 0;
	ztrim (my_time);
	in_time = my_time;

    /*                                                                     */
    /*  SKIP LEADING BLANKS                                                */
    /*                                                                     */

	for ( ; *in_time == ' '; ++in_time)
		;

    /*                                                                     */
    /*  EMPTY INPUT IS 0 TIME                                              */
    /*                                                                     */

	if (*in_time == 0)
		return 0;

    /*                                                                     */
    /*  PARSE THE HOURS                                                    */
    /*                                                                     */

	if (!isdigit (*in_time))
	{
		sprintf (errbuf, "non-numeric character:  '%c'", *in_time);
		error_message = errbuf;
		return -1;
	}

	hours = 0;

	while (isdigit (*in_time))
	{
		hours = (hours * 10) + *in_time - '0';

		++in_time;
	}

	switch (*in_time)
	{
		case ':':  military_time = FALSE;  break;
		case 0:    military_time = TRUE;  break;
		default:   military_time = TRUE;  break;
	}	

	if (military_time)
	{  /* Military time */
		if (use_military)
		{

		    /*                                                                     */
		    /*  SKIP BLANKS                                                        */
		    /*                                                                     */

			for ( ; *in_time == ' '; ++in_time)
				;

			strncpy (remainder, in_time, 500);
			remainder[499] = 0;

			if ((hours < 24) && (hours > 0))
				return hours * 60;

			minutes = hours % 100;
			hours = hours / 100;

			if (minutes > 59)
			{
				error_message = "minutes is greater than 59";
				return -1;
			}

			return (hours * 60) + minutes;

		}
		else
			return hours;
	}

    /*                                                                     */
    /*  CHECK FOR SEPARATOR                                                */
    /*                                                                     */

	if (*in_time == ':')
	{  /*  Look for minutes  */
		has_colon = TRUE;
		++in_time;

	/*                                                                 */
	/*  PARSE THE MINUTES                                              */
	/*                                                                 */

		if (!isdigit (*in_time))
		{
			sprintf (errbuf, "non-numeric character:  '%c'", *in_time);
			error_message = errbuf;
			return -1;
		}

		minutes = *in_time - '0';

		++in_time;

		if (isdigit (*in_time))
		{
			minutes = (minutes * 10) + (*in_time - '0');
			++in_time;
		}

		if (minutes > 59)
		{
			error_message = "minutes is grerater than 59";
			return -1;
		}

	}  /*  Look for minutes  */
	else
		minutes = 0;

    /*                                                                     */
    /*  AN EXTRA DIGIT AT THIS POINT IS BAD                                */
    /*                                                                     */

	if (isdigit (*in_time))
	{
		error_message = "Too many digits in time";
		return -1;
	}

    /*                                                                     */
    /*  Look for (and skip) seconds                                        */
    /*                                                                     */

	if (*in_time == ':')
	{  /*  We have seconds  */

		++in_time;

	/*                                                                 */
	/*  PARSE THE SECONDS                                              */
	/*                                                                 */

		if (!isdigit (*in_time))
		{
			sprintf (errbuf, "Unexpected extra digit:  %c", *in_time);
			error_message = errbuf;
			return -1;
		}

		++in_time;

		if (!isdigit (*in_time))
		{
			sprintf (errbuf, "Unexpected extra digit:  %c", *in_time);
			error_message = errbuf;
			return -1;
		}

		++in_time;

		if (isdigit (*in_time))
		{
			sprintf (errbuf, "Unexpected extra digit:  %c", *in_time);
			error_message = errbuf;
			return -1;
		}

	}  /*  We have seconds  */

    /*                                                                     */
    /*  SKIP BLANKS                                                        */
    /*                                                                     */

	for ( ; *in_time == ' '; ++in_time)
		;

	strncpy (remainder, in_time, 500);
	remainder[499] = 0;

	return (hours * 60) + minutes;

}

/***************************************************************************/
/* NAME                                                                    */
/*      c_time_2 - convert a time interval to internal format              */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This function will parse an input time and convert it into an      */
/*   internal form which is an integer expressing a number of minutes.     */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      int c_time_2 (char * in_time)                                      */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           in_time  (R) - String to be parsed                            */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           The time or:                                                  */
/*               -1 = invalid input                                        */
/*                0 = blank input                                          */
/*                                                                         */
/***************************************************************************/

int c_time_2 (const char * callers_time)
{
	return c_time_x(callers_time, TRUE);
}


/***************************************************************************/
/* NAME                                                                    */
/*      c_time - convert a time of day to internal format                  */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This function will parse an input time and convert it into an      */
/*   internal form which is an integer expressing a number of minutes.     */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      int c_time (char * in_time)                                        */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           in_time  (R) - String to be parsed                            */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           The time or:                                                  */
/*               -1 = invalid input                                        */
/*                0 = blank input                                          */
/*                                                                         */
/***************************************************************************/


int c_time (const char * callers_time)



{
    static char my_time[80];
    int minutes;

    strncpy (my_time, callers_time, 80);
    my_time[79] = 0;
    ztrim (my_time);
    in_time = my_time;

    /*                                                                     */
    /*  SKIP LEADING BLANKS                                                */
    /*                                                                     */

    for ( ; *in_time == ' '; ++in_time)
        ;

    /*                                                                     */
    /*  CHECK FOR NOON AND MIDNIGHT                                        */
    /*                                                                     */

    if (strncasecmp (in_time, "noon", 4) == 0)
    {
	    strncpy (remainder, &in_time[4], 500);
	    remainder[499] = 0;
	    return 720;
    }
    
    if (strncasecmp (in_time, "midnight", 8) == 0)
    {
	    strncpy (remainder, &in_time[8], 500);
	    remainder[499] = 0;
	    return 1440;
    }

    minutes = c_time_2 (callers_time);
    
    if (minutes <= 0)
        return minutes;

    if (minutes > 1440)
    {
	    error_message = "time value is too large";
	    return -1;
    }


    /*                                                                     */
    /*  CHECK FOR am OR pm                                                 */
    /*                                                                     */

    if  (strncasecmp (in_time, "pm", 2) == 0)
    {
        in_time += 2;  /* skip pm */

        if (minutes >=  780)
	{
		error_message = "time value is too large";
		return -1;
	}
        else if (minutes < 720)
            minutes += 720;
    }

    if (strncasecmp (in_time, "p.m.", 4) == 0)
    {
        in_time += 4;  /* skip p.m. */

        if (minutes >=  780)
	{
		error_message = "time value is too large";
		return -1;
	}
	else
            minutes += 720;

    }

    if (strncasecmp (in_time, "a.m.", 4) == 0)
    {
        in_time += 4;

        if ((minutes >= 720) && (minutes < 780))
            minutes -= 720;
        else if (minutes >=  780)
	{
		error_message = "time value is too large";
		return -1;
	}
    }

    if (strncasecmp (in_time, "am", 2) == 0)
    {
        in_time += 2;

        if ((minutes >= 720) && (minutes < 780))
            minutes -= 720;
        else if (minutes >=  780)
	{
		error_message = "time value is too large";
		return -1;
	}
    }

    /* Skip blanks */

    while (*in_time == ' ')
        ++in_time;

    /*  Check for time zone */
    /*  The current implementation assumes local time zone is pacific */

    if ((strncasecmp (in_time, "mdt", 3) == 0)
     || (strncasecmp (in_time, "mst", 3) == 0))
    {
	    in_time += 3;
	    minutes -= 60;
    }

    else if ((strncasecmp (in_time, "cdt", 3) == 0)
	  || (strncasecmp (in_time, "cst", 3) == 0))
    {
	    in_time += 3;
	    minutes -= 120;
    }

    else if ((strncasecmp (in_time, "edt", 3) == 0)
	  || (strncasecmp (in_time, "est", 3) == 0))
    {
	    in_time += 3;
	    minutes -= 180;
    }
	
    strncpy (remainder, in_time, 500);
    remainder[499] = 0;

    if (minutes < 0)
        minutes += (24 * 60);  /* assume previous day */

    return minutes;

}

/**********************************************************************
*
* FUNCTION: e_time
*
* DESCRIPTION:
*       Convert internal format time to external elapsed-time form
*
* CALLING SEQUENCE:
*       e_time (minutes, time_string)
*
* INPUT PARAMETERS:
*       minutes               Internal format time
*
*
* OUTPUT PARAMETERS:
*       time_string           String to contain output
*
* RETURNED VALUE:
*       None.
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

void e_time (int minutes, char *time_string)

{
	if (minutes < 0)
		strcpy (time_string, "??h??m");
	else if (minutes == 0)
		strcpy (time_string, "      ");
	else
		sprintf (time_string, "%02dh%02dm", minutes / 60, minutes % 60);
}

/**********************************************************************
*
* FUNCTION: e_timez
*
* DESCRIPTION:
*       Convert internal format time to external elapsed-time form
*       This differs from e_time in that this function returns 0h00m
*       for zero, whereas e_time returns blanks.
*
* CALLING SEQUENCE:
*       e_time (minutes, time_string)
*
* INPUT PARAMETERS:
*       minutes               Internal format time
*
*
* OUTPUT PARAMETERS:
*       time_string           String to contain output
*
* RETURNED VALUE:
*       None.
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

void e_timez (int minutes, char *time_string)

{
	if (minutes < 0)
		strcpy (time_string, "??h??m");
	else if (minutes == 0)
		strcpy (time_string, "00h00m");
	else
		sprintf (time_string, "%02dh%02dm", minutes / 60, minutes % 60);
}

/**********************************************************************
*
* FUNCTION: p_time
*
* DESCRIPTION:
*       Convert internal format time to external form
*
* CALLING SEQUENCE:
*       p_time (minutes, time_string)
*
* INPUT PARAMETERS:
*       minutes               Internal format time
*
*
* OUTPUT PARAMETERS:
*       time_string           String to contain output
*
* RETURNED VALUE:
*       None.
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

void p_time (int minutes, char *time_string)

{
	if (minutes < 0)
		strcpy (time_string, "??:??");
	else if (minutes == 0)
		strcpy (time_string, "     ");
	else
		sprintf (time_string, "%02d:%02d", minutes / 60, minutes % 60);
}

/**********************************************************************
*
* FUNCTION: p_timez
*
* DESCRIPTION:
*       Convert internal format time to external form
*       This differs from p_time in that this function returns 0:00
*       for zero, whereas p_time returns blanks.
*
* CALLING SEQUENCE:
*       p_time (minutes, time_string)
*
* INPUT PARAMETERS:
*       minutes               Internal format time
*
*
* OUTPUT PARAMETERS:
*       time_string           String to contain output
*
* RETURNED VALUE:
*       None.
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

void p_timez (int minutes, char *time_string)

{
	if (minutes < 0)
		strcpy (time_string, "??:??");
	else if (minutes == 0)
		strcpy (time_string, "00:00");
	else
		sprintf (time_string, "%02d:%02d", minutes / 60, minutes % 60);
}

/**********************************************************************
*
* FUNCTION: p_time12
*
* DESCRIPTION:
*       Convert internal format time to external form (12-hour clock)
*
* CALLING SEQUENCE:
*       p_time12 (minutes, time_string)
*
* INPUT PARAMETERS:
*       minutes               Internal format time
*
*
* OUTPUT PARAMETERS:
*       time_string           String to contain output
*
* RETURNED VALUE:
*       None.
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

void p_time12 (int minutes, char *time_string)

{
    if (minutes < 0)
        strcpy (time_string, "??:??");
    else if (minutes == 0)
        strcpy (time_string, "     ");
    else if (minutes < 60)
	sprintf (time_string, "12:%02d AM", minutes);
    else if (minutes == 720)
	    strcpy (time_string, "Noon");
    else if (minutes == 1440)
	    strcpy (time_string, "Midnight");
    else if (minutes < 720)
        sprintf (time_string, "%02d:%02d AM", minutes / 60, minutes % 60);
    else if (minutes < 780)
        sprintf (time_string, "12:%02d PM", minutes % 60);
    else
        sprintf (time_string, "%02d:%02d PM", (minutes / 60) - 12,
                 minutes % 60);
}

/***************************************************************************/
/* NAME                                                                    */
/*      c_time_3 - convert a time interval to internal format              */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This function will parse an input time and convert it into an      */
/*   internal form which is an integer expressing a number of minutes.     */
/*   This routine is the same as c_time_2 with the addition that it takes  */
/*   the forms nn hours or nn minutes as well                              */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      int c_time_3 (char * in_time)                                      */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           in_time  (R) - String to be parsed                            */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           The time or:                                                  */
/*               -1 = invalid input                                        */
/*                0 = blank input                                          */
/*                                                                         */
/***************************************************************************/


int c_time_3 (const char * callers_time)



{
    int i_time;

    i_time = c_time_x (callers_time, FALSE);

    if (i_time < 0)
        return i_time;

    if (has_colon)
        return i_time;

    /*                                                                     */
    /*  Skip leading blanks                                                */
    /*                                                                     */

    while (*in_time == ' ')
	    ++in_time;

    /*                                                                     */
    /*  Look for key words                                                 */
    /*                                                                     */

    if (strncmp (in_time, "hour", 4) == 0)
    {
	    in_time  += 5;

	    if (*in_time == 's')
		    ++in_time;

	    i_time *= 60;
    }

    if (strncmp (in_time, "minute", 6) == 0)
    {
	    in_time += 6;

	    if (*in_time == 's')
		    ++in_time;
    }

    while (*in_time == ' ')
	    ++in_time;

    strncpy (remainder, in_time, 500);
    remainder[499] = 0;

    return i_time;
}

/****************************************************************************

        NAME:       ctime_error

        Purpose:        To report an error

        Arguments:      None

        Processing:     Return error message

        Side Effects:   None

        Returns:        Most recent error message

****************************************************************************/

char* ctime_error (void)

{
	return error_message;
}  /*  ctime_error  */

/****************************************************************************

        NAME:       ctime_remainder

        Purpose:        To return any residue text after the time value

        Arguments:      None

        Processing:     Return a pointer

        Side Effects:   None

        Returns:        Pointer to a string; empty if no residue

****************************************************************************/

char*  ctime_remainder (void)

{
	return remainder;
}  /*  ctime_remainder  */

/****************************************************************************

        NAME:       ctime_help

        Purpose:        To return a help string

        Arguments:      None

        Processing:     Return a pointer

        Side Effects:   None

        Returns:        Pointer to a halp string

****************************************************************************/

char*  ctime_help (int time_of_day)

{
	char* text;

	if (time_of_day)
		text =  "The following time formats can be used:\n"
                        "\n"
                        "Style           Format         Example\n"
                        "=====           ======         =======\n"
                        "Military time   HHMM           1330\n"
                        "24-hour time    HH:MM          13:30\n"
                        "12-hour time    HH:MM a.m.     3:30 a.m.\n"
                        "12-hour time    HH:MM am       3:30 am\n"
                        "12-hour time    HH:MM p.m.     3:30 p.m.\n"
                        "12-hour time    HH:MM pm       3:30 pm\n"
                        "special         noon           noon\n"
                        "special         midnight       midnight\n"
                        "\n"
		  "All letters may be upper or lower case\n";
	else
		text =  "The following time formats can be used:\n"
			  "\n"
			  "Style           Format         Example\n"
			  "=====           ======         =======\n"
			  "Military time   HHMM           1330\n"
			  "24-hour time    HH:MM          13:30\n"
			  "hours           HH hours       3 hours\n"
			  "minutes         MM minutes     5 minutes\n";
	return text;
	
}  /*  ctime_help  */


VERSION_PROC (ctime_version)
