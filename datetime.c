/*  DATETIME.C

    Glenn Story - 3/17/84

    This module contains two routines for accessing the system
    date and time.

    UNIX version

*/

#include "gs.h"
#include <time.h>

static struct tm *unix_time;

static void get_itime (void);

void get_a_date (char *date)

{
        static char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
        char month[4];

        get_itime ();

        strncpy (month, &months[(unix_time->tm_mon) * 3], 3);
        month[3] = 0;  /*  set null terminator */

        sprintf (date, "%s %d, %d", month,
                unix_time->tm_mday, unix_time->tm_year + 1900);
}

void get_n_date (char *date)

{
        get_itime ();

        sprintf (date, "%d/%d/%d", unix_time->tm_mon + 1, unix_time->tm_mday,
                unix_time->tm_year + 1900);
}

void get_time (char *time)

{
        get_itime ();

        sprintf (time, "%d:%02d:%02d", unix_time->tm_hour, unix_time->tm_min,
                unix_time->tm_sec);
}

void time_stamp (char *stamp)

{
        get_itime ();

        sprintf (stamp, "%d/%02d/%02d %02d:%02d:%02d",
                 unix_time->tm_year + 1900, unix_time->tm_mon + 1,
                 unix_time->tm_mday, unix_time->tm_hour, unix_time->tm_min,
                 unix_time->tm_sec);
}


static void get_itime (void)

{
    time_t bintime;

    bintime = time(0);
    unix_time = localtime (&bintime);
}
