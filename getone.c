/*  GETONE.C

    Glenn Story - 2/22/84

    This function will get one of a set of characters

*/

#include "gs.h"

extern int lscrn_saved_x;

void get_one (y, x, c, s)

int x, y;
char *c;
const char *s;

{
    get_one_default (y, x, c, s, '?');
}

void get_one_default (y, x, c, s, default_value)

int x, y;
char *c;
const char *s;
int default_value;

{
        int i;
        char temp[2];

        temp[0] = (char) default_value;
        temp[1] = 0;

	/*  If x is relative make it absolute here in case we retry */
	if (x < 1)
		x = lscrn_saved_x - x;

        while (TRUE)
        {
                geta (y, x, temp);
                if (rd ())
                    *c = temp[0];
                else
                    *c = 27;

                for (i=0; s[i] != 0; i++)
                        if (s[i] == *c)
                                return;
                putchar (7);
		clear();
        }
}

