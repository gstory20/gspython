%module _getfile
   %include typemaps.i
   %{
#include "gslib.h"
	%}

   int py_getfile (const char *program, const char *file, char direction,
		const char *mask, char *initial_value);

   char* getfile_result();
   
		   