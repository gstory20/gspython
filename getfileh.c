/****************************************************************************

        PROGRAM DESCRIPTION

        NAME    getfileh.c

        AUTHOR  Glenn Story

        DATE WRITTEN 2/10/92 (10/2/87)

        SYSTEM  Subroutine

        DESCRIPTION This function will get a file name for the caller
                    This is the high-level part of this interface

        MODIFICATION HISTORY

        7/25/88 -- Added ESC support

        5/8/91 -- ANSI calling sequences

        2/10/92 -- Split into high (user-interface) and low (file-system
                   interface) levels

        3/9/93 -- Unix version

        7/23/93 -- Fix spelling error

        1/27/94 -- Add type 'e' (exiting output)

        11/9/94 -- Call sd in unix version

        12/6/94 -- Build list to pick for input and output/existing files

        1/19/96 -- Fix directory-changing bug in Tandem version

        5/7/2000 -- Make sd work in Windows NT

        12/6/2008 -- Call sd.exe instead of sd

        11/30/2014 - Add getfile_result for python

	10/8/2018  - Don't allow blank filename

	5/31/2021  - Update interface to list_pick_ex

 ****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"
#include <fcntl.h>

#ifdef BC
#include <dir.h>
#endif

#ifdef IBM
#include <dos.h>
#include <direct.h>
#endif

#ifdef UNIX
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef box
#undef box
#endif

/****************************************************************************

        EQUATES

 ****************************************************************************/

#ifdef IBM
#define MAX_PATH 100
#endif

#ifdef UNIX
#define MAX_PATH 1024
#endif

/****************************************************************************

        MACRO DEFINITIONS

 ****************************************************************************/

/****************************************************************************

        STRUCTURES

 ****************************************************************************/

/****************************************************************************

        Global data

 ****************************************************************************/

static char save_response[512];

static void make_filename (char * target, char * source, char * path);
static void getfileg (const char *program, const char *file,
                      const char *direction, const char *mask, char* dir,
                      char *filename);

int retry = FALSE;

#ifdef IBM
#ifndef WIN32API
struct SREGS my_regs;
#endif
int saved_ds;
int saved_dx;
void set_dta ();        /*  Set DTA address for PC-DOS routines */
void reset_dta ();      /*  Reset DTA address after PC-DOS call */
#endif

/****************************************************************************

        NAME:  getfile_result

        Purpose:  To retun the selected filename (mainly for python)

        Arguments:  none

        Processing:  Return the static vaue

****************************************************************************/

char* getfile_result(void)
{
    return save_response;
}

/****************************************************************************

        NAME:  py_getfile

        Purpose:  To get the name of a file

        Arguments:  1.  Program name
                    2.  File description
                    3.  "i" or "o" or "w" or "l" or "v" or "?" or "e" or "a"
                        "i" = input file
                        "o" = output file"
                        "w" = input file; wildcard characters allowed
                        "l" = input file; lists of files allowed (only valid
                              on TANDEM version)
                        "v" = input file; VC members allowed (only valid on
                              TANDEM version)
                        "?" = input/output file (no existence check done)
                        "e" = output file but must already exist
                        "a" = ouptut file;  allow appending or overwiting
                              existing file
                    4.  file mask
                    5.  Default initial value

        Processing:  1.  Get a response
                     2.   Process it

****************************************************************************/

int py_getfile (const char *program, const char *file, char direction,
         const char *mask, char *initial_value)

{
    int rc;
    char response[500];

    strcpy (response, initial_value);

    return getfile(program, file, direction, mask, response);
}


/****************************************************************************

        NAME:  switch_dir

        Purpose:  Let the user change directories

        Arguments:  1.  Program name
                    2.  File description
                    3.  Directory

        Processing:  1.  Call sd
                     2.  Set the directory

****************************************************************************/

static void switch_dir(const char* program, const char* file, char* dir)
{
#ifndef TANDEM
        FILE *sd_output;
        char results_filename[100];
        char* temp_dir;
#endif

#ifdef IBM
        erase ();
        system ("sd");

        temp_dir = getenv ("TEMP");

        if (temp_dir == NULL)
                temp_dir = "C:\\";

        sprintf (results_filename, "%s\\sdtmp.bat",
                 temp_dir);
        sd_output = fopen (results_filename, "r");
        if (sd_output == NULL)
        {
                perror ("Can't get results of sd");
                printf ("Press any key to continue");
                getachar ();
        }
        else
        {
                fgets (dir2, 800, sd_output);
                fclose (sd_output);
                unlink (results_filename);

                if (dir2[3] == '"')
                {
                        strcpy (dir, &dir2[4]);
                        dirp = strchr (dir, '"');
                        if (dirp != NULL)
                                *dirp = 0;
                }
                else
                        strcpy (dir, &dir2[3]);

                chdir (dir);
        }
        return;

#endif

#ifdef TANDEM
        do
        {  /*  loop until valid input  */
                erase ();
                saya (3, 32, "Program:");
                saya (3, 42, program);
                saya (5, 35, "File:");
                saya (5, 42, file);
                saya (7, 22, "Current vol/subvol:");
                rpad (dir, 14);
                geta (7, 42, dir);
                if (!rd ())
                        return (ERROR);
                trim (dir);
        }  /*  loop until valid input  */
        while (chvol (dir));

        return;
#endif

#ifdef UNIX
        system ("sd.exe getfile");

        sprintf (results_filename, "/tmp/sd.getfile");
        sd_output = fopen (results_filename, "r");
        if (sd_output == NULL)
        {
                perror ("Can't get results of sd");
                printf ("Press any key to continue");
                getachar ();
        }

        fgets (dir, 800, sd_output);
        fclose (sd_output);
        unlink (results_filename);

        chdir (dir);
        return;
#endif
}
/****************************************************************************

        NAME:  getfile

        Purpose:  To get the name of a file

        Arguments:  1.  Program name
                    2.  File description
                    3.  "i" or "o" or "w" or "l" or "v" or "?" or "e" or "a"
                        "i" = input file
                        "o" = output file"
                        "w" = input file; wildcard characters allowed
                        "l" = input file; lists of files allowed (only valid
                              on TANDEM version)
                        "v" = input file; VC members allowed (only valid on
                              TANDEM version)
                        "?" = input/output file (no existence check done)
                        "e" = output file but must already exist
                        "a" = ouptut file;  allow appending or overwiting
                              existing file
                    4.  file mask
                    5.  Location to return file name

        Processing:  1.  Get a response
                     2.   Process it

****************************************************************************/

int getfile (const char *program, const char *file, char direction,
         const char *mask, char *response)

{
      char io[127];
      char command[80];
      char dir[800];
      char err_msg[80];
      int answer;
      char answer2;
      char filename[141];
      int error;
      char dir2[800];
      char* dirp;

      err_msg[0] = 0;

      *command = *mask;  /*  make the compiler happy */

      strcpy (filename, response);

      switch (direction)
      {
	      case 'a':
	      case 'o':  strcpy (io, "output");  break;
#ifdef TANDEM
	      case 'v':  strcpy (io, "input (VC members OK)");  break;
	      case 'l':  strcpy (io, "input (lists OK)");  break;
#endif
	      case 'i':  strcpy (io, "input");  break;
	      case 'w':  strcpy (io, "input (wildcards OK)");  break;
	      case '?':  strcpy (io, "input/output");  break;
	      case 'e':  strcpy (io, "output (must already exist)"); break;
	      default:  fprintf (stderr, "GETFILE:  Invalid direction:  %c\n",
				 direction);
		      abort();
      }

#ifdef IBM
#ifndef WIN32API
#define GETFILEG
#endif
#endif

#ifdef UNIX
#define GETFILEG
#endif

#ifdef TANDEM
      strcpy (dir, getenv ("DEFAULTS"));
#endif

#ifdef GETFILEG
      if ((direction == 'i') || (direction == 'e'))
      {
          getfileg (program, file, io, mask, dir, filename);
	  if (strcmp(filename, response) == 0)
	  {
		  if (direction == 'i')
		  {
			  erase ();
			  return (ERROR);
		  }
	  }
	  else
	  {
		  getcwd (dir, 800);
		  dirpath (dir);
		  make_filename (response, filename,
				 dir);
		  strcpy (save_response, response);
		  return 0;
	  }
      }
#endif

      while (TRUE)
      {

#ifdef IBM
#define UI
#endif

#ifdef UNIX
#define UI
#endif

#ifdef UI
	      getcwd (dir, 800);
	      dirpath (dir);
#endif

         erase ();
         saya (3, 32, "Program:");
         saya (3, 42, program);
         saya (5, 35, "File:");
         saya (5, 42, file);
#ifdef UI
         saya (7, 22, "Current directory:");
#endif

#ifdef TANDEM
         saya (7, 22, "Current vol/subvol:");
#endif
         saya (7, 42, dir);
         saya (9, 27, "Type of file:");
         saya (9, 42, io);
         saya (12, 21, "Enter file name");
#ifdef TANDEM
         saya (13, 24, "or '?' to list current subvolume");
         saya (14, 24, "or '\\' to change current subvolume");
#else
         saya (13, 24, "or '?' to list current directory");
         saya (14, 24, "or '\\' to change current directory");
#endif
         saya (15, 24, "or ESC  to abort");
         rpad (filename, 40);
         geta (17, 21, filename);
         gotoxy (10, 20);
         blink ();
         saya (20, 10, err_msg);
         normal ();
         err_msg[0] = 0;
         box (18, 11, 64, 18);
         if (!rd ())
            strcpy (filename, "\033");
         ztrim (filename);

#ifdef IBM
         if ((strchr (filename, '\\')) || (filename[1] == ':'))
#endif

#ifdef TANDEM
         if (strchr (filename, '.'))
#endif

#ifdef UNIX
         if (strchr (filename, '/'))
#endif

             *dir = 0;

         switch (filename[0])
         {
            case '?':  erase ();
#ifdef TANDEM
#define GOTSYS
                       sprintf (command, "fup files %s", dir);
#endif

#ifdef IBM
#define GOTSYS
                       sprintf (command, "dir %s/p", mask);
#endif

#ifdef UNIX
#define GOTSYS
                       sprintf (command, "ls %s", mask);
#endif

#ifndef GOTSYS
#error System not defined
#endif

#ifdef GETFILEG
                       getfileg (program, file, io, mask, dir, filename);
                       break;
#else

                       fflush (stdout);
                       system (command);
                       printf ("Press any key to continue.");
                       getachar ();
                       break;
#endif
	    case 0:
		    strcpy(err_msg, "Please specify a file name");
		    break;

            case '\\':
                       if (filename[1] == 0)
                       {  /*  \ is only character  */
                               switch_dir(program, file, dir);
                       }  /*  \ is only character  */
                       break;

            case 033:           /* ESCAPE */
                if (filename[1] == 0) /* If only character */
                {  /* escape requested */
			erase ();
			return (ERROR);
                }  /* escape requested */

                /*  Else fall through to default processing */

            default:   error = getfilel ((char) direction, filename);
                       switch (error)
                       {
                           case 0:  erase ();
                                    make_filename (response, filename, dir);
				    strcpy (save_response, response);
                                    return (0);

                           case -1:
                                    if (direction == 'a')
                                    {
                                        erase ();
                                        saya (3, 32, "Program:");
                                        saya (3, 42, program);
                                        saya (5, 35, "File:");
                                        saya (5, 42, file);
#ifdef UI
                                        saya (7, 22, "Current directory:");
#endif

#ifdef TANDEM
                                        saya (7, 22, "Current vol/subvol:");
#endif
                                        saya (7, 42, dir);
                                        saya (9, 27, "Type of file:");
                                        saya (9, 42, io);
                                        saya (11, 32, "Filename:");
                                        saya (11, 42, filename);
                                        saya (13, 10,
                                   "File already exists.   Do you want to...");
                                        saya (15, 10,
                                              "1.  Overwrite the file");
                                        saya (16, 10,
                                              "2.  Append to the file");
                                        saya (17, 10,
                                            "3.  Pick a different file name");
                                        saya (18, 10,
                                              "0.  Abort the request.");
                                        saya (20, 10,
                                              "Enter your choice (0-3):");
                                        get_one (20, 36, &answer2, "0123");

                                        switch (answer2)
                                        {
                                        case '0':  return ERROR;
                                        case '1':
                                            erase ();
					    make_filename (response, filename,
						    dir);
					    strcpy (save_response, response);
                                            return (0);
                                        case '2':
                                            erase ();
                                            make_filename (response, filename,
                                                           dir);
					    strcpy (save_response, response);
                                            return (1);
                                        case '3':  break;
                                        }
                                    }
                                    else
                                    {
                                        saya (20, 10,
                      "File already exists.  Do you want to overwrite it?");
                                        answer = FALSE;
                                        getl (20, 62, &answer);
                    set_field(20, 62);
                                        rd ();
                                        if (answer)
                                        {
                                            erase ();
                                            make_filename (response, filename,
                                                           dir);
					    strcpy (save_response, response);
                                            return (0);
                                        }

                                    }
                                    break;

                           case -2: strcpy (response, filename);
                                    strcpy (err_msg, "File does not exist");
                                    break;

                           default: sprintf (err_msg,
                                       "File # %d in list does not exist",
                                       error);
                                    break;
                       }
                       break;
         }
      }
      NO_EXIT ();
}


/***************************************************************************/
/* NAME                                                                    */
/*      make_filename - Make a fully-qualified filename                    */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This routine will concatinate a filename with its pathname         */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      static void make_filename (char * target ,char * source            */
/*           ,char * path)                                                 */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           target  (W) - The string to be filled                         */
/*           source  (R) - The filename to be used                         */
/*           path  (R) - The path to be added or null                      */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           Nothing                                                       */
/*                                                                         */
/***************************************************************************/


static void make_filename (char * target, char * source, char * path)

{
    if ((*path == 0) || (source[0] == '('))
        strcpy (target, source);
    else
#ifdef TANDEM
        sprintf (target, "%s.%s", path, source);
#else
        sprintf (target, "%s%s", path, source);
#endif


}
#ifdef GETFILEG


/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           callback

        Purpose:        Callback from list_pick to handle our commands

        Description:    This function is called by list_pick_ex when
                        one of our commands is to be executed

        Arguments:      1.  list entry
                        2.  command
                        3.  dont_care (extra data not used)

        Processing:     Process the command and return to listpick

        Returns:        Instructions to list_pick_ex
                        0 = Keep going
                        1 = Return current entry (not used here)
                        2 = return NULL

*****************************************************************************
*****************************************************************************
****************************************************************************/

static int callback(void* list_entry, int key, void* values)
{
        struct list_entry
        {
                struct list_entry* l_next;
                struct list_entry* l_prev;
                char* l_descr;
        } *my_list_entry;

        struct values
        {
                char* program;
                char* file;
                char* dir;
        } *my_values;

        char command[800];

        my_list_entry = (struct list_entry*) list_entry;
        my_values = (struct values*) values;

        switch (key)
        {
                /* Edit the highlighted  record */
                case 'e':
                        sprintf(command, "%s %s", getenv("EDITOR"),
                                  my_list_entry->l_descr);
                        system(command);
                        return 0;

                /*  Move to new directory */
                case 'm':
                        switch_dir(my_values->program, my_values->file,
                                   my_values->dir);
                        retry = TRUE;
                        return 2;

                default:
                        lputc ('\007');
        } /* switch */

        return 0;
}

/**********************************************************************
*
* FUNCTION: getfileg
*
* DESCRIPTION:
*       Get a file from the user based on a list passed displayed on the screen
*
* CALLING SEQUENCE:
*       getfileg (direction, mask, filename)
*
* INPUT PARAMETERS:
*       program               Name of program
*       file                  Description of file
*       direction             Specifies direction of transfer
*       dir                   Current directory
*       mask                  Specifies filename mask
*
*
* OUTPUT PARAMETERS:
*       filename              File name set by user
*
* RETURNED VALUE:
*       None.
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

static void getfileg (const char *program, const char* file,
                      const char *direction, const char *mask,
                      char* dir, char *filename)

{
        struct state
        {
                const char* program;
                const char* file;
                char* dir;
        } state;

    char full_path[MAX_PATH];
    char full_name[355];
    char my_direction[80];
    char my_instruction[180];
    char cur_dir[MAX_PATH];

    struct files
    {
        struct files *f_next;
        struct files *f_prev;
        char *f_name;
    } file_root;

    struct files *current_file;

#ifdef IBM
    union REGS dos_regs;
    char dta[129];
    char call[30];
    int result;
#endif

#ifdef UNIX
    DIR *dirp;
    struct dirent *dir_entry;
    int error_pause = FALSE;
    char my_mask[80];
#endif

    do
    {
            file_root.f_next = file_root.f_prev = &file_root;
            getcwd (cur_dir, MAX_PATH);

#ifdef UNIX
            if (strcmp (mask, "*.*") == 0)
                strcpy (my_mask, "*");
            else
                strcpy (my_mask, mask);

            dirp = opendir (cur_dir);
            if (dirp == NULL)
            {
                perror ("Can't open directory");
                exit (ERROR_RETURN);
            }

            erase ();

            while ((dir_entry = readdir (dirp)) != NULL)
            {
                strcpy (full_name, dir_entry->d_name);

                sprintf (full_path, "%s/%s", cur_dir, full_name);

                if (patcmp (dir_entry->d_name, my_mask))
                {

                    trim (full_name);

                    /*
                     *  NOW DISPLAY THE ENTRY
                     */

                    current_file = (struct files *) malloc (sizeof (struct files));
                    if (current_file == NULL)
                    {
                        printf ("Out of memory\n");
                        exit (ERROR_RETURN);
                    }

                    list_add_ascending (&file_root, current_file, full_name);

                }
            }

            if (error_pause)
            {
                printf ("\nPress any key to continue\n");
                getachar ();
            }

            closedir (dirp);
#endif

#ifdef IBM


            /*  LOOK FOR FIRST MATCHING FILE */

            segread (&my_regs);

            strcpy (full_path, mask);

            dos_regs.x.ax = 0x4E00;
            dos_regs.x.dx = (int) full_path;
            dos_regs.x.cx = 0x0;  /*  no attributes */
            strcpy (call, "Find First (dir.)");

            set_dta (dta);
            result = intdosx (&dos_regs, &dos_regs, &my_regs);
            reset_dta ();

            while (dos_regs.x.cflag == 0)  /* while carry flag is not set */
            {

                /*  PROCESS THE FILE LOCATED */

                strcpy (full_name, &dta[30]);

                trim (full_name);

                /*
                 *  NOW DISPLAY THE ENTRY
                 */

                current_file = (struct files *) malloc (sizeof (struct files));
                if (current_file == NULL)
                {
                    printf ("Out of memory\n");
                    exit (ERROR_RETURN);
                }

                list_add_ascending (&file_root, current_file, full_name);


                /*  NOW LOOK FOR THE NEXT OCCURANCE */

                dos_regs.x.ax = 0x4F00;
                dos_regs.x.dx = (int) dta;
                strcpy (call, "Find Next (dir.)");

                set_dta (dta);
                result = intdosx (&dos_regs, &dos_regs, &my_regs);
                reset_dta ();
            }

            /*  CONTROL COMES HERE WHEN A DOS SEARCH FAILS */

            if (dos_regs.x.ax != 18)
            {
                printf
             ("*** ERROR *** Unexpected error code %d from %s\n",
                        dos_regs.x.ax, call);
                return;
            }

#endif

            /*
             *  NOW GET THE USER'S CHOICE
             */

            state.program = program;
            state.file = "file";
            state.dir = dir;
            int dont_care;

            retry = FALSE;

	    switch (direction[0])
	    {
		    case 'a':
		    case 'o':  strcpy (my_direction, "output");  break;
		    case 'i':  strcpy (my_direction, "input");  break;
		    case 'w':  strcpy (my_direction, "input (wildcards OK)");
			       break;
		    case '?':  strcpy (my_direction, "input/output");  break;
		    case 'e':  strcpy (my_direction,
					 "output (must already exist)");
			       break;
		    default:  fprintf (stderr,
				       "GETFILE:  Invalid direction:  %c\n",
				       direction);
			    abort();
	    }

	    sprintf(my_instruction, "Select a file to be used for %s",
		      my_direction);

	    current_file = list_pick_ex (program,
					 "",
					 "",
					 &file_root,
					 "E(dit) M(ove to new directory)",
					 "em",
					 callback,
					 &state,
					 FALSE,
					 &dont_care,
					 1,
					 FALSE,
					 my_instruction,
					 NULL
					);

            if (current_file != NULL)
            {
                strcpy (filename, current_file->f_name);
            }

            list_purge (&file_root);
    } while (retry);
}

#ifdef IBM
/****************************************************************************

        NAME:  set_dta

        Purpose:  To set the DTA address

        Arguments:  dta address

        Processing:  Call DOS to set DTA address

****************************************************************************/

void set_dta (dta)

char *dta;

{
    union REGS dos_regs;

    dos_regs.x.ax = 0x2F00;  /* get dta */
    segread (&my_regs);

    intdosx (&dos_regs, &dos_regs, &my_regs);

    saved_ds = my_regs.es;
    saved_dx = dos_regs.x.bx;

    dos_regs.x.ax = 0x1A00;
    my_regs.ds = my_regs.ds;

    dos_regs.x.dx = _FP_OFF (dta);

    intdosx (&dos_regs, &dos_regs, &my_regs);
}

/****************************************************************************

        NAME:  reset_dta

        Purpose:  To set the DTA address  to its saved value

        Arguments:  none

        Processing:  Call DOS to set DTA address

****************************************************************************/

void reset_dta ()

{
    union REGS dos_regs;

    dos_regs.x.ax = 0x1A00;
    my_regs.ds = saved_ds;
    dos_regs.x.dx = saved_dx;

    intdosx (&dos_regs, &dos_regs, &my_regs);
}

#endif
#endif
