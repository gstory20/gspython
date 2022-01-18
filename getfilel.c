/****************************************************************************

        PROGRAM DESCRIPTION

        NAME    getfilel.c

        AUTHOR  Glenn Story

        DATE WRITTEN 3/9/93 (10/2/87)

        SYSTEM  Subroutine

        DESCRIPTION This function will get a file name for the caller
                    This is the low-level portion of this functionality
                    This is the UNIX version of this file.

        MODIFICATION HISTORY


 ****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

#ifdef box
#undef box
#endif

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

        Global data

 ****************************************************************************/

extern int errno;

/****************************************************************************

        NAME:  getfile

        Purpose:  To get the name of a file

        Arguments:  1.  "I" or "O", etc.
                    2.  Location to return file name

        Processing:  1.  Get a response
                     2.   Process it

        Return values:  0 = No errors
                       -1 = File already exists
                       -2 = File does not exist
                       >0 = File #n in list does not exist

****************************************************************************/

int getfilel (char direction, char *filename)

{
      int defaults[13];
      int file_number;
      int i;
      char one_file[80];
      int j;
      char my_name[80];

                      strcpy  ((char *) defaults, "                       ");
                      strncpy ((char *) my_name, filename, 80);

                      if ((direction == 'o') || (direction == 'a'))
                      {
                        if (file_exists (filename))
                            return (-1);
                        else             /* output file does not exist */
                           return (0);
                      }
                      else if ((direction == 'i') /* direction is input */
                           ||  (direction == 'e')) /* dir. is output (exists)*/
                       {
                        if (file_exists (filename))
                           return (0);
                        else
                           return (-2);
                       }
                      else if (direction == 'w') /* direction is input */
                       {
                        if (file_exists (filename))
                           return (0);
                        else
                           if (strchr (filename, '*') != NULL)
                              return (0);
                           else
                              return (-2);
                       }
                       else if (direction == 'l')  /* input with lists */
                       {  /*  Parse the list  */
                         if (filename[0] == '(')
                         {  /*  Loop through each name one the list  */
                           for (i = 1, j = 0, file_number = 0;
                                   filename[i] != 0; ++i)
                               switch (filename[i])
                               {  /*  switch filename[i]  */
                                   case ' ':    break;

                                   case ')':
                                   case ',':    one_file[j] = 0;
                                                ++file_number;
                                                if (getfilel ((char) 'i',
                                                    one_file))
                                                        return (file_number);

                                                if (filename[i] == ')')
                                                    return (0);

                                                j = 0;
                                                break;

                                   default:     one_file[j++] = filename[i];
                                                break;
                               }  /*  switch filename[i]  */
                           return (0);
                         }  /*  Loop through each name one the list  */
                         else
                           return getfilel ((char) 'w', filename);

                       }  /*  Parse the list  */
                       else   /* direction is '?' so no checking on
                                 existence */
                              return (0);
      NO_EXIT ();
}
