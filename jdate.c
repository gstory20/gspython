
/* ------ jdate: convert a julian day number to a calendar date
*
*        reference:
*           collected algorithms of the acm, number 199
*
*        purpose:
*           jdate converts a julian day number to the
*           corresponding gregorian calendar date.  the procedure
*           is valid for any valid gregorian calendar date.
*        note: this is the true julian date, not the serial date
*
*        usage:
*           jdate(julian,&day,&month,&year)
*
*        parameters:
*           julian - julian day number (long int)
*           day    - day of month
*           month  - month
*           year   - year
*
*       note:  day, month, year must be pointers since values are
*              returned!
*
*        pgm: s. stern, jmb realty corp, chicago; feb, 1980.
*             converted from SRC.FLIB to C 6/85
*************************************************************************/

void  jdate(julian,day,month,year)

      long julian;
      int *day,*month,*year;

      {
      long d,m,y;

/*
*  use long ints as work space for returning values
*/

      julian = julian - 1721119L;
      y = (4*julian - 1)/146097L;
      julian = 4*julian - 1 - 146097L*y;
      d = julian/4;
      julian = (4*d+3)/1461;
      d = 4*d + 3 - 1461*julian;
      d = (d + 4)/4;
      m = (5*d - 3)/153;
      d = 5*d - 3 - 153*m;
      d = (d + 5)/5;
      y = 100*y + julian;

      if (m < 10)
            m = m + 3;
         else
            {
            m = m - 9;
            y++;
            }
/*
*        move results into caller's space
*/
	*year = (int) y;
	*month = (int) m;
	*day = (int) d;
      return;
}
