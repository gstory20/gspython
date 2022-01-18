/****************************************************************************

        PROGRAM DESCRIPTION

        NAME    CDATE.C

        AUTHOR  Glenn Story

        DATE WRITTN 21:07:31  7/6/1986

        SYSTEM  Designed for org programs - may be used by others

        DESCRIPTION This subroutine receives a date in one of several
        formats and returns the actual date in the form yy/mm/dd.

        The input formats may be:

        yy/mm/dd (literally) = today's date
        86/12/15 (for example) = the date as entered
        12/15/86
        12/15
        15
        12/86
        15-12-86
        15-Dec
        86-12-15
        15-12
        12-86
        Dec. 15, 1986
        15 Dec 1986
        Dec. 15, 86
        15 Dec. 86
        December 15, 1986
        15 December 1986
        December 15, 86
        15 December 86
        Dec 86
        today = today's date
        tomorrow = tomorrow's date
        week = today + 7 days
        xxx (day of week) = next occurance of that day
        month = today + 1 month
        year = today + 1 year
        x weeks = today + (7 * x) weeks (x single digit only)
        x days = today + x days
        x months = today + x months
        x workdays
        x business days (same as workdays)
        1st or first or 2nd or second or 3rd or third or 4th or fourth
           or last xxx (day of week)
        start or 00/00/00 = represents the earliest possible date
        end or 99/99/99 = represents the latest possible date
        Any other = ????????

        MODIFICATION HISTORY

         12/28/88 - Add more flexible input specifications and
            a "past" mode

         1/6/89 - Added new entry point cdate_2 (third arg added)

         1/20/89 - Added cdate_1

         2/1/89 - Fix bug with negative dates 

         2/19/90 - Allow only month to be specified

         9/7/90 - Allow European format date with hyphens

         11/1/90 - Handle missing day (month & year only)

         7/23/91 - If day only specified, and its past and future is
                   specified, then make it next month

         5/11/93 - Add "workdays"

         7/16/93 - Add many comments

         6/1/95 - Add relative day of month

         7/6/95 - Make max_day_of_month a separate function

         12/10/95 - Fix spelling of "fourth"

         6/19/96 - Allow "start" and "end" in future/past

         5/14/98 - Fix Y2K problems

         1/1/2000 - fix Y2K problem

         1/22/2005 - Add support for Excel date format dd-MMM

         2/14/2008 - Make relative date handling more rigorous and consistent:
	             * "today", "tomorrow" and "yesterday" are always relative
                       to today's date even if a different relative date is 
                       given in cdate_2
		     * When evaluating a date for correctness with respect to
                       past/future the comparison is always relative to today's
                       date even if a different relative date is given in 
                       cdate_2
		     * If only a single number is given, it is taken as a day
                       of the month within the relative month and day (as
                       before) but it will not be adjusted relative to past
                       and future, allowing for the possibility that the date
                       will be flaged as an error if past or future is 
                       requested.

	5/10/9 - Add "year" as a valid input

	10/15/10 - Add get_cdate (for python)

	10/13/13 - C99 Compatibility

	1/23/17 - Make work days different than business days
	
 ****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

#ifdef ANSI
#include <ctype.h>
#endif

/****************************************************************************

        EQUATES

 ****************************************************************************/

/*  Define DEBUG to create the test main routine */

#define VERSION

/****************************************************************************

        MACRO DEFINITIONS

 ****************************************************************************/

/****************************************************************************

        STRUCTURES

 ****************************************************************************/

/****************************************************************************

        Global data

 ****************************************************************************/

static int alpha_month ();
static int got_today = FALSE;
static char today[21];
static char weekdays[7][4] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
static char months[12][4] = {"jan", "feb", "mar", "apr", "may", "jun",
       "jul", "aug", "sep", "oct", "nov", "dec"};
static char *next_char;

static char output_date[20];

static void skip_blanks (void);
static int scan_num (void);
static int alpha_month (void);

static long null_holiday = 0L;
static long *holidays = &null_holiday;

extern long jday ();

int cyear (int in_year);
/* To convert years properly */

/*^*/

/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           cdate

        Purpose:        To convert a date to standard form
                        relative to today's date

        Description:    This routine converts one of many date formats
                        to an absolute date of the form yy/mm/dd.
                        If a relative date is entered, it is converted
                        relative to today's date.

        Arguments:      1.  Date (input/output)
                        2.  'f' = date must be today or future
                            'p' = date must be today or past
                            'b' = date can be past or future

        Processing:     Convert the date based on a scan of the input format

        Returns:        0 = Todays' date was specified
                        1 = A numeric date was specified and returned unchanged
                        2 = The date was converted
                        3 = an error was detected in the input parameters

*****************************************************************************
*****************************************************************************
****************************************************************************/

int cdate (char *date, char past)

{
    return (cdate_2 (date, past, today));
}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           cdate_2

        Purpose:        To convert a date to standard form
                        relative to a specified date

        Description:    This routine converts one of many date formats
                        to an absolute date of the form yy/mm/dd.
                        If a relative date is entered, it is converted
                        relative to the third parameter

        Arguments:      1.  Date (input/output)
                        2.  'f' = date must be today or future
                            'p' = date must be today or past
                            'b' = date can be past or future
                        3.  Date for use with relative dates (must be in
                           the form yy/mm/dd)

        Processing:     Convert the date based on a scan of the input format

        Returns:        0 = Todays' date was specified
                        1 = A numeric date was specified and returned unchanged
                        2 = The date was converted
                        3 = an error was detected in the input parameters
                        4 = relative date is in invalid format

*****************************************************************************
*****************************************************************************
****************************************************************************/

int cdate_2 (char *date, char past, const char *in_relative)

{
     char relative[11];
     int work_weekday;
     char dummy[20];
     char *temp;
     long work_jul;
     int work_day;
     int work_month;
     int work_year;
     int first_num, second_num, third_num;
     int max_day;
     int loop_again;
     char input_date[11];
     int t_month;
     int t_day;
     int t_year;
     long t_jul;
     long today_jul;
     int t_weekday;
     int cardinal_day[5];
     int cardinal;
     int cardinal_weekday;
     int i;

    /*
     *  FIRST GET TODAY'S DATE
     */

     time_stamp (today);
     today[10] = 0;  /* truncate after date before time */

     /*
      * VALIDATE RELATIVE DATE
      */

    if (in_relative[2] == '/')
    {  /*  Two-digit year  */
        if ((!isdigit (in_relative[0]))
            ||  (!isdigit (in_relative[1]))
            ||  (!isdigit (in_relative[3]))
            ||  (!isdigit (in_relative[4]))
	    ||  (in_relative[5] != '/')
            ||  (!isdigit (in_relative[6]))
            ||  (!isdigit (in_relative[7])))
        {
            strcpy (date, "????????");
	    strcpy (output_date, date);
            return 4;
        }
        else
        {  /*  Reformat as 4-digit year  */
            t_year = atoi (in_relative);
            t_year = cyear (t_year);
            sprintf (relative, "%04d%s", t_year, &in_relative[2]);
        }  /*  Reformat as 4-digit year  */
    }  /*  Two-digit year  */
    else if (in_relative[4] == '/')
    {  /*  Four-digit year  */
        if ((!isdigit (in_relative[0]))
            ||  (!isdigit (in_relative[1]))
            ||  (!isdigit (in_relative[2]))
            ||  (!isdigit (in_relative[3]))
            ||  (!isdigit (in_relative[5]))
            ||  (!isdigit (in_relative[6]))
            ||  (in_relative[7] != '/')
            ||  (!isdigit (in_relative[8]))
            ||  (!isdigit (in_relative[9])))
        {
            strcpy (date, "????????");
	    strcpy (output_date, date);
            return 4;
        }
        else
            strcpy (relative, in_relative);
    }  /*  Four-digit year  */
    else
    {
        strcpy (date, "????????");
	strcpy (output_date, date);
        return 4;
    }

    /*
     *  CONVERT TODAY'S DATE TO JULIAN
     */

     sscanf (today, "%d/%d/%d", &t_year, &t_month, &t_day);
     t_year = cyear (t_year);
     today_jul = jday (t_day, t_month, t_year);

     /*  
      *  CONVERT RELATIVE DATE TO JULIAN
      */

     sscanf (relative, "%d/%d/%d", &t_year, &t_month, &t_day);
     t_year = cyear (t_year);
     t_jul = jday (t_day, t_month, t_year);
     t_weekday = weekday (t_jul, dummy);


     /*
      *  EXTRACT INDIVIDUAL FIELDS FROM RELATIVE DATE
      */

    /*
     *  NOW MAKE SURE INPUT IS ALL LOWER CASE
     */

    for (temp = date; *temp != 0; ++temp)
        *temp = (char) chlower (*temp);

    /*
     *  NOW BEGIN HANDLING EXPECTED FORMATS
     *
     *  FIRST - LITERAL FORMAT "yy/mm/dd"
     *  AND ALSO "today"
     */

    if ((strncmp (date, "yy/mm/dd", 8) == 0)
      || (strncmp (date, "today", 5) == 0))
    {
        strcpy (date, today);
	strcpy (output_date, date);
        return (0);
    }

    /*
     *  EARLIEST AND LATEST POSSIBLE DATES
     */

    if ((strncmp (date, "start", 5) == 0)
    ||  (strncmp (date, "00/00/00", 8) == 0))
    {
        if (past == 'f')
        {
            strcpy (date, "????????");
	    strcpy (output_date, date);
            return 3;
        }
        else
        {
            strcpy (date, "00/00/00");
	    strcpy (output_date, date);
            return 1;
        }
    }
    
    if ((strncmp (date, "end", 3) == 0)
    ||  (strncmp (date, "99/99/99", 8) == 0))
    {
        if (past == 'p')
        {
            strcpy (date, "????????");
	    strcpy (output_date, date);
            return 3;
        }
        else
        {
            strcpy (date, "99/99/99");
	    strcpy (output_date, date);
            return 1;
        }
    }

    /*
     * Look for cardinal dates
     */

    cardinal = 0;

    if ((strncmp (date, "1st ", 4) == 0) || (strncmp (date, "first ", 6) == 0))
        cardinal = 1;
    else if ((strncmp (date, "2nd ", 4) == 0)
             || (strncmp (date, "second ", 7) == 0))
        cardinal = 2;
    else if ((strncmp (date, "3rd ", 4) == 0)
             || (strncmp (date, "third ", 6) == 0))
        cardinal = 3;
    else if ((strncmp (date, "4th ", 4) == 0)
             || (strncmp (date, "fourth ", 6) == 0))
        cardinal = 4;
    else if (strncmp (date, "last ", 5) == 0)
        cardinal = 5;

    if (cardinal != 0)
    {   /* Have a cardinal date */

        /*
         * Skip to second word
         */

        for (next_char = date; *next_char != ' '; ++next_char)
            ;

        skip_blanks ();

        /*
         * Get the day of the week
         */

        cardinal_weekday = -1;
        for (work_weekday = 0; work_weekday < 7; ++work_weekday)
            if (strncmp (next_char, weekdays[work_weekday], 3) == 0)
                cardinal_weekday = work_weekday;

        if (cardinal_weekday == -1)
            goto error_out;


        /*
         * Now loop through the month finding the days
         */

        max_day = max_day_of_month (t_month, t_year);

        for (work_day = 1, i = 0; work_day <= max_day; ++work_day)
        {   /* Loop through the month */
            work_jul = jday (work_day, t_month, t_year);
            work_weekday = weekday (work_jul, dummy);

            if (work_weekday == cardinal_weekday)
            {   /* Found matching day of week */
                cardinal_day[i++] = work_day; /* nth day */
                cardinal_day[4]   = work_day; /* last day */
            }   /* Found matching day of week */

        }   /* Loop through the month */

        work_day = cardinal_day[cardinal-1];
        work_month = t_month;
        work_year = t_year;
        goto work_return;
    }   /* Have a cardinal date */

    /*
     *  DATES BEGINNING WITH NUMBERS
     */

    next_char = date;
    first_num = scan_num ();
    if (first_num)
   {
      switch (*next_char)
      {
         /*
          *  Handle dates of the forms:
          *  mm/dd
          *  mm/yy
          *  mm/dd/yy
          *  dd/mm/yy  (only for days > 12; else ambiguous
          *  yy/mm/dd
          */
         case '/':   ++next_char;
                     second_num = scan_num ();
                     if (!second_num) /* If there is no number after the '/' */
                        goto error_out; /* Then it's an error */

                     if (*next_char == '/') /*If there's a third number? */
                     {
                        ++next_char;

                        if (!isdigit (*next_char)) /*If there's no number after the / */
                           goto error_out; /* Then it's an error */

                        third_num = scan_num ();
                     } /* if */
                     else
                        third_num = -1; /* Indicate no third number */

                     work_day = work_month = work_year = 0;

                     if (first_num > 31) /* If the first number is too large */
                                         /* to be a month or day */
                        work_year = first_num; /* Then it must be a year */
                     else if (first_num > 12) /* If the first number is too */
                                              /* large to be a month */
                        work_day = first_num; /* Then it must be a day*/
                     else
                        work_month = first_num; /* otherwise it's a month */

                     if (second_num > 31) /* If the second number is too */
                                          /* large to be a month */
                     {
                        if (work_year == 0) /* and first_num wasn't a year */
                           work_year = second_num; /* then second_num is */
                        else
                           goto error_out; /* two years in date ?? */
                     } /* if */
                     else if (second_num > 12) /* if the second number is
                                                  too large to be a month */
                     {
                        if (work_day == 0) /* If first_num wasn't a day */
                           work_day = second_num; /* then second_num is */
                        else
                           goto error_out; /*  Two months ?? */
                     } /* else */
                     else
                        if (work_month == 0) /* If first_num wasn't a month */
                           work_month = second_num; /* then second_num is */
                        else if (work_day == 0) /* if first_num wasn't a day */
                           work_day = second_num; /* then second_num is */
                        else
                           work_year = second_num; /* it must be the year */

                     if (work_month == 0) /* If we don't have a month yet */
                     {
                        if (third_num < 13) /*And third_num could be a month */
                           work_month = third_num; /* then make it so */
                        else
                           goto error_out; /* Don't have a month */
                     } /* if  */
                     else if (work_day == 0) /*If we don't have a day yet */
                     {
                        if (third_num == -1) /* and we dont' have 3 numbers */
                            work_day = 1;    /* then day defaults to 1 */
                        else if (third_num < 32) /* if it could be a day */
                           work_day = third_num; /* make it so */
                        else
                           goto error_out; /* don't know what third_num is */
                     } /* else */
                     else
                        work_year = third_num; /* third_num must be a year */

		     if (work_year == -1) /* if year not supplied */
		     {
			     work_year = t_year; /* then default is current year */

			     switch (past)
			     {
				     case 'f':
					     if (work_month < t_month)
						     work_year++;
					     else if ((work_month == t_month)
						     && (work_day < t_day))
						     work_year++;
					     break;

				     case 'p':
					     if (work_month > t_month)
						     work_year--;
					     else if ((work_month == t_month)
						     && (work_day > t_day))
						     work_year--;
					     break;
			     }
		     }
		     
                     work_year = cyear (work_year);

work_return:
                     strncpy (input_date, date, 10); /* save user's date */

                     sprintf (date, "%04d/%02d/%02d", work_year, work_month,
                         work_day); /* format date from components */

                     switch (past)
                     {
                        case 'p':   if (strcmp (date, today) > 0)
                                    {
past_error:
                                        /*
                                         *  User only wanted dates less than
                                         *  or equal to relative date
                                         */

                                        strcpy (date, "?future?");
					strcpy (output_date, date);
                                        return (3);
                                    }
                                    break;

                        case 'f':   if (strcmp (date, today) < 0)
                                    {
future_error:
                                        /*
                                         *  User only wanted dates greater
                                         *  or eaual to relative date
                                         */

                                        strcpy (date, "??past??");
					strcpy (output_date, date);
                                        return (3);
                                    }
                                    break;

                        case 'b':   break; /* All dates OK */

                        default:    goto error_out; /* Invalid past value*/

                     } /* switch */

                     /*
                      *  If we are returning the same characters the user
                      *  passed in, return 1, otherwise return 2
                      */

		     strcpy (output_date, date);
                     return ((strncmp (date, input_date, 10) == 0) ? 1 : 2);

         /*
          *  Handle dates of the forms:
          *  mm-dd
          *  mm-yy
          *  dd-mm-yy
	  *  dd-MMM
          *  mm-dd-yy (only for days > 12; else ambiguous
          *  yy-mm-dd
          */
         case '-':   ++next_char;
                     second_num = scan_num ();
                     if (second_num) /* If there is a number after the '-' */
		     {
		       if (*next_char == '-') /*If there's a third number? */
			 {
			   ++next_char;

			   if (!isdigit (*next_char)) /*If there's no number after the / */
			     goto error_out; /* Then it's an error */

			   third_num = scan_num ();
			 } /* if */
		       else
			 third_num = -1; /* Indicate no third number */

		       work_day = work_month = work_year = 0;

		       if (first_num > 31) /* If the first number is too large */
			                  /* to be a month or day */
			 work_year = first_num; /* Then it must be a year */
		       else if (second_num > 12) /* If the 2nd number is too */
			                         /* large to be a month */
			 work_month = first_num; /* Then first_num is month */
		       else
			 work_day = first_num; /* else first_num is day */

		       if (second_num > 31) /* If the second number is too */
                                          /* large to be a month */
			 {
			   if (work_year == 0) /* and first_num wasn't a year */
			     work_year = second_num; /* then second_num is */
			   else
			     goto error_out; /* two years in date ?? */
			 } /* if */
		       else if (second_num > 12) /* if the second number is
						    too large to be a month */
			 {
			   if (work_day == 0) /* If first_num wasn't a day */
			     work_day = second_num; /* then second_num is */
			   else
			     goto error_out; /*  Two months ?? */
			 } /* else */
		       else
			 if (work_month == 0) /* if first_num wasn't a month */
                           work_month = second_num; /* then second_num is */
			 else if (work_day == 0) /* if first_num wasn't a day */
                           work_day = second_num; /* then second_num is */
			 else
                           work_year = second_num; /* it must be the year */

		       if (work_month == 0) /* If we don't have a month yet */
			 {
			   if (third_num < 13) /*And third_num could be a month */
			     work_month = third_num; /* then make it so */
			   else
			     goto error_out; /* Don't have a month */
			 } /* if  */
		       else if (work_day == 0) /*If we don't have a day yet */
			 {
			   if (third_num == -1) /* and we dont' have 3 numbers */
                             work_day = 1;    /* then day defaults to 1 */
			   else if (third_num < 32) /* if it could be a day */
			     work_day = third_num; /* make it so */
			   else
			     goto error_out; /* don't know what third_num is */
			 } /* else */
		       else
			 work_year = third_num; /* third_num must be a year */

		       if (work_year == -1) /* if year not supplied */
                         work_year = t_year; /* then default is current year */

		       work_year = cyear (work_year);

		       goto work_return;
		     }
		     /*  else see if we have an alpha month -- fall through */

         /*
          *  Handle these cases:
          *  nn mmm [yy[yy]]
          *  nn days
          *  nn weeks
          *  nn months
          *  nn years
          *  nn work days
          *  nn business days (same as work days)
          *
          *  Some of these (days, weeks, work days) are computed based on
          *  Julian dates, while others (months, years) are computed based
          *  on Gregorian dates.
          */

         case ' ':   skip_blanks ();
                     work_year = t_year;
                     work_month = t_month;
                     work_day = t_day;

                     /*
                      * nn weeks
                      */

                     if (strncmp (next_char, "week", 4) == 0)
                     {
                        work_jul = t_jul +  (first_num * 7);
                        goto jul_return;
                     }

                     /*
                      * nn months
                      */

                     else if (strncmp (next_char, "month", 5) == 0)
                        work_month += first_num;

                     /*
                      * nn years
                      */

                     else if (strncmp (next_char, "year", 4) == 0)
                        work_year += first_num;

                     /*
                      * nn days
                      */

                     else if (strncmp (next_char, "day", 3) == 0)
                     {
                        work_jul = t_jul + first_num;
                        goto jul_return;
                     }

                     /*
                      * nn work days
                      * nn business days
                      */

                     else if ((strncmp (next_char, "work", 4) == 0)
                          ||  (strncmp (next_char, "business", 8) == 0))
                     {  /* Handle work days */
			 int want_work_day = \
			    (strncmp (next_char, "work", 4) == 0);

                         work_jul = t_jul;
			 
                         /*
                          * We have to handle work days one at a time
                          * looking for weekends and holidays.  Holidays are
                          * kept in an array of Julian dates supplied by the
                          * caller
                          */

                         while (first_num != 0)
                         {
                             if (first_num < 0) /* If nn is negative */
                                 --work_jul; /* Then decrement Julian date */
                             else
                                 ++work_jul; /* Else increment Julian date */

                             /*
                              * If this is a work/business day then
                              * increment/decrement our loop counter
                              */

                             if (isa_holiday (work_jul, want_work_day) == 0)
			     {
                                 if (first_num < 0)
				 {
                                     ++first_num;
				 }
				 else
				 {
                                     --first_num;
				 }
			     }
			 }
                         goto jul_return;

                     }  /* Handle work days */
                     else

                     /*
                      *  Look for nn mmm [yy[yy]]
                      */

                     {
                        if ((work_month = alpha_month ()) > 12)
                        {  /* Month is invalid */

                            /*
                             *  Handle the nn case.  Treat nn as a day in
                             *  the current month.
                             */

                            work_month = t_month; /* Current month */
                            work_day = first_num; /* Specified day */
                        }  /* Month is invalid */

                        else
                        {  /* Month is valid */
                            work_day = first_num;
                            skip_blanks ();

							if (*next_char == '-')
								++next_char;

                            if (*next_char != 0)
                            {  /* Look for year */
                               if ((work_year = scan_num ()) == 0)
                                   work_year = t_year;
                            }  /* Look for year */
                            else
                               work_year = t_year; /* use current year */

                            work_year = cyear (work_year);
                        }  /* Month is valid */

                     } /* else */

/*                                                                        */
/*  NOW CLEAN UP THE COMPUTED DATE                                        */
/*                                                                        */

clean_work:
                     /*
                      *  The following loop will iterate while we have
                      *  months or days out of range.  This is necessary
                      *  because some of the Gregorian-based computations
                      *  may create invalid dates like February 30 (one
                      *  month past Janurary 30).
                      */

                     do
                     {
                        loop_again = FALSE;

                        while (work_month > 12) /* is month too large */
                        {
                           work_month -= 12;
                           ++work_year;
                           loop_again = TRUE;
                        } /* while */

                        while (work_month < 1) /* is month too small */
                        {
                           work_month += 12;
                           --work_year;
                           loop_again = TRUE;
                        } /* while */

                        max_day = max_day_of_month (work_month, work_year);

                        if (work_day > max_day) /* is day to large */
                        {
                           work_day -= max_day;
                           ++work_month;
                           loop_again = TRUE;
                        } /* if */
                     }
                     while (loop_again);

                     goto work_return;

         /*
          *  This is another case of nn by itself, yielding a day in the
          *  current month.
          *  month.
          */

         default:    work_day = first_num; /* Day is as specified by user */
                     work_month = t_month; /* Current month */
                     work_year = t_year; /* Current year */

		            sprintf (date, "%04d/%02d/%02d", work_year, work_month,
								work_day);

					 switch (past)
					 {
					 case 'p':
						 if (strcmp (date, today) > 0)
							 work_month--;
						 break;

					 case 'f':
						 if (strcmp (date, today) < 0)
							 work_month++;
						 break;
					 }


                     goto clean_work;
      } /* switch */

   } /* if */

    /*
     *  TOMORROW or DAY
     */


    if (strncmp (date, "tomorrow", 8) == 0)
    {
            work_jul = today_jul + 1;

jul_return:
            jdate (work_jul, &work_day, &work_month, &work_year);

            sprintf (date, "%04d/%02d/%02d", work_year, work_month,
                work_day);

            switch (past)
            {
               case 'p':   if (work_jul > today_jul)
                              goto past_error;
                           break;

               case 'f':   if (work_jul < today_jul)
                              goto future_error;
                           break;

               case 'b':   break;

               default:    goto error_out;

            } /* switch */

	    strcpy (output_date, date);
            return (2);
    }

    /*
     *  YESTERDAY
     */


    if (strncmp (date, "yesterday", 9) == 0)
    {
            work_jul = today_jul - 1;

            goto jul_return;
    }

    /*
     *  DAY
     */

    if (strncmp (date, "day", 3) == 0)
    {
	    work_jul = t_jul + 1;
	    goto jul_return;
    }

    /*
     *  WEEK
     */

    if (strncmp (date, "week", 4) == 0)
    {
	    work_jul = t_jul + 7;
	    goto jul_return;
    }

    /*
     *  MONTH
     */

    if (strncmp (date, "month", 5) == 0)
    {
        work_month = t_month;
        work_day = t_day;
        work_year = t_year;

        if (t_month == 12)       /* If we're already in December */
        {
            ++work_year;        /* Go to new year */
            work_month = 1;     /*January */
        }
        else
            ++work_month;       /* If not December, then bump the month */

        work_jul = jday (work_day, work_month, work_year);

        goto jul_return;
    }

    /*
     *  YEAR
     */

    if (strncmp (date, "year", 4) == 0)
    {
	    ++t_year;
	    work_jul = jday (t_day, t_month, t_year);
	    goto jul_return;
    }
    
    /*
     *  DAY OF WEEK
     */

    /*
     *  Loop through the table of valid weekday names until a match is found
     */

    for (work_weekday = 0; work_weekday < 7; ++work_weekday)
        if (strncmp (date, weekdays[work_weekday], 3) == 0)
        { /* A valid weekday was found */
            work_jul = work_weekday - t_weekday + t_jul;

            /*
             * work_jul now contains a date on the requested day of the
             * week.  It may however be in the wrong week (if the user asked
             * for only future or past dates).  For example if today is
             * Friday, and the user asked for Wednesday, then work_jul would
             * contain Wednesday of this week.  If the user also wanted
             * future dates, then we must adjust to next week's Wednesday.
             */

            if (past == 'p')
            {
               if (work_weekday >= t_weekday) /* If day in the future */
                  work_jul -= 7; /* Then adjust back one week */
            } /* if */
            else
            {
               if (work_weekday <= t_weekday) /* If the day is in the past */
                     work_jul += 7; /* Then adjust forward one week */
            }

            goto jul_return;
        } /* A valid weekday was found */

    /*
     *  mmm dd, yyyy or mmm yyyy
     */

   if ((work_month = alpha_month ()) <= 12)
   { /* A valid month name was found */
      skip_blanks ();

      if ((work_day = scan_num ()) == 0) /* If no day was found */
      {
            work_day = 1;       /* Set day to first of month */
            work_year = t_year; /* Use current year */
            goto work_return;
      }

      if (work_day > 31)        /* If it's not a valid day */
      {
            work_year = work_day; /* Then assume it's a year */
            work_day = 1;       /* And use the first day of the month */

            work_year = cyear (work_year);

            goto work_return;
      }


      if (*next_char == ',')    /* Skip a comma following the day */
         ++next_char;

      skip_blanks ();

      if ((work_year = scan_num ()) == 0) /* If a year was not specified */
         work_year = t_year;    /* then use the current year */

      work_year = cyear (work_year);

      goto clean_work;
   } /* if */

    /*
     *  ANYTHING ELSE IS INVALID
     */

error_out:
    strcpy (date, "????????");
    strcpy (output_date, date);
    return (3);

}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           alpha_month

        Purpose:        To compute a number month given an alpha month

        Description:    This function will convert jan -> 1, feb -> 2, etc.

        Arguments:      none

        Processing:     loop through the table of valid month abbreviations

        Returns:        numeric month value

*****************************************************************************
*****************************************************************************
****************************************************************************/

static int alpha_month ()

{
   int work_month;

   for (work_month = 0; work_month < 12; ++work_month)
      if (strncmp (next_char, months[work_month], 3) == 0)
         break;

   while ((*next_char != ' ') && (*next_char != 0) && (*next_char != '-'))
      ++next_char;

   return (work_month + 1);

}  /* alpha_month  */


/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           scan_num

        Purpose:        to scan a number

        Description:    This routine scans characters until a non-numeric
                        character is found and returns the result

        Arguments:      none

        Processing:     Loop through characters

        Returns:        the number scanned

*****************************************************************************
*****************************************************************************
****************************************************************************/

static int scan_num ()

{
      int number = 0;
      int sign = 1;

      if (*next_char == '+')
         ++next_char;
      else if (*next_char == '-')
      {
         sign = -1;
         ++next_char;
      } /* else */

      while (isdigit (*next_char))
      {
         number *= 10;
         number += *next_char - '0';
         ++next_char;
      } /* while */

      return (number * sign);
}  /* scan_num  */


/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           skip_blanks

        Purpose:        To skip over blanks in the input

        Description:    This routine skips over blanks in the input string

        Arguments:      none

        Processing:     loop until a non-blank is found;

        Returns:        nothing

*****************************************************************************
*****************************************************************************
****************************************************************************/

static void skip_blanks ()

{
      while (*next_char == ' ')
         ++next_char;
}  /* skip_blanks  */



/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           cdate_1

        Purpose:        To process a date with fixed and relative parts

        Description:    This function returns a date based on an input
                        date.  The input can be either the same format
                        as for cdate (which is then called to process it)
                        or yy/mm/dd +/-relative date

        Arguments:      1.  Date
                        2.  'p', 'f', or 'b'

        Processing:     1.  If yy/mm/dd +/- relative, call cdate_2
                        2.  Otherwise call cdate

        Returns:        (Same as cdate and cdate_2)

*****************************************************************************
*****************************************************************************
****************************************************************************/

int cdate_1 (char *date, char past)

{
    char *relative;
    int rc;

      /*
       *  Check for a valid base date in the form yy/mm/dd
       */

      if (isdigit (date[0]))
      {
        if (isdigit (date[1]))
	{
            if (date[2] == '/')
            {  /*  Two-digit year  */
                if (isdigit (date[3]))
		{
                    if (isdigit (date[4]))
		    {
                        if (date[5] == '/')
			{
                            if (isdigit (date[6]))
			    {
                                if (isdigit (date[7]))
				{
                                    if (date[8] == ' ')
                                    {
                                        next_char = &date[9];
                                        skip_blanks ();
                                        if (next_char[0] == '+')
                                        {
                                            /*
                                             *  A valid date was found so
                                             *  use it with cdate_2
                                             */

                                            date[8] = 0;
                                            relative = &next_char[1];
                                            rc = cdate_2 (relative, past,date);
                                            strcpy (date, relative);
                                            return (rc);
                                        } /* if */
                                        else if (next_char[0] == '-')
                                        {
                                            /*
                                             * A valid date was found and a
                                             * negative relative date was found
                                             */
                                            date[8] = 0;
                                            relative = next_char;
                                            rc = cdate_2 (relative,past,date);
                                            strcpy (date, relative);
                                            return (rc);
                                        } /* if */
                                    } /* if */
				}
			    }
			}
		    }
		}
            }  /*  Two-digit year  */
            else
	    {
              if (isdigit (date[2]))
	      {
                if (isdigit (date[3]))
		{
                  if (date[4] == '/')
		  {
	            if (isdigit (date[5]))
		    {
                      if (isdigit (date[6]))
		      {
                        if (date[7] == '/')
			{
                            if (isdigit (date[8]))
			    {
                                if (isdigit (date[9]))
				{
                                    if (date[10] == ' ')
                                    {
                                        next_char = &date[10];
                                        skip_blanks ();
                                        if (next_char[0] == '+')
                                        {
                                            /*
                                             *  A valid date was found so
                                             *  use it with cdate_2
                                             */

                                            date[10] = 0;
                                            relative = &next_char[1];
                                            rc = cdate_2 (relative, past,date);
                                            strcpy (date, relative);
                                            return (rc);
                                        } /* if */
                                        else if (next_char[0] == '-')
                                        {
                                            /*
                                             * A valid date was found and a
                                             * negative relative date was found
                                             */
                                            date[10] = 0;
                                            relative = next_char;
                                            rc = cdate_2 (relative,past,date);
                                            strcpy (date, relative);
                                            return (rc);
                                        } /* if */
                                    } /* if */
				}
			    }
			}
		      }
		    }
		  }
		}
	      }
	    }
	}
      }

      return (cdate (date, past)); /* No base date so handle same as cdate */
}  /* cdate_1  */


/**********************************************************************
*
* FUNCTION: set_holidays
*
* DESCRIPTION:
*       This function will accept an array of Julian dates and save its
*       location for use with the "nn workdays" option
*
* CALLING SEQUENCE:
*       set_holidays (holidays)
*
* INPUT PARAMETERS:
*       holidays              array of longs contains julian dates
*
*
* OUTPUT PARAMETERS:
*       none
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

void set_holidays (long *my_holidays)

{
    holidays = my_holidays;
}

/****************************************************************************

        NAME:       max_day_of_month

        Purpose:    To return the last day of a specified month

        Arguments:  1.  month (input)
                    2.  year (input)

        Processing: Use the nursery r

        Returns:    The day of the month (or zero if month is out of range)

****************************************************************************/

int max_day_of_month (int month, int year)

{
    int max_day;

    if ((month < 1) || (month > 12))
        return 0;

    switch (month) /* compute days in month */
    {
    case 9:     /*  30 days hath September         */
    case 4:     /*  April,                         */
    case 6:     /*  June, and                      */
    case 11:    /*  November                       */
        max_day = 30;
        break;

    default: max_day = 31;  /*  All the rest have 31 */
        break;

    case 2:  max_day = (year % 4) ? 28 : 29; /* except February where*/
                                                  /* the meter breaks down*/
                                                  /* entirely */
        break;

    } /* switch */

    return max_day;

}  /*  max_day_of_month  */

/****************************************************************************

        NAME:       isa_holiday

        Purpose:    To determine if a day is in the list of holidays

        Arguments:  1.  julian (in):  date to be checked
		    2.  want_work_day:  TRUE = want work day (Roku
            		                definition)
            		                FALSE = want a businness day
        Processing: 1.  See if its a Sunday
                    2.  Loop through holidays

        Returns:    0 = workday
                    1 = Saturday
                    2 = Sunday
                    3 = Holiday
		    4 = day off (only with want_work_day)
		    
****************************************************************************/

int isa_holiday (long julian, int want_work_day)
{
    int work_weekday;
    char dummy[20];
    long *holiday;

    work_weekday = weekday (julian, dummy);

    if (work_weekday == 0)
        return 2;

    if (work_weekday == 6)
        return 1;

    holiday = holidays;

    /*
     *  Search for the current date to the array of
     *  holidays
     */

    while (*holiday != 0L)
    {  /* Loop through holidays */
        if (*holiday == julian)
            return 3;
        ++holiday;
    }  /* Loop through holidays */

    if (want_work_day)
	    if ((work_weekday == 1) || (work_weekday == 4))
	    {
		    return 0;
	    }
	    else
	    {
		    return 4;
	    }
    return 0;
    
}  /*  isa_holiday  */

#ifdef DEBUG

main ()

{
    char my_date[80];
    char my_past[80];
    char relative_date[80];

    int rc1;
    int rc2;

    while (TRUE)
   {
      printf ("Enter date:  ");
      fgets (my_date, 80, stdin);
      printf ("Enter 'p' or 'f':  ");
      fgets (my_past, 80, stdin);
      printf ("enter relative date:  ");
      fgets (relative_date, 80, stdin);

      rc2 = cdate_2 (my_date, (char) my_past[0], relative_date);

      printf ("date = `%s', return code = %d\n",
        my_date,  rc2);
   } /* while */

}

#endif

/****************************************************************************

        NAME:       cyear

        Purpose:    To convert years properly

        Arguments:  1.  in_year (IN):  year to convert

        Processing: 1.  If year is > 1900 return as is.
                    2.  If year is < 50, assume 21st century.
                    3.  Otherwise assume 20th century.

        Returns:    Full year including century

****************************************************************************/

int cyear (int in_year)

{
    if (in_year > 1900)
        return in_year;

    if (in_year < 50)
        return in_year + 2000;

    return in_year + 1900;
}  /*  cyear  */

char* get_cdate(void)
{
	return output_date;
}


VERSION_PROC (cdate_version)
