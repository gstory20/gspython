/*  ZTRIM.C

    Glenn Story - 2/21/84

    This subroutine will trim a string by removing trailing spaces
    It differs from TRIM in that this routine will set a null string to
    be zero-length, whereas TRIM will leave a single blank
*/

#include "gs.h"

void ztrim (char *s)

{
        int i;

        for (i = (int) strlen (s) - 1;
	     (i != -1) && ((s[i] == ' ') || (s[i] == '\t'));
	     s[i--] = 0);

}

