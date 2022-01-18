%module _pyscrn
   %include typemaps.i
   %{
#include "lscrn.h"
#include "hscrn.h"
#include "gslib.h"
#include "getachar.h"
	%}

   enum lscrn_colors {LSCRN_BLACK, LSCRN_RED, LSCRN_GREEN, LSCRN_YELLOW,
   LSCRN_BLUE, LSCRN_PURPLE, LSCRN_AQUA, LSCRN_GRAY, LSCRN_NOCHANGE};

/*  The following enum is used to control behavior of Ctrl-C */
   enum getachar_ctrlc {
	   GETACHAR_CTRLC_CANCEL, /* Cancel the program */
	   GETACHAR_CTRLC_RAISE,  /* Raise any previouly set handler */
	   GETACHAR_CTRLC_RETURN  /* Return ^C character to caller */
   };

/*
 *  From hscrn
 */
   
void geta (int y, int x, char *s);
char* geta_return(int y, int x);
void getp (int y, int x, char *s);
char* getp_return(int y, int x);
void getn (int y, int x, int *INPUT);
int getn_return(int y, int x);
void getl (int y, int x, int *INPUT);
void getl_no_default (int y, int x, int *INPUT);
int getl_return(int y, int x);
void getf (int y, int x, double *INPUT, int decimals);
void getf_return2(int y, int x, double *OUTPUT);
void saya (int y, int x, const char *s);
void sayn (int y, int x, int n);
void sayl (int y, int x, int l);
void pysayf (int y, int x, double *INPUT, int decimals);
int rd (void);
int rd_one(void);
void set_field (int y, int x);
void erase (void);
void clear (void);

/*
 *  From lscrn
 */

void clrscrn ();
void set_color (int foreground, int background);
void normal (void);
void reverse ();
void blink ();
void rev_blink ();
void erase_line (int line_number);
int screen_size (void);
int screen_size_no_clear (void);
int line_size (void);
void lputs (const char *s);
void lputc (int c);
void gotoxy(int x, int y);
void save_screen(void);
void restore_screen(void);

/*
 *  From getachar
 */

int getachar ();
int getachar_nowait ();
int getachar_set_mode();
void reset_terminal ();

/*
 *  From title
 */

void set_title (const char* title);
void reset_title ();
void set_title_no_reset(const char* title);

