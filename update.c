/**********************************************************************
*
* MODULE: update.c
*
* BRIEF DESCRIPTION:
*       Check if a file is updatable
*
* MODULE TYPE:
*       Source
*
* FULL DESCRIPTION:
*       This program contains the function update_check which verifies that
*       a file is writable.  If it is not it goes through some
*       system-dependent error recovery.  This is the UNIX and IBM version of
*       this file.
*
* FUNCTIONS DEFINED:
*       release_lock2                   Release the file lock
*                                       {@description...@}
*       {@functions_defined...@}
*
* GLOBAL DATA DEFINED:
*       {@global_data_defined...@}
*
* REVISION HISTORY:
*       24Mar93      G. Story        Original release
*       13Oct13      G. Story        C99 compatibility
*       {@revision_history...@}
*
**********************************************************************/

/* Global Include Files */

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

#ifdef UNIX
#include <signal.h>
#include <sys/signal.h>
#endif

/* Subsystem Global Include Files */


/* Local Include Files */


/* Local Defines */

/* Global Data Referenced */

static void* global_cookie = NULL;
static char* saved_command_line = NULL;

struct lock_struct
{
	struct lock_struct* next;
	FILE *file_lock;
	char lock_filename[180];
	int locked;
};

static struct lock_struct* locks = NULL;

/* External Functions Referenced */

/****************************************************************************

        NAME:           unlock_all

        Purpose:        Only to be called by atexit;  unlocks all lock
			files

        Arguments:      None

        Processing:     Loop through the list of locks and unlock them

        Side Effects:   None.

        Returns:        nothing

****************************************************************************/

static void unlock_all(void)
{
	struct lock_struct* pLockStruct;

	for (pLockStruct = locks;
	     pLockStruct != NULL;
	     pLockStruct = pLockStruct->next)
	{
		release_lock2(pLockStruct);
	}
}

/****************************************************************************

        NAME:           save_command_line

        Purpose:        To save the command line for the lock file

        Arguments:      command_line:  (IN):  Command line to save

        Processing:     1.  duplicate the command line

        Side Effects:   command_line is saved.

        Returns:        Nothing

****************************************************************************/
void save_command_line(const char* command_line)
{
	if (saved_command_line != NULL)
		free (saved_command_line);

	saved_command_line = strdup(command_line);
}

/****************************************************************************

        NAME:           create_lock

        Purpose:        To create a lock file

        Arguments:      pLockStruct (IN/OUT):  Lock structure

        Processing:     1.  Open the file
                         2.  Write the process ID to it (if supported)

        Side Effects:   None.

        Returns:        TRUE if successful

****************************************************************************/

int create_lock (struct lock_struct* pLockStruct)
{
	char* host;

	host = get_hostname ();

    pLockStruct->file_lock = fopen (pLockStruct->lock_filename, "w");
    if (pLockStruct->file_lock == NULL)
    {
	    pLockStruct->locked = FALSE;
	    return FALSE;
    }

    fprintf (pLockStruct->file_lock, "%d %s\n", getpid (), host);

    if (saved_command_line != NULL)
	    fprintf(pLockStruct->file_lock, "%s\n", saved_command_line);
    
    fflush (pLockStruct->file_lock);

    pLockStruct->locked = TRUE;
    return TRUE;
}  /*  create_lock  */



/**********************************************************************
*
* FUNCTION: update_check2
*
* DESCRIPTION:
*       Check to see if a file is updatable.  If it is not, then recovery
*       action is taken.
*
* CALLING SEQUENCE:
*       update_cookie = update_check2 (filename)
*
* INPUT PARAMETERS:
*       filename              Name of file to check
*
*
* OUTPUT PARAMETERS:
*       None
*
* RETURNED VALUE:
*       update_cookie       TRUE if file is writable and not locked
*
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

void* update_check2 (const char *filename)

{
	int pid = 0;
	char host[80];
	char command[80];

    char answer;
    struct lock_struct* pLockStruct;

    char lock_filename[180];
    char my_filename[80];
    char *dot;
    int mypid = getpid ();
    char command_line[81];
    
    strcpy (my_filename, filename);
    dot = strrchr (my_filename, '.');

    if (dot)
        *dot = 0;

    sprintf (lock_filename, "%s.lck", my_filename);

    if (file_writable (filename))
    {  /* File is writable; check for locks */

	    /*  Create lock structure */
        pLockStruct
            = (struct lock_struct*) malloc (sizeof (struct lock_struct));
        if (pLockStruct == NULL)
            return NULL;

	pLockStruct->locked = FALSE;

	/*  Chain it to list of lock structures */
	pLockStruct->next = locks;
	locks = pLockStruct;

	/*  Add cleanup routine */
	atexit(unlock_all);

	/*  Save the lock filename */
        strcpy (pLockStruct->lock_filename, lock_filename);

	/*  Check to see if there is a lock file */
	if (file_exists (pLockStruct->lock_filename))
	{  /* Lock file already exists */
            while (TRUE)
            {
                FILE* lock_file = fopen (pLockStruct->lock_filename, "r");
                if (lock_file == NULL)
		{
                    return update_check2 (filename);
		}
		
                fscanf (lock_file, "%d %s", &pid, host);
		memset(command_line, 0, 81);
		fread (command_line, 1, 80, lock_file);
		fclose (lock_file);


#ifdef UNIX
		char*  host1;

		host1 = get_hostname ();

		if ((strcmp (host, host1) == 0)
		    &&  (pid != 0))
                {   /* See if process still exists */
                    char proc_file_name[512];

#ifdef MAC
		    sprintf (command, "ps -ef | grep %d | grep -v grep", pid);
		    if (system(command))
#else
                    sprintf (proc_file_name, "/proc/%d", pid);
		    if (!file_exists (proc_file_name))
#endif
                    {   /* Process is no longer in existence */
                        int i;

			if (isatty(0))
				erase ();
			
                        printf (
"\nFile %s was in use by process %d on this system (%s)\n",
				  filename, pid, host);
			printf ("%s", command_line);
			printf ("but that process no longer exists\n");
                        printf ("Data may have been lost\n");
			if (isatty(0))
			{
				printf ("Press any key to continue\n");
				printf ("\n");

				for (i = 20; i >0 ; i--)
				{   /* Loop for 20 seconds */
				    if (getachar_nowait () != 0)
					break;

				    printf ("Process will continue automatically in %d seconds \r", i);
				    fflush (stdout);
				    sleep (1);
				}   /* Loop for 20 seconds */

			}
			else
			{
				printf("\nstdin is not a tty\n");
				printf ("Current process continuing...\n");
			}

			create_lock (pLockStruct);
			return pLockStruct;
                    }   /* Process is no longer in existence */
                }   /* See if process still exists */
#endif

		/*  Now we process the situations where either (1) the
		 *  other process is still running, or (2) the other
		 *  process is on another host (so we can't tell if
		 *  it's running or not).
		 */
		
		if (isatty(0))
		{
			erase ();
			gotoxy (1, 4);
		}

		if ((strcmp (host, host1) == 0)
		    &&  (pid == mypid))
		{
			printf (
	"ERROR:  The current file is already locked by the current program\n");
			printf ("\n%s\n", filename);
			kill(mypid, SIGABRT);
			return NULL;  /*  in case SIGABRT is ignored */
			
		}
			
		printf ("WARNING:  This file is already locked by process %d on %s", pid, host);
		printf ("\n%s\n", filename);
		printf ("%s\n", command_line);
		if (isatty(0))
		{
			saya (12, 23, "What do you want to do?");
			saya (14, 23, "1 - Try to get access again");
			saya (15, 23,
			     "2 - Ignore the lock and access the file anyway");

			saya (16, 23,
			      "3 - Open the file for read-only access");

			saya (17, 23,
			      "4 - Display details about locking process");

			saya (18, 23, "5 - Exit from the program");
			saya (20, 23, "Enter your choice (1-5):");

			while (TRUE)
			{
				answer = getachar_nowait();
				if (answer == 0)
				{
				    if (
					  file_exists (
						pLockStruct->lock_filename
					)
				    )
				    {
					    sleep (1);
					    continue;
				    }
				    answer = '0';
				    break;
				}
				break;
			}
			
			erase ();

			switch (answer - '0')
			{
			case 1:
#ifdef UNIX
				/*  Check to see if there is still a lock file */
				if (file_exists (pLockStruct->lock_filename))
				{  /* Lock file already exists */
				    if (pid != 0)
				    {   /* See if process still exists */
					char proc_file_name[512];
					/*  /proc/pid will never exist on OS X
					 *   so I guess we'll just skip this
					 *   logic
					 */
					sprintf (proc_file_name, "/proc/%d",
						pid);

					if (file_exists (proc_file_name))
					{   /* Process still exists */
					    int answer2 = FALSE;
					    saya (5, 1,
						  "File is still locked.");
					    saya (7, 1,
				"Do you want to kill the previous process?");
					    getl (7, 43, &answer2);
					    rd ();

					    if (answer2)
					    {
						    kill (pid, SIGTERM);
						    sleep (2);
						    kill (pid, SIGKILL);
						    locks = pLockStruct->next;
						    free(pLockStruct);
						    return update_check2 (
							    filename
						    );
					    }
					}   /* Process still exists */
				    }   /* See if process still exists */
				}
#endif

			    /*  Fall through to case 0 */

			case 0:
			    if (!file_exists (pLockStruct->lock_filename))
			    {
				    create_lock (pLockStruct);
				    return pLockStruct;
			    }
			    break;

			case 2:
			    create_lock (pLockStruct);
			    return pLockStruct;

			case 3:
			    /* free (pLockStruct); */
			    return NULL;

			case 4:
			{
				char command[500];

#ifdef MAC
				sprintf (command, "macdpy %d", pid);
#else
				sprintf (command, "sysdpy %d", pid);
#endif
				system(command);
				break;
			}
			
			case 5:
			    exit (GOOD_RETURN);
			}
		}
		else
		{
			int i;

			printf("\nstdin is not a tty\n");
			printf("Watiing for file to unlock\n");
			fflush(stdout);

			/*  Wait for up to 10 minutes */
			for (i = 1; i < 600; i++)
			{
				if (!file_exists (pLockStruct->lock_filename))
				{
					create_lock (pLockStruct);
					return pLockStruct;
				}
				sleep (1);
			}
			printf("Still locked after 10 minutes\n");
			printf("Aborting current process\n");
			exit(ERROR_RETURN);
		}
            }
        }  /* Lock file already exists */
        else
        {
            create_lock (pLockStruct);
            return pLockStruct;
        }
    }  /* File is writable; check for locks */
    else
    {  /* File is not writable */
        erase ();
        printf ("You do not have write access to the data file.\n");
        printf ("File = %s\n", filename);
        printf ("\nPress any key to continue");
        getachar ();
        return NULL;
    }  /* File is not writable */
}


/**********************************************************************
*
* FUNCTION: update_check
*
* DESCRIPTION:
*       Check to see if a file is updatable.  If it is not, then recovery
*       action is taken.
*
* CALLING SEQUENCE:
*       update_ok = update_check (filename)
*
* INPUT PARAMETERS:
*       filename              Name of file to check
*
*
* OUTPUT PARAMETERS:
*       None
*
* RETURNED VALUE:
*       update_ok       TRUE if file is writable and not locked
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

int update_check (const char *filename)
{
    global_cookie = update_check2 (filename);
    return (global_cookie != NULL);
}

/**********************************************************************
*
* FUNCTION: release_lock2
*
* DESCRIPTION:
*       Release the file lock acquired when the file was opened
*
* CALLING SEQUENCE:
*       release_lock2 (lock_cookie)
*
* INPUT PARAMETERS:
*       lock_cookie (IN):  returned from update_check
*
*
* OUTPUT PARAMETERS:
*       None
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

void release_lock2 (void* cookie)

{
    struct lock_struct* pLockStruct = (struct lock_struct*) cookie;

    if (cookie == NULL)
         return;

    if (!pLockStruct->locked)
	    return;
    
    if (pLockStruct->file_lock != NULL)
    {
        fclose (pLockStruct->file_lock);

        if (unlink (pLockStruct->lock_filename) == ERROR)
			perror ("Unable to unlink lock file");
    }

    pLockStruct->locked = FALSE;
    
/*    free (pLockStruct); */
}


/**********************************************************************
*
* FUNCTION: release_lock
*
* DESCRIPTION:
*       Release the file lock acquired when the file was opened
*
* CALLING SEQUENCE:
*       release_lock ()
*
* INPUT PARAMETERS:
*       None
*
*
* OUTPUT PARAMETERS:
*       None
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

void release_lock (void)
{
    release_lock2 (global_cookie);
    global_cookie = NULL;
}
