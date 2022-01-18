%module _cvttime
   %include typemaps.i
   %{
#include "ctime.h"
	%}

   int c_time (const char* in_time);
   int c_time_3 (const char* in_time);
   char* ctime_error(void);
   char* ctime_remainder(void);
   char* ctime_help(int time_of_day);