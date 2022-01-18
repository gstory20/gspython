/****************************************************************************

        PROGRAM DESCRIPTION

        NAME            /home/glenns/gs/src/gethostname.c

        AUTHOR          Glenn Story

        DATE WRITTEN    07/09/2009 10:19:52

        DESCRIPTION     Function to get a host name

        MODIFICATION HISTORY

        {@change_history...@}

 ****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

#include "gs.h"

/****************************************************************************

        EQUATES

 ****************************************************************************/

/****************************************************************************

        MACRO DEFINITIONS

 ****************************************************************************/

/****************************************************************************

        STRUCTURES

 ****************************************************************************/

/****************************************************************************

        TYPEDEFS

 ****************************************************************************/

/****************************************************************************

        Global data

 ****************************************************************************/



/****************************************************************************

        Routines defined in other modules

 ****************************************************************************/

/****************************************************************************

        Routines defined in this module

 ****************************************************************************/


char* get_hostname (void);
/* To return a string with the host name */

/*^*/

/****************************************************************************

        NAME:       get_hostname

        Purpose:        To return a string with the host name

        Arguments:      None.

        Processing:     1.  Look for HOSTNAME environment variable
                        2.  Look for COMPUTERNAME environment variable
                        3.  Read contents of /etc/hostname
                        4.  If all else fails return an empty string

        Side Effects:   None.

        Returns:        Returns the host name.  If not found returns an
                        empty string.  (Never returns NULL)

****************************************************************************/

char* get_hostname (void)

{
	char* host;
	FILE* hostname;
	
#define HOSTNAMESIZE 255
	static char buffer[HOSTNAMESIZE + 1];
	
	host = getenv ("HOSTNAME");

	if (host == NULL)
		host = getenv ("COMPUTERNAME");

#ifdef UNIX
	if (host == NULL)
	{
		if (gethostname (buffer, HOSTNAMESIZE) == 0)
			host = buffer;
	}
#endif
	
	if (host == NULL)
		host = "";

	return host;
}  /*  get_hostname  */

