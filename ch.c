#include "gs.h"

#ifdef MSC
#include <ctype.h>
#endif

#ifdef vax
#include ctype
#endif

#ifdef TANDEM
#include <ctypeh> nolist
#endif

#include <ctype.h>

/*  convert a lower case letter to upper case */
int chupper(int c)
        {
        return (islower(c) ? c - ('a' - 'A') : c);
        }
/* convert an upper case letter to lower case */
int chlower(int c)
        {
        return (isupper(c) ? c - ('A' - 'a') : c);
        }
