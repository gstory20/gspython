%module pydbase
   %include typemaps.i
   %{
#include "dbase3.h"
const char* py_dbget_memo(struct dbcb* cur_dbcb, char* field);
	%}


void save_command_line(const char* command_line);
/*  Save command line for lock files */

   struct dbcb *dbopen2 (char *dbf_file, int rw_flag);
/* open the data base */


   long dbgetnumrecs (struct dbcb *cur_dbcb);
/*  Return the number of records in the database */


   int dbgetloc (struct dbcb *cur_dbcb);
/*  Get the current record number for future use by dbgoto */


   int dbgoto (struct dbcb *cur_dbcb, int num);
/*  goto the specified record */


   int dbjoin (struct dbcb *dbcb1, char *field1, char *index_field,
	       struct dbcb *dbcb2, char *field2);
/*  Get a record from a second data base relative to the first */


   int dblocatec (struct dbcb *cur_dbcb, char *field, char *s_value);
/*  locate a particular value */


   int dblocated (struct dbcb *cur_dbcb, char *field, char *s_value);
/*  locate a particular value */


   int dblocaten (struct dbcb* cur_dbcb, char *field, int s_value);
/*  locate a particular value */


   int dblocatef (struct dbcb* cur_dbcb, char *field, double s_value);
/*  locate a particular value */


   int dbputc (struct dbcb *cur_dbcb, char *field, char *value);
/*  put a field from current record */


   int  dbputd (struct dbcb *cur_dbcb, char *field, char *value);
/*  put a field from current record */


   int dbputn (struct dbcb *cur_dbcb, char *field, int value);
/*  put a field from current record */


   int dbputf (struct dbcb *cur_dbcb, char *field, double value);
/*  put a field from current record */


   int dbputl (struct dbcb *cur_dbcb, char *field, int value);
/*  put a field from current record */


   int dbflush (struct dbcb *cur_dbcb);
/* write dirty buffer and update header to disk */


   int dbrefresh (struct dbcb *cur_dbcb);
/*  read updated header from disk */


   int dbclose (struct dbcb *cur_dbcb);
/* close the data base */


   int dbfield (struct dbcb *cur_dbcb, const char *field);
/* find the specified field */


   int dbupdate (struct dbcb *cur_dbcb);
/* write buffer if it has been updated */


   int dbappend (struct dbcb *cur_dbcb);
/*  dbappend - add a new record to the end of the file */


   void dberror (char *string);
/*  reports error codes and aborts */


   void dberror_return (char *string);
/*  reports error codes and return to caller */


   int dberror_to_text (dbase_error db_err, char *text, int text_size);
/*  convert db error code to text */


   int dbmark (struct dbcb *cur_dbcb, char mark);
/* implements dbdelete and dbrecall */


   int dbezlocc (struct dbcb* cur_dbcb, char *field, char *s_value,
		 start_values start_from);
/*  easy locate */


   int dbezlocd (struct dbcb *cur_dbcb, char *field, char *s_value,
		 start_values start_from);
/*  easy locate */


   int dbezlocl (struct dbcb *cur_dbcb, char *field, int s_value,
		 start_values start_from);
/*  easy locate */


   int dbezlocn (struct dbcb *cur_dbcb, char *field, int s_value,
		 start_values start_from);
/*  easy locate */


   int dbezlocf (struct dbcb *cur_dbcb, char *field, double s_value,
		 start_values start_from);
/*  easy locate */


   int dbskip (struct dbcb *cur_dbcb, int value);


   int dbrewind (struct dbcb *cur_dbcb);
/* skip to specified record */


   void dbreturn_deleted (struct dbcb* cur_dbcb);
/*  Tell routines to return records marked for deletion */


   void dbskip_deleted (struct dbcb* cur_dbcb);
/*  Tell routines to skip over records marked for deletion */


   int dbcheckrec (struct dbcb* cur_dbcb, int value);
/*  Routine used by db_deleted macro */


   %apply int *OUTPUT {int *value}
   int dbgetn (struct dbcb *cur_dbcb, char *field, int *value);
/*  get a field from current record */


   %apply double *OUTPUT {double *value}
   int dbgetf (struct dbcb *cur_dbcb, char *field, double *value);
/*  get a field from current record */


   %apply int *OUTPUT {int *value}
   int dbgetl (struct dbcb *cur_dbcb, char *field, int *value);
/*  get a field from current record */


   char* dbget_record_buffer (struct dbcb* cur_dbcb);
/*  Return address of data buffer */


   int dbget_record_size (struct dbcb* cur_dbcb);
/*  Return size of data record */


   int dbget_memo (struct dbcb* cur_dbcb, char* field, char* buffer, int size, int* actual_size);
/*  Get memo field from dbt file */


   int dbput_memo (struct dbcb* cur_dbcb, char* field, char* buffer);
/*  Put memo field to dbt file */

   char* pygetfield(struct dbcb *cur_dbcb, int i);

int dbase3_version ();

int get_dberror ();

char* pydbgetc(struct dbcb *cur_dbcb, char* field);
char* pydbgetd(struct dbcb *cur_dbcb, char* field);
char* pydberrortext(struct dbcb *cur_dbcb, const char* verb);
char* py_dbget_memo(struct dbcb* cur_dbcb, char* field);

