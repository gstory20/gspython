/*  TRIM.C

    Glenn Story - 2/21/84

    This subroutine will trim a string by removing trailing spaces

*/

#include "gs.h"

void trim (char *s)

{
        int i;

        i = (int) strlen (s) - 1;

        while ((i > 0) && (s[i] == ' '))
            s[i--] = 0;

}

