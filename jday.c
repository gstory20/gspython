/* ------ jday: convert a calendar date to a julian date --------------

        reference: algorithm 199, collected algorithms from the acm

        purpose:
           jday converts a calendar date, gregorian calendar,
           to the corresponding julian day number.  the procedure
           is valid for any valid gregorian calendar date.
        note:  this is the true julian date, not the serial date.

        usage:
           julian = jday(day,month,year)

        parameters:
           day    - day of month (spi)
           month  - month (spi)
           year   - year (all 4 digits; spi)
           julian - julian day number (long int)

        pgm: s. stern, jmb realty corp, chicago; feb, 1980.
-------------------------------------------------------------------------*/
      long int
      jday(day,month,year)
      int day,month,year;
      {
      long int c;
      long int julian;
      long num1 = 146097L;
      long num2 = 1721119L;

      if (month > 2)
            month -= 3;
         else
            {
            month += 9;
            year--;
            }
      c = year/100;
      year -= (int) (100*c);
      julian =  (num1*c)/4L;
      julian += (1461L*year)/4L;
      julian += (153L*month+2)/5L;
      julian += day+num2;
      return(julian);
      }
