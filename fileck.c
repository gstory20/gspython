/*  FILECK.C

    Glenn Story - 2/26/84

    This program checks to see if a specified file exists.

*/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

#include <fcntl.h>
#ifdef IBM
#include <io.h>
#endif

int file_exists (const char *file_name)

{
        int fd;

        if ((fd = open (file_name, O_RDONLY)) == ERROR)
	{
		return (FALSE);
	}
        else
        {
                close (fd);
                return (TRUE);
        }
        NO_EXIT ();
}

int file_writable (const char *file_name)

{
        FILE *fd;

        if ((fd = fopen (file_name, "r+")) == (FILE *) 0)
                return (!file_exists (file_name));
        else
        {
                fclose (fd);
                return (TRUE);
        }
        NO_EXIT ();
}
