%module _cdate
   %include typemaps.i

   %{
#include "cdate.h"
	%}

   int  cdate (char *date, char past);
/*  date handling:  Convert date relative to to today                      */

int  cdate_1 (char *date, char past);
/*  Date handling:  Covert a [fixed +] relative date                       */

int  cdate_2 (char *date, char past, const char *relative);
/*  Date handling:  Convert date relative to the specified date            */

char *get_cdate(void);
