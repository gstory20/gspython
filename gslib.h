/***************************************************************************/
/*                                                                         */
/*  Program name:  gslib.h                                                 */
/*                                                                         */
/*  05/09/1991 - Glenn Story                                               */
/*                                                                         */
/*  This file contains function prototypes for my library routines         */
/*                                                                         */
/*  Modification History:                                                  */
/*                                                                         */
/*      8/13/1992, GS, Add const where appropriate; expand descriptions
       09Jun94, G. Story, add getfile
        12/31/1994, GS, Add procedures needed by FCSCAN
        1/12/95, make getword a separate header file
        {@change_history...@}
                                                                           */
/***************************************************************************/

#ifdef __cplusplus
extern "C"
{  /*  C definitions  */
#endif

#ifdef IBM
int absdir (char *dir);
/*  convert a directory path to absolute form */
#endif

char *append_text (char *old_string, const char* new_string);
/*  Text block handling:  Add a line from input to a text block            */

void blink (void);
/*  Low-level screen handling:  turn on blinking characters                */

void box (int x1, int y1, int x2, int y2);
/*  High-level screen handling:  Draw a box on the screen                  */

int c_time (const char * in_time);
/* convert a time of day to internal format                                */

int c_time_2 (const char * in_time);
/* convert a time interval to internal format                              */

int c_time_3 (const char * in_time);
/* convert a time interval to internal format (including nn hours)         */

int  cdate (char *date, char past);
/*  date handling:  Convert date relative to to today                      */

int  cdate_1 (char *date, char past);
/*  Date handling:  Covert a [fixed +] relative date                       */

int  cdate_2 (char *date, char past, const char *relative);
/*  Date handling:  Convert date relative to the specified date            */

int  chlower(int c);
/*  convert a char to lower case                                           */

int  chupper(int c);
/*  Convert a char to upper case                                           */

void clear (void);
/*  High-level screen handling:  Clear the screen                          */

void clipboard_copy(const char* text);
/*  Copy text to the clipboard */

void clrscrn (void);
/*  Low-level screen handling:  Clear the screen                           */

int cyear (int year);
/*  convert year to proper 4-digit format                                  */

int  datecmp (const char *date_1, const char *date_2);
/*  Compare two dates                                                      */

void dirpath (char *path);
/*  Fixes up directory path                                                */

#ifndef MAC
void erase (void);
/*  erase the screen                                                       */
#endif

void erase_line (int line_number);
/*  Low-level screen handling:  Erase one screen line                      */

void expand_text (const char * text, int line_count);
/*  break text into multiple lines                                         */

int  file_exists (const char *file_name);
/*  TRUE if file exists (is read-accessable)                               */

int  file_writable (const char *file_name);
/*  TRUE if file is writeable                                              */

void geta (int y, int x, char *s);
/*  High-level screen handling:  Get a string from the screen              */

void getf (int y, int x, double *n, int decimals);
/*  Hieh-level screen handling:  get a float from the screen               */

int getfile (const char *program, const char *file, char direction,
	     const char *mask, char *response);
/*  Get a file name from the user                                          */

int py_getfile (const char *program, const char *file, char direction,
	     const char *mask, char *initial_value);
/*  Get a file name from the user                                          */

char* getfile_result(void);

int getfilel (char type, char *filename);
/*  low-level interface to locate a file                                    */

char* get_hostname (void);
/* To return a string with the host name */

char* get_parent (void);
/*  Return the command name of the parent */

void getl (int y, int x, int *l);
/*  High-level screen handling:  Get a boolean from the screen             */

void getn (int y, int x, int *n);
/*  High-level screen handling:  Get a number from the screen              */

void getp (int y, int x, char *s);
/*  High-level screen handling:  Get a hidden string from the screen       */

void get_a_date (char *date);
/*  Get today's date as mmm dd, yyyy                                       */

void get_holidays (void);
/*  Get holidays from a file to cdate's array                              */

void get_n_date (char *date);
/*  Get today's date as mm/dd/yy                                           */

void get_one (int y, int x, char *c, const char *s);
/*  High-level screen handling:  Get one of selected set of chars.         */

void get_one_default (int y, int x, char *c, const char *s, int default_value);
/*  High-level screen handling:  Get one of selected set of chars.         */

int  get_text (int starting_line, int line_count, char *text);
/*  Text-block handling:  get text from the screen                         */

void get_text1 (int starting_line, int line_count, char *text);
/*  Text-block handling:  Break up text and do geta's for text block       */

void get_text2 (int line_count, char *text);
/*  Text-block handling:  Do completion on get text                        */

void get_time (char *time);
/*  Get the current time                                                   */

#ifdef IBM
int get_vol (int drive, char *label);
/*  Get the volume label for a specified drive */
#endif

void gotoxy (int x, int y);
/* Low-level screen handling:  direct cursor addressing                    */

int  helpsub (const char *text_name, const char *index_name,
    const char *title, const char *initial_line);
/*  display help text                                                      */

int isa_holiday (long julian, int want_a_workday);
/*  determine if a day is a holiday                                        */

void jdate (long work_jul, int *work_day, int *work_month, int *work_year);
/*   Converts a julian date to a gregorian date                            */

long jday (int day, int month, int year);
/*  convert gregorian date to julian date                                  */

void list_add_ascending (void *list_head, void *list_entry, const char *descr);
/*  Add an entry in ascending order within the list */

void list_add_head (void *list_head, void *list_entry, const char *descr);
/*  Add an entry to the beginning of the list */

void list_add_tail (void *list_head, void *list_entry, const char *descr);
/*  Add an entry to the end of the list */

void *list_find_ascending (void *list_head, const char *descr);
/*  find an entry in ascending order within the list */

void * list_pick (const char *title, const char *noun, const char *verb,
		  const void *list_head);
/*  pick an entry from a list */

void * list_pick_ex (const char *title, const char *noun, const char *verb,
		     const void *list_head, const char* menu,
		     const char* commands,
		     int (*callback) (void* list_entry, int key, void* state),
		     void* state, int return_is_edit, int* last_key,
		     int top_rec, int auto_return, const char* instruction,
		     const char* filter_string);
/*  pick an entry from a list */

void list_purge (void *list_head);
/*  purge all entries from a list */

void list_remove_entry (void *list_entry);
/*  remove an entry from a list */

void lputc (int character);
/*  low level screen I/O -- write a character */

void lputs (const char *string);
/*  low level screen I/O -- write a string */

void make_name (const char *path, const char *name, char *result);
/*  Add environmental path information to filename                         */

extern int make_name_error;

int max_day_of_month (int month, int year);
/*  To return the last day of a specified month */

#ifdef IBM
int mount (int drive, char *vol_id);
/*  Verify that the specified floppy (or CD-ROM) is mounted */
#endif

int my_system (const char *command);
/*  My interface to system() function                                      */

void newpage (FILE *printer_file);
/*  Printer support:  Skip printer to new page                             */

#ifndef MAC
void normal (void);
/*  Make screen attributes normal                                          */
#endif

int  patcmp (const char *string, const char *pattern);
/*  Patern compare                                                         */

void printwrap (FILE* file, int requested_size, const char* format, ...);
/*  Print to a file and wrap lines */

void patcmp_help (const char *program, const char *field,
                  char *result_pattern);
/* Give user help in selecting a search pattern */

void p_time (int minutes, char *time_string);
/*  Convert time to external 24-hour format                                */

void p_time12 (int minutes, char *time_string);
/*  Convert time to external 12-hour format                                */

void queueachar(int character);
/*  Queue a character to be "read" by getachar */

int  rd (void);
/*  High-level screen handling:  Read specified fields                     */

void release_lock (void);
/*  Release lock obtained by file_updatable ()                             */

void release_lock2 (void* cookie);
/*  Release lock obtained by file_updatable ()                             */

void reset_title (void);
/*  Reset the window title (to hostname) */

void reverse (void);
/*  Low-level screen handling:  Reverse video                              */

void rev_blink (void);
/*  low-level screen handling:  Reverse blinking characters                */

void rpad (char *string, int max_length);
/*  Pad a string to the right                                              */

void saya (int y, int x, const char *s);
/*  High-level screen handling:  Display a string on the screen            */

char* saybytes(unsigned int bytes);
/*  Convert bytes to MB, TB, etc.*/

void sayf (int y, int x, double n, int decimals);
/*  High-level screen handling:  Display a number on the screen            */

void sayl (int y, int x, int l);
/*  Hign-level screen handling:  Display a boolean on the screen           */

void sayn (int y, int x, int n);
/*  High-level screen handling:  Display a number on the screen            */

void say_text (int starting_line, int line_count, const char *text);
/*  Text-block handling:  Write text to the screen                         */

int screen_size (void);
/*  Low-level screen handling:  Get the number of lines in the screen      */

void set_field (int y, int x);
/*  High-level screen handling:  set current field to match co-ordinates   */

void set_holidays (long *my_holidays);
/*  Set holidays into cdate data fields                                    */

void set_security (const char *old_name, const char *new_name, FILE *new_fd);
/*  Set file security of new file to match old file                        */

void set_title (const char* title);
/*  Set the window title */

void set_title_no_reset (const char* title);
/*  Set the window title without automatic reset on exit */

void stripnl (char *string);
/*  Strip a new line from a string                                         */

#ifndef IBM
void strupr (char *string);
/*  translate a string to uppercase */

void strlwr (char *string);
/*  translate a string to lower case */
#endif

void time_stamp (char *stamp);
/*  Get today's date and time in the form yy/mm/dd hh:mm:ss                */

char* toroman (int number);
/* To convert a number to roman numerals */

void trim (char *s);
/*  Trim trailing blanks;  min. size 1 char.                               */

int update_check (const char *filename);
/*  Displays warning screen if file is not updateable                      */

void* update_check2 (const char *filename);
/*  Displays warning screen if file is not updateable                      */

int weekday (long julian, char *day);
/*  Computes the day of the week from the julian date                       */

void write_text (FILE *out_file, int record_type, const char *record);
/*  text-block handling:  Write a text block to a file                     */

void ztrim (char *s);
/*  Trim trailing blanks;  min. size 0 char.                               */

#ifdef __cplusplus
}  /*  C definitions  */
#endif
