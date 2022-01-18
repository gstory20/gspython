
/*------ weekday: return the day of the week.
*
*        given a true julian date, return the day of the
*        week as a string and as a number (0 = sunday).
*
*        usage:
*           daynum = weekday(jday,daystr)
*
*        parameters:
*           daynum <- number  0=sunday,
*                             1=monday,etc.
*           jday   -> long int julian day number
*           daystr <-  day name
*
*        pgm: s. stern, jmb realty corp
*             converted from FLIB to C 6/85
*
*-------------------------------------------------------------------*/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"








      int weekday(jday,daystr)
      long jday;
      char *daystr;

      {
      long int ldaynum;
      int daynum;
      static char *dayname[] = {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday" };

     long sunday = 2445127L;
     long daydiff;
     int reverse = 0;
/*
*        get the day of the week rel to sunday
*/
      daydiff = jday - sunday;
      if (daydiff < 0) reverse = 7;
      ldaynum = daydiff % 7;
      daynum = (int) ldaynum;
      if (daynum != 0) daynum += reverse;
/*
*        get the string description
*/
      strcpy(daystr,dayname[daynum]);
      return(daynum);
}
