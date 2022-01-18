/****************************************************************************
*****************************************************************************
*****************************************************************************

        PROGRAM DESCRIPTION

        NAME         patcmp.c

        AUTHOR       Glenn Story

        DATE WRITTEN 5/1/88

        DESCRIPTION  This program contains the patcmp function that
           compares a string to a pattern.

        MODIFICATION HISTORY

        3/7/91 - No match if pattern shorter than string
        7/1/96 - Add unequal compares and case insensativity
        10/2/96 - Add regular expression capability
	10/13/13 - C99 Compatibility
	
*****************************************************************************
*****************************************************************************
*****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"
#include "regexp.h"

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

/*
**  The following procedure incorporates the original functionality of
**  patcmp().  It can be called directly by a user that does not want the
**  functionality of the new patcmp (which follows this function).
*/

int patcmp2 (const char *string, const char *pattern)

{
      const char *cur_string;          /*  current position in string     */
      const char *cur_pattern;         /*  current position in pattern    */

      cur_string = string;
      cur_pattern = pattern;

      while (*cur_pattern)
      {
         switch (*cur_pattern)
         {
            case '?':   ++cur_pattern;
                        ++cur_string;
                        break;

            case '*':   while (*cur_pattern == '*')
                                ++cur_pattern;

                        if (!*cur_pattern)
                           return (TRUE); /*  end of pattern              */

                        while (*cur_pattern == '?')
                        {
                            ++cur_pattern;
                            ++cur_string;
                        }

                        while (TRUE)
                        {

                            while (*cur_string != *cur_pattern)
                            {
                                if (!*cur_string)
                                    return (FALSE);   /*  end of string    */

                                ++cur_string;
                            } /* while */

                            if (patcmp2 (cur_string, cur_pattern))
                                return (TRUE);
                            else
                                ++cur_string;
                        } /* while */

/*                         break;  -- unreachable */

             default:  if (!*cur_string)
                           return (FALSE); /*  end of string              */

                       if (*cur_pattern++ != *cur_string++)
                           return (FALSE); /*  pattern mismatch           */

                        break;
         } /* switch */
      } /* while */

     return (*cur_string == 0);          /*  end of pattern                 */
}  /* patcmp2 */


/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           patcmp

        Purpose:        To compare a string to a pattern

        Description:    This function compares a string to a pattern,
                        possibly containing wildcard characters * or ?.

        Arguments:      1.  String
                        2.  Pattern

        Processing:     search through the string and the pattern
			See patcmp_help for details of pattern options

        Returns:        TRUE if they match; FALSE otherwise

*****************************************************************************
*****************************************************************************
****************************************************************************/

int patcmp (const char *string, const char *pattern)
{
    const char *pattern_data;
    enum {EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, NOT_EQUAL}
        compare_type;
    int result;
    char *source;
    char *target;
    int upshifted;
    int return_value;
    static regexp *save_prog = NULL;

    if (pattern != NULL)
    {
        if (string == NULL)
	{
            return FALSE;
	}
	else
        {
            switch (pattern[0])
            {
                case '%':  save_prog = regcomp (&pattern[1]);
                           if (save_prog == NULL)
			   {
                               return FALSE;
			   }
			   
                           return regexec (save_prog, string, FALSE);

                case '>':  if (pattern[1] == '=')
                           {
                               compare_type = GREATER_EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else
                           {
                               compare_type = GREATER;
                               pattern_data = &pattern[1];
                           }
                           break;


                case '<':  if (pattern[1] == '=')
                           {
                               compare_type = LESS_EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else if (pattern[1] == '>')
                           {
                               compare_type = NOT_EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else
                           {
                               compare_type = LESS;
                               pattern_data = &pattern[1];
                           }
                           break;


                case '=':  if (pattern[1] == '=')
                           {
                               compare_type = EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else if (pattern[1] == '>')
                           {
                               compare_type = LESS_EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else if (pattern[1] == '<')
                           {
                               compare_type = GREATER_EQUAL;
                               pattern_data = &pattern[2];
                           }
                           else
                           {
                               compare_type = EQUAL;
                               pattern_data = &pattern[1];
                           }
                           break;

                case '!':  if (pattern[1] == '=')
                               pattern_data = &pattern[2];
                           else
                               pattern_data = &pattern[1];

                           compare_type = NOT_EQUAL;
                           break;

                case '\\': compare_type = EQUAL;
                           pattern_data = &pattern[1];
                           break;

                default:   compare_type = EQUAL;
                           pattern_data = pattern;
                           break;
            }

            if (pattern_data[0] == '^')
            {  /* case-insensitive search wanted */
                source = malloc (strlen (string) + 1);
                target = malloc (strlen (pattern_data));

                if ((target == NULL) || (source == NULL))
                {
                    fprintf (stderr,
                            "patcmp:  Not enough memory for specified search");

                    if (source)
                        free (source);

                    if (target)
                        free (target);

                    return TRUE;
                }

                strcpy (source, string);
                strupr (source);

                strcpy (target, &pattern_data[1]);
                strupr (target);

                upshifted = TRUE;
            }  /* case-insensitive search wanted */
            else
            {  /* case-sensitive search wanted */
                source = (char *) string;
                target = (char *) pattern_data;

                upshifted = FALSE;
            }  /* case-sensitive search wanted */
            if (compare_type == EQUAL)
                return_value = patcmp2 (source, target);
            else if (compare_type == NOT_EQUAL)
                return_value = !patcmp2 (source, target);
            else
            {  /* unequal compares */
                result = strcmp (source, target);

                switch (compare_type)
                {
                case LESS:           return_value = (result <  0); break;
                case GREATER:        return_value = (result >  0); break;
                case LESS_EQUAL:     return_value = (result <= 0); break;
                case GREATER_EQUAL:  return_value = (result >= 0); break;
                case NOT_EQUAL:      return_value = (result != 0); break;
                case EQUAL:          abort();  /*  make compile happy */
                }

            }  /* unequal compares */

            if (upshifted)
            {
                free (source);
                free (target);
            }

            return return_value;
        }
    }
    
    return TRUE;
}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        NAME:           patcmp_help_text

        Purpose:        return a help string to the caller

        Description:    Returns a help string to the caller

        Arguments:      None

        Processing:     nothing but a return statement

        Returns:        a help string

*****************************************************************************
*****************************************************************************
****************************************************************************/

const char* patcmp_help_text(void)
{
        return "\n\
                The pattern may contain the following meta characters:\n\
\n\
                At the beginning of the string:\n\
\n\
                %  String is a regular expression\n\
                =  String must equal pattern\n\
                == String must equal pattern\n\
                >  String must be greater than pattern\n\
                >= String must be greater than or equal to pattern\n\
                <  String must be less than pattern\n\
                <= String must be less than or equal to pattern\n\
                <> String must be unequal to pattern\n\
                != String must be unequal to pattern\n\
                \\x 'x' is treated literally (instead of being\n\
                    being interpreted as above\n\
                none of the above:  String must equal pattern\n\
\n\
                Following the above:\n\
                ^  Comparison is case insensative\n\
\n\
                Anywhere in string (only for equal comparisons)\n\
                *  Zero or more occurances of any character\n\
                ?  Exactly one occurance of any character\n\
                none of the above:  character must match exactly\n";
}
			
			
#ifdef DEBUG

main ()

{
   char my_string[80];
   char my_pattern[80];

   while (TRUE)
   {
      printf ("Enter string:  ");
      fgets (my_string, 79, stdin);

      if (my_string[0] == ' ')
         exit (GOOD_RETURN);

      stripnl (my_string);

      printf ("Enter pattern:  ");
      fgets (my_pattern, 79, stdin);

      if (my_pattern[0] == ' ')
         exit (GOOD_RETURN);

      stripnl (my_pattern);
      trim (my_pattern);

     if (patcmp (my_string, my_pattern))
         printf ("Matched\n\n");
      else
         printf ("Not matched\n\n");
   } /* while */
}

#endif

