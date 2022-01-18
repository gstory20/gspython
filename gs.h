#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "gslib.h"
#include "gsvers.h"
#include "getachar.h"

#define HELP_WANTED ((strcmp (argv[1], "?") == 0) \
		     || (strcmp (argv[1], "-h") == 0) \
		     || (strcmp (argv[1], "--help") == 0))

#define CHECK_WANTED ((strcmp (argv[1], "-c") == 0) \
		     || (strcmp (argv[1], "--check") == 0))


#define MAXPATH FILENAME_MAX /* 4096 */ /* MAX_PATH = 256 in windows */
#define TRUE 1
#define FALSE 0
#define ERROR -1
#define console(a,b,c,d) /* console function is null */
#define getakey getachar
#define GOOD_RETURN 0
#define ERROR_RETURN 1
#define UNIX
#define MAC
/* #define NO_EXIT() 0 */
#define NO_EXIT()
#define NOT_USED(x) x=0
#define ANSI
#define blanks(s,i) s[0] = 0;  rpad (s, i)
#define mvc(count,to,from) memcpy (to, from, count)
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define bool int
#define true TRUE
#define false FALSE
#define stricmp strcasecmp
#define INTEL_ENDIAN

FILE* open_printer ();
void close_printer (FILE* file);
