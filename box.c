/*
 *  BOX.C
 *
 *  Glenn Story -- 6/1/87
 *
 *  This program draws a box around a region
 */

#include "gs.h"

#ifdef box
#undef box
#endif

#ifdef IBM
#define UL_CORNER 201
#define UR_CORNER 187
#define LL_CORNER 200
#define LR_CORNER 188
#define H_LINE 205
#define V_LINE 186
#else
#ifdef VAX
#define UL_CORNER '+'
#define UR_CORNER '+'
#define LL_CORNER '+'
#define LR_CORNER '+'
#define H_LINE '-'
#define V_LINE '|'
#endif
#endif

/***************************************************************************/
/* NAME                                                                    */
/*      box - Draw a box around an area of the screen                      */
/*                                                                         */
/* PURPOSE                                                                 */
/*      This routine is part of the screen interface package.  It          */
/*   draws a graphics box around text on the screen.                       */
/*                                                                         */
/* SYNOPSIS                                                                */
/*      void box (int x1 ,int x2 ,int x2 ,int y2)                          */
/*                                                                         */
/* DESCRIPTION                                                             */
/*      Arguments                                                          */
/*           x1  (R) - Column of upper-left corner                         */
/*           y1  (R) - Row of upper-left corner                            */
/*           x2  (R) - Column of lower-right corner                        */
/*           y2  (R) - Row of lower-right corner                           */
/*                                                                         */
/*                                                                         */
/*      Return Value                                                       */
/*           None.                                                         */
/*                                                                         */
/***************************************************************************/


void box (int x1, int y1, int x2, int y2)

{
    int x, y;

#ifdef TANDEM
   char termtype;

    static char UL_CORNER = 0;
    static char UR_CORNER;
    static char LL_CORNER;
    static char LR_CORNER;
    static char H_LINE;
    static char V_LINE;
    static char graph_on;
    static char graph_off;

    if (UL_CORNER == 0)             /*  If we need to initialize */
      {
         termtype = tterm ();       /*  Ask terminal for its type */

         switch (termtype)
         {
           case 'F':
              UL_CORNER = 'I';
              UR_CORNER = ';';
              LL_CORNER = 'H';
              LR_CORNER = '<';
              H_LINE = 'M';
              V_LINE = ':';
              graph_on = 14;
              graph_off = 15;
              break;

           case 'D':
           case 'J':
              UL_CORNER = 'O';
              UR_CORNER = '!';
              LL_CORNER = 'R';
              LR_CORNER = '$';
              H_LINE = 'G';
              V_LINE = '?';
              graph_on = 14;
              graph_off = 15;
              break;

          case 'G':
             UL_CORNER = '+';
             UR_CORNER = '+';
             LL_CORNER = '+';
             LR_CORNER = '+';
             H_LINE = '-';
             V_LINE = 'I';
             graph_on = 0;
             graph_off = 0;
             break;

          default:
             UL_CORNER = '+';
             UR_CORNER = '+';
             LL_CORNER = '+';
             LR_CORNER = '+';
             H_LINE = '-';
             V_LINE = '|';
             graph_on = 0;
             graph_off = 0;
             break;

      } /* switch */
    }  /*  if */
    lputc (graph_on);
#define GRAPHOFF
#endif

#ifdef UNIX
   char* termtype;

    static char UL_CORNER = 0;
    static char UR_CORNER;
    static char LL_CORNER;
    static char LR_CORNER;
    static char H_LINE;
    static char V_LINE;
    static char graph_on;
    static char graph_off;

    if (UL_CORNER == 0)             /*  If we need to initialize */
      {
         termtype = getenv ("TERM");

         if (strcmp (termtype, "dtterm") == 0)
         {
              UL_CORNER = 'l';
              UR_CORNER = 'k';
              LL_CORNER = 'm';
              LR_CORNER = 'j';
              H_LINE = 'q';
              V_LINE = 'x';
              graph_on = '\016';
              graph_off = '\017';
         }
         else
         {
             UL_CORNER = '+';
             UR_CORNER = '+';
             LL_CORNER = '+';
             LR_CORNER = '+';
             H_LINE = '-';
             V_LINE = '|';
             graph_on = 0;
             graph_off = 0;
         }
    }  /*  if */
    lputc (graph_on);
#define GRAPHOFF
#endif

    /*
     *  DRAW THE TOP ROW
     */

    gotoxy (x1, y1);
    lputc ( UL_CORNER);

    for (x = x1+1; x < x2; ++x)
        lputc ( H_LINE);

    lputc ( UR_CORNER);

    /*
     *  DRAW THE LINES DOWN THE SIDE
     */

    for (y = y1+1; y < y2; ++y)
    {
        gotoxy (x1, y);
        lputc ( V_LINE);

        gotoxy (x2, y);
        lputc ( V_LINE);
    }

    /*
     *  PUT THE BOTTOM LINE
     */

    gotoxy (x1, y2);
    lputc ( LL_CORNER);

    for (x = x1+1; x < x2; ++x)
        lputc ( H_LINE);

    lputc ( LR_CORNER);

#ifdef GRAPHOFF
    lputc (graph_off);
#endif
}
