/*  RPAD.C

    Glenn Story - 2/23/84

    This function will pad a string to the right to a maximum size.

*/

#include "gs.h"

void rpad (char *string, int max_length)

{
        int i;

        for (i = (int) strlen (string); i < max_length; i++)
                string[i] = ' ';
        string[i] = 0;
}

