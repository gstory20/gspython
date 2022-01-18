/*
 *  dbase3.c
 *
 *  Glenn Story - 11/14/86
 *
 *  This is the main file for creating dBASE subroutines in a dBASE II
 *  environment
 */

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#ifdef WIN32
#include <windows.h>
#undef ERROR
#endif

#include "gs.h"
#define DB_INTERNAL
#include "dbase3.h"
#include <fcntl.h>

#ifndef EXPORT_DLL
#ifdef XBASE_EXPORTS
#define EXPORT_DLL __declspec(dllexport)
#else
#define EXPORT_DLL
#endif
#endif

/*
    This module contains the subroutines necessary to access dBASE II
    files from a C language program.

    18:38:16  8/10/1985 - IBM version

    14:42:28  7/4/1986 - added dbappend (a la "append blank")

    17:32:51  8/17/1986 - Fix dbappend to always add 512-bytes segments

    11/14/86 - This becomes a giant #include file
             - You must compile dbase2.c or dbase3.c

    08/26/87 - Add support for date field

    09/29/87 - Convert to Microsoft C

    10/4/87 - Change filename rules;  add dbdelete and dbrecall;

    5/7/88 - Add dbezloc.

    8/12/89:  1.  Imbedded original dbase.c code directly instead
                  of including it.
              2.  Broke dbget and dbput into data-type depenedent
                  routines
              3.  Removed conditional code for dbase2 and QC

    8/17/89:  Tandem version

    7/17/91:  Handle variable date input formats

    7/13/92:  Allow converting numeric fields to floats

    7/27/93:  close files on open errors

    6/1/95:  Don't append .dbf unless necessary

    6/2/95:  Allow relative dates

    4/21/96:  Add dbjoin, dbskip, dbrewind

    3/19/97:  Version 2.0 - Add multipartition support

    7/30/97:  Version 2.2 - Save real file name in DBCB

    5/14/98:  Version 2.3 - Y2K

    8/22/02:  Version 2.4 - Add support for memo fields

    5/31/11:  Version 2.5 - Check validity of dbcb

    4/27/13:  Version 2.6 - Lock files opened for update

    6/12/13:  Version 2.6.1 - Fix NULL-pointer dereference in cvt_date

    10/13/13: Version 2.7   - C99 compatibility

    9/24/15: Version 2.8    - All general goto negative number == from
			      end of file

    3/4/18: Version 2.8.1   - Fix SIGSEGV in py_dbdet_memo

    6/14/18: Version 2.8.2  - Fix problem with .RI files on Mac OS

    9/3/18:  Version 2.9    - Issue an error when text field and no
                              .DBF file

    2/27/19: Version 2.9.1  - Allow both .dbt and .DBT for memo file

    12/21/19: Version 2.10  - Save .dbt name in dbcb (for pbase)

    11/30/20: Version 2.11  - Allow "?" for locial value (tri-state)

    12/1/20:  Version 2.11.1 - Treat " " as false in logical values
			       (get only)
*/

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#define VERSION "2.11.1"

#ifdef MSC
#include <io.h>
#else
#include <errno.h>
#endif

#ifndef TANDEM
#ifndef _AFXDLL
/* extern long lseek (); */
#endif
#endif

EXPORT_DLL
dbase_error db_error;                   /*  error code */
char cur_field_type;                    /*  current field type */
char *cur_loc;                          /*  location of current field */
char* db_error_file = NULL;

static int cvt_date (char *i_date, char *o_date, char *prev_date);
static void set_error_file (const char* error_file);

static char* py_memo_buffer = NULL;
static int py_memo_size = 0;

EXPORT_DLL
int get_dberror(void)
{
	return db_error;
}

EXPORT_DLL
   char* pydbgetc(struct dbcb *cur_dbcb, const char* field)
{
	static char value[1024];
	int rc;

	rc = dbgetc (cur_dbcb, field, value);

	if (rc == 0)
		return value;
	else
		return NULL;
}

EXPORT_DLL
   char* pydbgetd(struct dbcb *cur_dbcb, char* field)
{
	static char value[1024];
	int rc;

	rc = dbgetd (cur_dbcb, field, value);

	if (rc == 0)
		return value;
	else
		return NULL;
}

EXPORT_DLL
char* pydberrortext(struct dbcb *cur_dbcb, const char* verb)
{
	static char message[1024];
	char text[50];
	int os = FALSE;

	if (db_error_file == NULL)
		db_error_file = strdup("<none>");

	os = dberror_to_text (db_error, text, 50);

	sprintf (message, "Error in xBASE access routine:  %s\n"
		 "xBASE error code %d:  %s\nFilename: %s\n", verb,
		 db_error, text, db_error_file);

	if (os)
	{
		sprintf (text, "OS error code %d:  %s", errno, strerror (errno));
		strcat (message, text);
	}

	return message;
}

EXPORT_DLL
char* pygetfield (struct dbcb *cur_dbcb, int i)
{
	char type;

	static char field_string[1024];

	if (cur_dbcb->fields[i].f_name[0] == 0x0d)
		return NULL;

	type = cur_dbcb->fields[i].f_type;

	if (cur_dbcb->fields[i].f_decimals != 0)
		type = 'F';

	sprintf (field_string, "%s:%c:%d:%d",
		 cur_dbcb->fields[i].f_name,
		 type,
		 cur_dbcb->fields[i].f_size,
		 cur_dbcb->fields[i].f_decimals);

	return field_string;
}


/*  dbopen2 returns NULL on error */
EXPORT_DLL
   struct dbcb *dbopen2 (char *dbf_file, int rw_flag)      /* open the data base */
{
	struct dbcb* new_dbcb = dbopen(dbf_file, rw_flag);
	if (new_dbcb == (struct dbcb*) -1)
		return NULL;
	return new_dbcb;
}


/*  dbopen returns -1 on error */
   EXPORT_DLL
   struct dbcb *dbopen (char *dbf_file, int rw_flag)      /* open the data base */

{
        struct dbcb *cur_dbcb;

#ifdef PARTITIONS
        struct partition_table* partitions;
#endif
        char file_id[256];
        int fd;
        int i;
        int total_record_count;
        int j;
        char* dot;
        struct dbcb* ri_file;
        char ri_field[11];
	int open_flags;

        /*  open the file */

#ifdef TANDEM
      strcpy (file_id, dbf_file);
      open_flags = (rw_flag == 2) ? 0 : 1;
#else
      open_flags = rw_flag;
#endif


      if (file_exists (dbf_file))
        strcpy (file_id, dbf_file);
      else
      {  /*  Add explicit .dbf extension  */
          sprintf (file_id, "%s.dbf", dbf_file);
          if (!file_exists (file_id))
          {  /*  See if it's partitioned  */
#ifdef PARTITIONS
#ifdef TANDEM
              sprintf (file_id, "%s00", dbf_file);
#else
              sprintf (file_id, "%s.d00", dbf_file);
#endif
              if (file_exists (file_id))
              {  /*  Open a multi-partition file  */

		  /*                                                       */
                  /*  First determine how many partitions there are        */
                  /*                                                       */

                  for (i = 1; TRUE; ++i)
                  {
#ifdef TANDEM
                      sprintf (file_id, "%s%02d", dbf_file, i);
#else
                      sprintf (file_id, "%s.d%02d", dbf_file, i);
#endif
                      if (!file_exists (file_id))
                          break;
                  }

                  partitions = (struct partition_table*) malloc (
                      sizeof (struct partition_table)
                      + (sizeof (struct partition_entry) * (max (i - 2, 0))));
                  if (partitions == NULL)
                  {
                      db_error = DE_NOMEM_DBCB;
		      set_error_file (dbf_file);
                      return ((struct dbcb *) ERROR);
                  }

                  partitions->eye_catcher = MP_EYECATCHER;
                  partitions->partition_count = i;
                  partitions->current_entry = 0;
                  total_record_count = 0;

                  for (i = 0; i < partitions->partition_count; ++i)
                  {  /*  Loop for each partitions  */
#ifdef TANDEM
                      sprintf (file_id, "%s%02d", dbf_file, i);
#else
                      sprintf (file_id, "%s.d%02d", dbf_file, i);
#endif
                      partitions->partition[i].dbcb
                              = dbopen (file_id, rw_flag);
                      if (partitions->partition[i].dbcb
                          == (struct dbcb*) ERROR)
                      {
                          for (j = 0; j < i; ++j)
                              dbclose (partitions->partition[j].dbcb);

                          free (partitions);
                          return (struct dbcb*) ERROR;
                      }

                      partitions->partition[i].first_record
                              = total_record_count + 1;

                      total_record_count
                              += (int) partitions->partition[i].dbcb->num_recs;
                  }  /*  Loop for each partitions  */

                  return (struct dbcb*) partitions;
              }  /*  Open a multi-partition file  */
#endif
	  }  /*  See if it's partitioned  */
      }  /*  Add explicit .dbf extension  */

#ifndef UNIX
        open_flags |= O_BINARY;  /*  turn on binary flag */
#endif

        if ((fd = open (file_id, open_flags)) == ERROR)
        {
                db_error = DE_OPENFAIL;
		set_error_file(dbf_file);
                return ((struct dbcb *) ERROR);
        }

        /* locate a dbcb */

        if ((cur_dbcb = (struct dbcb *) malloc (MAX_DBCB_SIZE)) == NULL)
        {
                db_error = DE_NOMEM_DBCB;
		set_error_file(dbf_file);
                close (fd);
                return ((struct dbcb *) ERROR);
        }

        cur_dbcb->magic_number = MAGIC_NUMBER;
        cur_dbcb->db_fd = (short) fd;
	cur_dbcb->open_flags = open_flags;

        /*  set the status flag  */

        cur_dbcb->flag = OPEN;
        if ((rw_flag & 2) == 2)
	{
		cur_dbcb->lock_structure = update_check2(dbf_file);
		if (cur_dbcb->lock_structure != NULL)
			cur_dbcb->flag |= READ_WRITE;
	}
	else
		cur_dbcb->lock_structure = NULL;

        /*
         *  Save the file name in the DBCB
         */

        cur_dbcb->real_file_name = malloc (strlen (file_id) + 1);
        if (cur_dbcb->real_file_name != NULL)
            strcpy (cur_dbcb->real_file_name, file_id);

        /*  Initialize the dbcb so it doesn't look like partition table */

        memset (cur_dbcb, 0, 4);
	cur_dbcb->magic_number = MAGIC_NUMBER;

        /*  Get the file header into the DBCB */

        if (dbrefresh (cur_dbcb) == ERROR)
        {
             if (cur_dbcb->real_file_name != NULL)
                free (cur_dbcb->real_file_name);

            free (cur_dbcb);

            return ((struct dbcb *) ERROR);
        }

        cur_dbcb->ndx_cb = NULL;

        /* now get a buffer for the data */

        if ((cur_dbcb->data_buf = malloc (cur_dbcb->rec_size + 513)) == NULL)
        {
            if (cur_dbcb->real_file_name != NULL)
                free (cur_dbcb->real_file_name);

            free (cur_dbcb);

            db_error = DE_NOMEM_BUF;
	    set_error_file(dbf_file);
            close (fd);
            return ((struct dbcb *) ERROR);
        }

        /*  get the first record */

        cur_dbcb->cur_rec = 0;

        if (dbgoto (cur_dbcb, 1) == ERROR)
           if (db_error != DE_EMPTY)
           {
               if (cur_dbcb->real_file_name != NULL)
                   free (cur_dbcb->real_file_name);

               free (cur_dbcb);

               close (fd);
               return ((struct dbcb *) ERROR);
           }

        /*                                                                 */
        /*  Now look for a referencial integrity file                      */
        /*                                                                 */

        strcpy (file_id, cur_dbcb->real_file_name);

        dot = strrchr (file_id, '.');

        if (dot != NULL)
            *dot = 0;

        strcat (file_id, ".RI");

        if ((stricmp (file_id, cur_dbcb->real_file_name) == 0)
        || ((ri_file = dbopen (file_id, 0)) == (struct dbcb*) ERROR))
            cur_dbcb->ref_fields = NULL;
        else
        {  /*  Process the ri file  */
            /*                                                             */
            /*  count the active fields                                    */
            /*                                                             */

            dbskip_deleted (ri_file);

            for (i = 0;
                 (i < MAX_FIELDS) && (cur_dbcb->fields[i].f_name[0] != 0x0d);
                 ++i)
                /*  null body */ ;

            /*                                                             */
            /*  Allocate the ri structure                                  */
            /*                                                             */

            cur_dbcb->ref_fields
       = (struct db_references*) malloc (sizeof (struct db_references) * i);

            if (cur_dbcb->ref_fields == NULL)
            {
                db_error = DE_NOMEM_REF;
		set_error_file(dbf_file);
                return ((struct dbcb *) ERROR);
            }

            /*                                                             */
            /*  Null out the array                                         */
            /*                                                             */

            for (j = 0; j < i; ++j)
                cur_dbcb->ref_fields[j].r_file = NULL;

            do
            {  /*  Loop through ri file  */
                if (dbgetc (ri_file, "B_FIELD", ri_field) == ERROR)
                {
                    db_error = DE_RI_BAD_FIELD;
		    set_error_file(dbf_file);
                    return ((struct dbcb *) ERROR);
                }

                ztrim (ri_field);

                if ((i = dbfield (cur_dbcb,  ri_field)) == ERROR)
                {
                    db_error = DE_RI_BAD_FIELD;
		    set_error_file(dbf_file);
                    return ((struct dbcb *) ERROR);
                }

                if (dbgetc (ri_file, "FILE", file_id) == ERROR)
                {
                    db_error = DE_RI_OPEN_FAIL;
		    set_error_file(dbf_file);
                    return ((struct dbcb *) ERROR);
                }

		ztrim(file_id);
                cur_dbcb->ref_fields[i].r_file = dbopen (file_id, 0);
                if (cur_dbcb->ref_fields[i].r_file == (struct dbcb*) ERROR)
                {
                    /*  Unqualified filename failed to prepend path of main file*/

                    char full_id[512];
                    char* slash;
                    strcpy (full_id, cur_dbcb->real_file_name);
                    slash = strrchr (full_id, '\\');
                    if (slash == NULL)
                        slash = strrchr (full_id, '/');

                    if (slash != NULL)
                    {
                        slash[1] = 0;
                        strcat (full_id, file_id);
                        cur_dbcb->ref_fields[i].r_file = dbopen (file_id, 0);
                    }

                    if (cur_dbcb->ref_fields[i].r_file == (struct dbcb*) ERROR)
                    {
                        db_error = DE_RI_BAD_FIELD;
			set_error_file(dbf_file);
                        return ((struct dbcb *) ERROR);
                    }
                }

                dbskip_deleted (cur_dbcb->ref_fields[i].r_file);

                if (dbgetc (ri_file, "R_FIELD", ri_field) == ERROR)
                {
                    db_error = DE_RI_BAD_FIELD;
		    set_error_file(dbf_file);
                    return ((struct dbcb *) ERROR);
                }

                ztrim (ri_field);

                if ((cur_dbcb->ref_fields[i].r_field
                     = dbfield (cur_dbcb->ref_fields[i].r_file,  ri_field))
                    == ERROR)
                {
                    db_error = DE_RI_BAD_FIELD;
		    set_error_file(dbf_file);
                    return ((struct dbcb *) ERROR);
                }
            }  /*  Loop through ri file  */
            while (dbskip (ri_file, 1) != ERROR);

	    if (db_error != DE_NOTFOUND)
	    {
		    set_error_file(dbf_file);
                return ((struct dbcb *) ERROR);
	    }

            dbclose (ri_file);

        }  /*  Process the ri file  */

        /*                                                                 */
        /*  Now look for memo file                                         */
        /*                                                                 */

	for (i = 0;
	     (i < MAX_FIELDS) && (cur_dbcb->fields[i].f_name[0] != 0x0d);
	     ++i)
	{
		cur_dbcb->memo_file = -1;
		cur_dbcb->memo_file_name = NULL;

		if (cur_dbcb->fields[i].f_type == 'M')
		{
			/*  If we found a memo field, we need a memo
			 *  file
			 */
			strcpy (file_id, cur_dbcb->real_file_name);

			if (dot != NULL)
				*dot = 0;


			strcat (file_id, ".dbt");

			cur_dbcb->memo_file = open (file_id, cur_dbcb->open_flags);
			if (cur_dbcb->memo_file == -1)
			{
				/*  .dbt failed;  try .DBT  */
				if (dot != NULL)
					*dot = 0;


				strcat (file_id, ".DBT");

				cur_dbcb->memo_file = open (file_id, cur_dbcb->open_flags);
				if (cur_dbcb->memo_file == -1)
				{
					db_error = DE_NO_DBT;
					set_error_file(cur_dbcb->real_file_name);
					return ((struct dbcb *) ERROR);
				}
			}

			cur_dbcb->memo_file_name = strdup (file_id);

			/*  Once we find one memo field and open the
			 *  memo file, we don't need to loop any more
			 */
			break;
		}
	}

        /*  all's well that ends well */

        return (cur_dbcb);
}

static int check_dbcb (struct dbcb *cur_dbcb)
{
#ifdef PARTITIONS
  	struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
	if (cur_dbcb == NULL)
	{
		db_error = DE_BADDBCB;
		set_error_file ("<NULL>");
		return (ERROR);
	}

#ifdef PARTITIONS
	if (partitions->eye_catcher == MP_EYECATCHER)
		return 0;
#endif

	if (cur_dbcb->magic_number == (char) CLOSED_MAGIC_NUMBER)
	{
		db_error = DE_CLOSED;
		set_error_file ("<CLOSED>");
		return (ERROR);
	}

	if ((cur_dbcb->magic_number & 7) != MAGIC_NUMBER)
	{
		db_error = DE_BADDBCB;
		set_error_file ("<BAD>");
		return (ERROR);
	}

	return 0;
}

EXPORT_DLL
int dbrefresh (struct dbcb *cur_dbcb) /* get header from disk */
{
#ifdef BIGENDIAN
    union
    {
        char c[4];
        int i;
        short s;
    } swapper;
#endif
    int size_read;
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        for (i = 0; i < partitions->partition_count; ++i)
        {  /*  Loop through partitions  */
            if (dbrefresh (partitions->partition[i].dbcb) == ERROR)
                return ERROR;
        }  /*  Loop through partitions  */

        partitions->current_entry = 0;
        return 0;
    }  /*  Multiple partitions  */
#endif

    if (cur_dbcb->flag & CREATE_IN_PROGRESS)
    {
        db_error = DE_CREATMODE;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
        }

    if ((cur_dbcb->flag & UPDATE_HEADER) != 0)
    {
        db_error = DE_DIRTY;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

    if (lseek (cur_dbcb->db_fd, 0L, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_OPEN;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

    size_read = read (cur_dbcb->db_fd, (char *) cur_dbcb, FIXED_HEADER);

    if (size_read == ERROR)
    {
        db_error = DE_READFAIL_DBCB;
	set_error_file (cur_dbcb->real_file_name);
        close (cur_dbcb->db_fd);
        return (ERROR);
    }

    if (size_read != FIXED_HEADER)
    {
        db_error = DE_TOOSMALL;
	set_error_file (cur_dbcb->real_file_name);
        close (cur_dbcb->db_fd);
        return ERROR;
    }

    if ((cur_dbcb->magic_number & 7) != MAGIC_NUMBER)
    {
	    db_error = DE_BADFILE;
	    set_error_file (cur_dbcb->real_file_name);
	    return (ERROR);
    }

#ifdef BIGENDIAN
    swapper.c[0] = cur_dbcb->num_recs_swapped[3];
    swapper.c[1] = cur_dbcb->num_recs_swapped[2];
    swapper.c[2] = cur_dbcb->num_recs_swapped[1];
    swapper.c[3] = cur_dbcb->num_recs_swapped[0];
    cur_dbcb->num_recs = swapper.i;

    swapper.c[0] = cur_dbcb->r1_offset_swapped[1];
    swapper.c[1] = cur_dbcb->r1_offset_swapped[0];
    cur_dbcb->r1_offset = swapper.s;

    swapper.c[0] = cur_dbcb->rec_size_swapped[1];
    swapper.c[1] = cur_dbcb->rec_size_swapped[0];
    cur_dbcb->rec_size = swapper.s;
#endif

    if (DBCB_SIZE > (&cur_dbcb->flag - (char*) cur_dbcb))
    {
	    db_error = DE_READFAIL_DBCB;
	    set_error_file (cur_dbcb->real_file_name);
	    close (cur_dbcb->db_fd);
	    return (ERROR);
    }

    size_read = read (cur_dbcb->db_fd, (char *) cur_dbcb + FIXED_HEADER,
		      DBCB_SIZE - FIXED_HEADER);

    if (size_read == ERROR)
    {
	    db_error = -12;
	    db_error = DE_READFAIL_DBCB;
	    set_error_file (cur_dbcb->real_file_name);
	    close (cur_dbcb->db_fd);
	    return (ERROR);
    }

    if (size_read != (DBCB_SIZE - FIXED_HEADER))
    {
        db_error = DE_TOOSMALL;
	set_error_file (cur_dbcb->real_file_name);
        close (cur_dbcb->db_fd);
        return ERROR;
    }

    if ((cur_dbcb->magic_number & 7) != MAGIC_NUMBER)
    {
        db_error = DE_BADFILE;
	set_error_file (cur_dbcb->real_file_name);
        return (ERROR);
    }

    /*
     *  Force the next record to be read from disk
     */


    cur_dbcb->cur_rec = 0;

    return (0);

}

EXPORT_DLL
int dbgoto (struct dbcb *cur_dbcb, int num)    /*  goto the specified record */

{
    char *c;
    unsigned long my_num;
    int bytes_read;
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    int result;

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

    if (dbupdate (cur_dbcb) == ERROR)
        if (db_error != DE_EMPTY)
	{
	    set_error_file ( cur_dbcb->real_file_name);
            return (ERROR);
	}

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        if (num == -1)
        {  /*  Last record in file  */
            partitions->current_entry = partitions->partition_count - 1;

            while (partitions->current_entry >= 0)
            {
                if (dbgoto (
                        partitions->partition[partitions->current_entry].dbcb,
                            -1) == ERROR)
                    if (db_error == DE_EMPTY)
                    {
                        partitions->current_entry--;
                    }
		    else
		    {
		            set_error_file (cur_dbcb->real_file_name);
			    return ERROR;
		    }
                else
                    return 0;

            }
        }  /*  Last record in file  */

        for (i = 0; i < partitions->partition_count; ++i)
        {  /*  Loop through partitions  */
            if (num < partitions->partition[i].first_record)
                break;
        }  /*  Loop through partitions  */

        partitions->current_entry = i - 1;
        result = dbgoto (
            partitions->partition[partitions->current_entry].dbcb,
   1+ num - partitions->partition[partitions->current_entry].first_record);

        if (result == ERROR)
            if ((i > 1) && (db_error == DE_EMPTY))
                db_error = DE_NOTFOUND;

        return result;
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file (cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file (cur_dbcb->real_file_name);
            return (ERROR);
        }

        if (num < 0)
                num = (int) cur_dbcb->num_recs + 1 + num;

        if ((num > cur_dbcb->num_recs) || (num < 1))
        {
                db_error = DE_NOTFOUND;
		set_error_file ( cur_dbcb->real_file_name);
                return (ERROR);
        }

        my_num = num;
        cur_dbcb->cur_rba = ((my_num - 1) * cur_dbcb->rec_size)
            + cur_dbcb->r1_offset;

        if (num != cur_dbcb->cur_rec)
        {
                if (lseek (cur_dbcb->db_fd, (long) cur_dbcb->cur_rba, 0) == ERROR)
                {
                        db_error = DE_SEEKFAIL_READ;
			set_error_file (cur_dbcb->real_file_name);
                        return (ERROR);
                }

                bytes_read = read (cur_dbcb->db_fd, cur_dbcb->data_buf,
                                cur_dbcb->rec_size);

                if (bytes_read == ERROR)
                {
                        db_error = DE_READFAIL;
			set_error_file (cur_dbcb->real_file_name);
                        return (ERROR);
                }

                if (bytes_read < cur_dbcb->rec_size)
                {
                    db_error = DE_TOOSMALL;
		    set_error_file (cur_dbcb->real_file_name);
                    return (ERROR);
                }
        }

        cur_dbcb->cur_rec = num;

        c = cur_dbcb->data_buf;

        if (*c == '*')
                return (DELETED);

        return (0);
}

EXPORT_DLL
int dbgetloc (struct dbcb *cur_dbcb)
{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;

    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return (partitions->partition[partitions->current_entry].first_record
     + (int) (partitions->partition[partitions->current_entry].dbcb->cur_rec))
     - 1;
    }  /*  Multiple partitions  */
#endif

    return (int) (cur_dbcb->cur_rec);
}

EXPORT_DLL
int dblocatec (struct dbcb *cur_dbcb, char *field, char *s_value)
/*  locate a particular value */

{
        char f_value[256];
        int i;

	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

        /*  The following loop is executed until (1) a match is
        found between a record in the data base  and the user-supplied
        argument, or (2) until the end of the file is reached.
        */

        while (TRUE)
        {
                if (dbgetc (cur_dbcb, field, f_value) == ERROR)
                        return (ERROR);

                /*   the following loop compares the two
                strings, characters.  It differes from strcmp
                in that if the end of the first string is
                reached, the strings are considered matched.
                */

                for (i = 0; (s_value[i] ==
                        f_value[i]) &&
                        (s_value[i] != 0); i++) ;

                if (s_value[i] == 0)
                        return (0);

                if (dbskip (cur_dbcb, 1) == ERROR)
                        return (ERROR);
        }
    NO_EXIT ();
}

EXPORT_DLL
int dblocated (struct dbcb *cur_dbcb, char *field, char *s_value)
/*  locate a particular value */

{
        char f_value[255];
        char my_s_value[255];

	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

        /*  The following loop is executed until (1) a match is
        found between a record in the data base  and the user-supplied
        argument, or (2) until the end of the file is reached.
        */

        strncpy (my_s_value, s_value, 254);
        my_s_value[254] = 0;

        if (cdate (my_s_value, (char) 'b') > 2)
        {
            db_error = DE_BADDATE;
	    set_error_file (cur_dbcb->real_file_name);
            return (ERROR);
        }

        while (TRUE)
        {
                if (dbgetd (cur_dbcb, field, f_value) == ERROR)
                        return (ERROR);

                if (strncmp (my_s_value, f_value,  10) == 0)
                   return (0);

                if (dbskip (cur_dbcb, 1) == ERROR)
                        return (ERROR);
        }
    NO_EXIT ();
}

EXPORT_DLL
int dblocaten (struct dbcb* cur_dbcb, char *field, int s_value)
/*  locate a particular value */

{
        int f_value;

	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

        /*  The following loop is executed until (1) a match is
        found between a record in the data base  and the user-supplied
        argument, or (2) until the end of the file is reached.
        */

        while (TRUE)
        {
                if (dbgetn (cur_dbcb, field, &f_value) == ERROR)
                        return (ERROR);

                if (f_value == s_value)
                        return (0);

                if (dbskip (cur_dbcb, 1) == ERROR)
                        return (ERROR);
        }
    NO_EXIT ();
}

EXPORT_DLL
int dblocatef (struct dbcb* cur_dbcb, char *field, double s_value)
/*  locate a particular value */

{
        double f_value;

	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

        /*  The following loop is executed until (1) a match is
        found between a record in the data base  and the user-supplied
        argument, or (2) until the end of the file is reached.
        */

        while (TRUE)
        {
                if (dbgetf (cur_dbcb, field, &f_value) == ERROR)
                        return (ERROR);

                if (f_value == s_value)
                        return (0);

                if (dbskip (cur_dbcb, 1) == ERROR)
                        return (ERROR);
        }
    NO_EXIT ();
}

static int cvt_date (char *i_date, char *o_date, char *prev_date)

{
      char my_date[80];

      if (i_date == NULL)
	      return -1;

      strncpy (my_date, i_date, 79);
      my_date[79] = 0;
      ztrim (my_date);

      if (my_date[0] == 0)
      {
          strncpy (o_date, "        ", 8);
          return 0;
      }

      if (strncmp (my_date, "00/00/00", 8) == 0)
      {
          strncpy (o_date, "00000000", 8);
          return 0;
      }

      if (strncmp (my_date, "99/99/99", 8) == 0)
      {
          strncpy (o_date, "99999999", 8);
          return 0;
      }

      if (((my_date[0] == '+') || (my_date[0] == '-'))
      && (prev_date[0] != ' '))
      {   /* Use date relative to old date */
          if (cdate_2 (my_date, (char) 'b', prev_date) > 2)
              return -1;
      }   /* Use date relative to old date */
      else
          if (cdate (my_date, (char) 'b') > 2) /* Use date relative to*/
                                               /* today */
              return -1;

      o_date[0] = my_date[0];
      o_date[1] = my_date[1];
      o_date[2] = my_date[2];
      o_date[3] = my_date[3];
      o_date[4] = my_date[5];
      o_date[5] = my_date[6];
      o_date[6] = my_date[8];
      o_date[7] = my_date[9];
      return 0;
}


EXPORT_DLL
int dbgetc (struct dbcb *cur_dbcb, const char *field, char *value)
/*  get a field from current record */

{
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbgetc (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

                mvc (cur_dbcb->fields[i].f_size, value, cur_loc);
                value[cur_dbcb->fields[i].f_size] = 0;
                return (0);

}

EXPORT_DLL
int dbgetd (struct dbcb* cur_dbcb, char *field, char *value)
/*  get a field from current record */

{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbgetd (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        if (strncmp (cur_loc, "        ", 8) == 0)
        {
            strcpy (value, "        ");
            return 0;
        }

        if (strncmp (&cur_loc[2], "999999", 6) == 0)
        {
            strcpy (value, "99/99/99");
            return 0;
        }

        value[0]    = cur_loc[0];
        value[1]    = cur_loc[1];
        value[2]    = cur_loc[2];
        value[3]    = cur_loc[3];
        value[4]    = '/';
        value[5]    = cur_loc[4];
        value[6]    = cur_loc[5];
        value[7]    = '/';
        value[8]    = cur_loc[6];
        value[9]    = cur_loc[7];
        value[10]    = 0;

    return (0);
}

EXPORT_DLL
int dbgetn (struct dbcb *cur_dbcb, char *field, int *value)
/*  get a field from current record */

{
        int i;
        char *s;
        char temp[100];
        int size;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbgetn (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        size = cur_dbcb->fields[i].f_size;
        strncpy (temp, cur_loc, size);
        temp[size] = 0;
        for (s = temp; *s == ' '; s++);
        *value = atoi (s);
        return (0);
}

EXPORT_DLL
int dbgetf (struct dbcb *cur_dbcb, char *field, double *value)
/*  get a field from current record */

{
        int i;
        char *s;
        char temp[100];
        int size;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbgetf (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        size = cur_dbcb->fields[i].f_size;
        strncpy (temp, cur_loc, size);
        temp[size] = 0;
        for (s = temp; *s == ' '; s++);
        *value = (double) atof (s);
	return (0);
}

EXPORT_DLL
int dbgetl (struct dbcb *cur_dbcb, char *field, int *value)
/*  get a field from current record */

{
        char c;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbgetl (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

       if ((dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

       if (((c = cur_loc[0]) == 'Y') ||
	   (c == 'y') || (c == 'T') || (c == 't'))
                *value = 1;
        else if (((c = cur_loc[0]) == 'N') ||
		 (c == 'n') || (c == 'F') || (c == 'f') || (c == ' '))
		*value = 0;
       else
	       *value = -1;

       return (0);
}

static int check_ri (struct dbcb *cur_dbcb, int i)
{
    char base_value_c[256];
    double base_value_f;
    char base_field[11];
    char reference_field[11];
    struct dbcb* ri_file;
    int result;

    /*                                                                     */
    /*  If no RI file, then always return success                          */
    /*                                                                     */

    if (cur_dbcb->ref_fields == NULL)
        return 0;

    /*                                                                     */
    /*  Get the base field                                                 */
    /*                                                                     */

    strncpy (base_field, cur_dbcb->fields[i].f_name, 10);
    base_field[10] = 0;
    ztrim (base_field);

    switch (cur_dbcb->fields[i].f_type)
    {
    case 'C':
        dbgetc (cur_dbcb, base_field, base_value_c);  break;

    case 'D':
        dbgetd (cur_dbcb, base_field, base_value_c);  break;

    case 'N':
        dbgetf (cur_dbcb, base_field, &base_value_f);  break;

    case 'L':
        return TRUE;  /* Logical fields are not logical candidates */
    }

    /*                                                                     */
    /*  Get the reference field                                            */
    /*                                                                     */

    ri_file = cur_dbcb->ref_fields[i].r_file;

    if (ri_file == NULL)
        return 0;

    strncpy (reference_field,
             ri_file->fields[cur_dbcb->ref_fields[i].r_field].f_name,
             10);

    reference_field[10] = 0;
    ztrim (reference_field);

    switch (cur_dbcb->fields[i].f_type)
    {
    case 'C':
        result = dbezlocc (ri_file, reference_field, base_value_c,
                           FROM_TOP);
        break;

    case 'D':
        result = dbezlocd (ri_file, reference_field, base_value_c,
                           FROM_TOP);
        break;

    case 'N':
        result = dbezlocf (ri_file, reference_field, base_value_f,
                           FROM_TOP);
        break;
    }

    if (result == ERROR)
    {
        if (db_error == DE_NOTFOUND)
            db_error = DE_RI_FAIL;

	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    return 0;
}

EXPORT_DLL
int dbputc (struct dbcb *cur_dbcb, char *field, char *value)
/*  put a field from current record */

{
       int i;
        char s[257];
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbputc (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

    if (cur_dbcb->flag & CREATE_IN_PROGRESS)
    {
        db_error = DE_CREATMODE;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    if (cur_dbcb->num_recs == 0)
    {
        db_error = DE_EMPTY;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

    if ((cur_dbcb->flag & READ_WRITE) == 0)
    {
        db_error = DE_RDONLY;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;

        mvc (cur_dbcb->fields[i].f_size, s, value);
        s[cur_dbcb->fields[i].f_size] = 0;
        rpad (s, cur_dbcb->fields[i].f_size);
        mvc (cur_dbcb->fields[i].f_size, cur_loc, s);

        return (check_ri (cur_dbcb, i));
}

EXPORT_DLL
int dbputd (struct dbcb *cur_dbcb, char *field, char *value)
/*  put a field from current record */

{
        int i;
        char s[257];
        char prev_date[11];
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbputd (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;

        if (dbgetd (cur_dbcb, field, prev_date) == ERROR)
            return ERROR;

        if (cvt_date (value, s, prev_date))
        {
            db_error = DE_BADDATE;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        mvc (cur_dbcb->fields[i].f_size, cur_loc, s);

        return (check_ri (cur_dbcb, i));
}

EXPORT_DLL
int dbputn (struct dbcb *cur_dbcb, char *field, int value)
/*  put a field from current record */

{
        int i;
        char s[257];
        char f[10];
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbputn (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;

        sprintf (f, "%%%dd", cur_dbcb->fields[i].f_size);
        sprintf (s, f, value);

        if ((char) strlen (s) > cur_dbcb->fields[i].f_size)
        {
            db_error = DE_TOOBIG;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        mvc (cur_dbcb->fields[i].f_size, cur_loc, s);
        return (check_ri (cur_dbcb, i));
}

EXPORT_DLL
int dbputf (struct dbcb *cur_dbcb, char *field, double value)
/*  put a field from current record */

{
        int i;
        char s[257];
        char f[10];
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbputf (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;

        sprintf (f, "%%%d.%df", cur_dbcb->fields[i].f_size,
                                cur_dbcb->fields[i].f_decimals);
        sprintf (s, f, value);

        if ((char) strlen (s) > cur_dbcb->fields[i].f_size)
        {
            db_error = DE_TOOBIG;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        mvc (cur_dbcb->fields[i].f_size, cur_loc, s);
        return (check_ri (cur_dbcb, i));
}

EXPORT_DLL
int dbputl (struct dbcb *cur_dbcb, char *field, int value)
/*  put a field from current record */

{
        int i;
        char s;
        int Bool;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbputl (partitions->partition[partitions->current_entry].dbcb,
                       field, value);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        if ((i = dbfield (cur_dbcb, field)) == ERROR)
                return (ERROR);

        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;

        if (value == 1)
		s = 'T';
        else if (value == 0)
		s = 'F';
	else
		s = '?';

        *cur_loc = s;
        return (check_ri (cur_dbcb, i));
}

EXPORT_DLL
int dbclose (struct dbcb *cur_dbcb)
/* close the data base */

{
    int fd;
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        for (i = 0; i < partitions->partition_count; ++i)
        {
            if (dbclose (partitions->partition[i].dbcb) == ERROR)
                return ERROR;
        }

        return 0;
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

    if (dbflush (cur_dbcb) == ERROR)
        return (ERROR);

        /*  if there is an index file, close it */

        if (cur_dbcb->ndx_cb != NULL)
                if (close (cur_dbcb->ndx_cb->ndx_fd) == ERROR)
                {
                        db_error = DE_CLOSEFAIL_NDX;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }

	/*  Unlock the file */
	release_lock2(cur_dbcb->lock_structure);

        /*  now close the file */

	fd = cur_dbcb->db_fd;

        if (close (fd) == ERROR)
        {
                db_error = DE_CLOSEFAIL_DBF;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        if (cur_dbcb->memo_file > -1)
            if (close (cur_dbcb->memo_file))
            {
                db_error = DE_CLOSEFAIL_DBT;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
            }

        if (cur_dbcb->real_file_name != NULL)
            free (cur_dbcb->real_file_name);

        free (cur_dbcb->data_buf);

        if (cur_dbcb->ref_fields != NULL)
        {  /*  Free RI fields  */
		for (i = 0;
		     (i < MAX_FIELDS) && (cur_dbcb->fields[i].f_name[0] != 0x0d);
		     ++i)
                if (cur_dbcb->ref_fields[i].r_file != NULL)
                    dbclose (cur_dbcb->ref_fields[i].r_file);

            free (cur_dbcb->ref_fields);
        }  /*  Free RI fields  */

        free (cur_dbcb);

        return (0);
}

EXPORT_DLL
int dbflush (struct dbcb *cur_dbcb)
/* flush the data base */

{
        int fd;
        char date[21];

#ifdef BIGENDIAN
      union
      {
        char c[4];
        int i ;
      } swapper;
#else
      int save;
#endif

    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        for (i = 0; i < partitions->partition_count; ++i)
        {
            if (dbflush (partitions->partition[i].dbcb) == ERROR)
                return ERROR;
        }

        return 0;
    }  /*  Multiple partitions  */
#endif

        /* purge the buffer */

        if (dbupdate (cur_dbcb) == ERROR)
           if (db_error != DE_EMPTY)   /*  if not "empty file" error */
	   {
		   set_error_file(cur_dbcb->real_file_name);
               return (ERROR);
	   }

        fd = cur_dbcb->db_fd;

        /*  if read/write mode, update the date modified */

        if (cur_dbcb->flag & UPDATE_HEADER)
        {
#ifndef BIGENDIAN
                save = (int) cur_dbcb->num_recs;
#endif
                if (lseek (fd, 0L, 0) == ERROR)
                {
                        db_error = DE_SEEKFAIL_CLOSE;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }

                if (read (fd, (char *) cur_dbcb, DBCB_SIZE) != DBCB_SIZE)
                {
                        db_error = DE_READFAIL_CLOSE;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }

                time_stamp (date);

                cur_dbcb->yy = (char) atoi (&date[2]);
                cur_dbcb->mm = (char) atoi (&date[5]);
                cur_dbcb->dd = (char) atoi (&date[8]);

#ifdef BIGENDIAN
                swapper.i = cur_dbcb->num_recs;
                cur_dbcb->num_recs_swapped[0] = swapper.c[3];
                cur_dbcb->num_recs_swapped[1] = swapper.c[2];
                cur_dbcb->num_recs_swapped[2] = swapper.c[1];
                cur_dbcb->num_recs_swapped[3] = swapper.c[0];
#else
                cur_dbcb->num_recs = save;
#endif

                if (lseek (fd, 0L, 0) == ERROR)
                {
                        db_error = DE_SEEKFAIL_CLOSE2;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }

                if (write (fd, (char *) cur_dbcb, DBCB_SIZE) != DBCB_SIZE)
                {
                        db_error = DE_WRITEFAIL_CLOSE;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }
                cur_dbcb->flag &= 255 - UPDATE_HEADER;
        }

        return (0);
}

EXPORT_DLL
int dbfield (struct dbcb *cur_dbcb, const char *field)
/* find the specified field */

{
        int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbfield (partitions->partition[partitions->current_entry].dbcb,
                       field);
    }  /*  Multiple partitions  */
#endif

        cur_loc = (cur_dbcb->data_buf) + 1;

        for (i = 0; i < MAX_FIELDS; i++)
        {
                if (strcmp (field, cur_dbcb->fields[i].f_name) == 0)
                {
                        cur_field_type = cur_dbcb->fields[i].f_type;
                        return (i);
                }
                cur_loc += cur_dbcb->fields[i].f_size;
        }
        db_error = DE_BADFIELDNAME;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
}

EXPORT_DLL
int dbupdate (struct dbcb *cur_dbcb)
/* write buffer if it has been updated */

{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
       return dbupdate (partitions->partition[partitions->current_entry].dbcb);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->num_recs == 0)
        {
            cur_dbcb->flag &= 255 - MODIFIED;
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if (cur_dbcb->flag & MODIFIED)
        {
                if (lseek (cur_dbcb->db_fd, (long) cur_dbcb->cur_rba, 0) == ERROR)
                {
                        db_error = DE_SEEKFAIL_WRITE;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }

                if (write (cur_dbcb->db_fd, cur_dbcb->data_buf,
                        cur_dbcb->rec_size) != cur_dbcb->rec_size)
                {
                        db_error = DE_WRITEFAIL;
			set_error_file(cur_dbcb->real_file_name);
                        return (ERROR);
                }
                cur_dbcb->flag &= 255 - MODIFIED;
        }
    return (0);
}

EXPORT_DLL
int dbappend (struct dbcb *cur_dbcb)
/*  dbappend - add a new record to the end of the file */

{
        long i;  /*  loop counter */
        long j;  /*  temporary */
        char *temp;  /* pointer to buffer location */
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        if (dbflush (cur_dbcb) == ERROR)
            return ERROR;

        partitions->current_entry = partitions->partition_count - 1;

       return dbappend (partitions->partition[partitions->current_entry].dbcb);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->flag & CREATE_IN_PROGRESS)
        {
            db_error = DE_CREATMODE;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        ++(cur_dbcb->num_recs);

        if (dbgoto (cur_dbcb, -1) == ERROR)
            if ((db_error != DE_READFAIL) && (db_error != DE_TOOSMALL))
	    {
		    set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
	    }

        cur_dbcb->cur_rec = cur_dbcb->num_recs;

        for (i = 1, temp = cur_dbcb->data_buf; i <= cur_dbcb->rec_size;
                ++i, ++temp)
            *temp = ' ';

        *temp = 0x1A;   /*  add end of file after record */

        if (lseek (cur_dbcb->db_fd, (long) cur_dbcb->cur_rba, 0) == ERROR)
        {
                db_error = DE_SEEKFAIL_WRITE;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        j = (long) (cur_dbcb->cur_rba + i);  /*  j = end of record rba */
        j = ((j + 511) / 512) * 512;  /* round j up to 512-byte boundary */
        i = (long) (j - cur_dbcb->cur_rba);  /* recompute bytes to write */

        if ((j = write ((int) cur_dbcb->db_fd, cur_dbcb->data_buf,
                   (unsigned int) i)) != i)
        {
#ifdef GUI
char text[500];
sprintf (text,
"\nrba = %d\nrecno = %d\ncount to write = %d\ncount written= %d\nWIN32 error = %d",
cur_dbcb->cur_rba, cur_dbcb->cur_rec, i, j, GetLastError ());
MessageBox (NULL, text, "BugCatcher", MB_OK);
DebugBreak ();
#endif
                db_error = DE_WRITEFAIL;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }
        cur_dbcb->flag &= 255 - MODIFIED;
        cur_dbcb->flag |= UPDATE_HEADER;
        return (0);
}


EXPORT_DLL
void dberror_return (const char *string)
/*  reports error codes and returns to caller */

{
#ifdef MAC
#define MU
#endif

#ifdef UNIX
#define MU
#endif

#ifdef MU
      char temp[80];
#endif
      char text[50];

      int os = FALSE;

#ifdef GUI
      char message[500];

      os = dberror_to_text (db_error, text, 50);

      sprintf (message, "Error in xBASE access routine:  %s\n"
	       "xBASE error code %d:  %s\nFilename: %s\n", string,
	       db_error, text, db_error_file);

      if (os)
      {
	      sprintf (text, "OS error code %d:  %s", errno, strerror (errno));
	      strcat (message, text);
      }

      MessageBox (NULL, message, "xBASE Error",
                  MB_OK | MB_ICONEXCLAMATION);
#else
      printf ("\nError in xBASE access routine:  %s \n", string);
      printf ("xBASE error code %d:  ", db_error);

      os = dberror_to_text (db_error, text, 50);
      printf ("%s\n", text);
      fflush (stdout);

      if (os)
      {

#ifdef MU
          sprintf (temp, "OS error code %d", errno);
          perror (temp);
#else
          printf ("OS error code %d:  %s\n", errno, strerror (errno));
#endif
      }
#endif

      printf ("Data base in error:  %s\n", db_error_file);
}

EXPORT_DLL
void dberror (char *string)

{
    dberror_return (string);
    exit (ERROR_RETURN);
}

EXPORT_DLL
int dbmark (struct dbcb *cur_dbcb, char mark)
/* implements dbdelete and dbrecall */

{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbmark (partitions->partition[partitions->current_entry].dbcb,
                       mark);
    }  /*  Multiple partitions  */
#endif

        if (cur_dbcb->num_recs == 0)
        {
            db_error = DE_EMPTY;
	    set_error_file(cur_dbcb->real_file_name);
            return (ERROR);
        }

        if ((cur_dbcb->flag & READ_WRITE) == 0)
        {
                db_error = DE_RDONLY;
		set_error_file(cur_dbcb->real_file_name);
                return (ERROR);
        }

        cur_loc = cur_dbcb->data_buf;

        *cur_loc = mark;
        cur_dbcb->flag |= MODIFIED + UPDATE_HEADER;
        return (0);
}

EXPORT_DLL
int dbezlocc (struct dbcb* cur_dbcb, char *field, char *s_value,
 start_values start_from)
/*  easy locate */

{
   int error;

   if (check_dbcb(cur_dbcb) == ERROR)
	   return ERROR;

   switch (start_from)
   {
      case FROM_TOP:       error = dbrewind (cur_dbcb);    break;
      case FROM_CURRENT:   error = 0;                      break;
      case FROM_NEXT:      error = dbskip (cur_dbcb, 1);   break;
      default:             db_error = DE_BADPARM;  error = ERROR;  break;
   } /* switch */

   if (error == ERROR)
      return (ERROR);

   return (dblocatec (cur_dbcb, field, s_value));
}

EXPORT_DLL
int dbezlocd (struct dbcb *cur_dbcb, char *field, char *s_value,
 start_values start_from)
/*  easy locate */

{
   int error;

   if (check_dbcb(cur_dbcb) == ERROR)
	   return ERROR;

   switch (start_from)
   {
      case FROM_TOP:       error = dbrewind (cur_dbcb);    break;
      case FROM_CURRENT:   error = 0;  break;
      case FROM_NEXT:      error = dbskip (cur_dbcb, 1);  break;
      default:             db_error = DE_BADPARM;
			   error = ERROR;
			   set_error_file(cur_dbcb->real_file_name);
			   break;
   } /* switch */

   if (error == ERROR)
      return (ERROR);

   return (dblocated (cur_dbcb, field, s_value));
}

EXPORT_DLL
int dbezlocl (struct dbcb *cur_dbcb, char *field, int s_value,
 start_values start_from)
/*  easy locate */

{
   int error;

   if (check_dbcb(cur_dbcb) == ERROR)
	   return ERROR;

   switch (start_from)
   {
      case FROM_TOP:       error = dbrewind (cur_dbcb);    break;
      case FROM_CURRENT:   error = 0;  break;
      case FROM_NEXT:      error = dbskip (cur_dbcb, 1);  break;
      default:             db_error = DE_BADPARM;
			   error = ERROR;
			   set_error_file(cur_dbcb->real_file_name);
			   break;
   } /* switch */

   if (error == ERROR)
      return (ERROR);

   return (dblocatel (cur_dbcb, field, s_value));
}

EXPORT_DLL
int dbezlocn (struct dbcb *cur_dbcb, char *field, int s_value,
 start_values start_from)
/*  easy locate */

{
   int error;

   if (check_dbcb(cur_dbcb) == ERROR)
	   return ERROR;

   switch (start_from)
   {
      case FROM_TOP:       error = dbrewind (cur_dbcb);    break;
      case FROM_CURRENT:   error = 0;  break;
      case FROM_NEXT:      error = dbskip (cur_dbcb, 1);  break;
      default:             db_error = DE_BADPARM;
			   error = ERROR;
			   set_error_file(cur_dbcb->real_file_name);
			   break;
   } /* switch */

   if (error == ERROR)
      return (ERROR);

   return (dblocaten (cur_dbcb, field, s_value));
}

EXPORT_DLL
int dbezlocf (struct dbcb *cur_dbcb, char *field, double s_value,
 start_values start_from)
/*  easy locate */

{
   int error;

   if (check_dbcb(cur_dbcb) == ERROR)
	   return ERROR;

   switch (start_from)
   {
      case FROM_TOP:       error = dbrewind (cur_dbcb);    break;
      case FROM_CURRENT:   error = 0;  break;
      case FROM_NEXT:      error = dbskip (cur_dbcb, 1);  break;
      default:             db_error = DE_BADPARM;
			   error = ERROR;
			   set_error_file(cur_dbcb->real_file_name);
			   break;
   } /* switch */

   if (error == ERROR)
      return (ERROR);

   return (dblocatef (cur_dbcb, field, s_value));
}

EXPORT_DLL
int dbjoin (struct dbcb *dbcb1, char *field1, char* index_field,
            struct dbcb *dbcb2, char *field2)
{
    char data1[100];
    char data2[100];
    int index;

    if (check_dbcb(dbcb1) == ERROR)
	    return ERROR;

    if (check_dbcb(dbcb2) == ERROR)
	    return ERROR;

    if (dbgetc (dbcb1, field1, data1) == ERROR)
        return ERROR;

    if (dbgetn (dbcb1, index_field, &index) == ERROR)
        return ERROR;

    if (dbgoto (dbcb2, index) == ERROR)
    {
        if (db_error == DE_NOTFOUND)
        {
		if (dbezlocc (dbcb2, field2, data1, FROM_TOP) == ERROR)
		{
			return ERROR;
		}

		if (dbputn (dbcb1, index_field, (int) dbcb2->cur_rec) == ERROR)
		{
			return ERROR;
		}

            return 0;
        }
        else
            return ERROR;
    }
    else
    {
        if (db_deleted (dbcb2))
            data2[0] = 0;
        else
            if (dbgetc (dbcb2, field2, data2) == ERROR)
                return ERROR;

        if (strcmp (data1, data2) != 0)
        {
            if (dbezlocc (dbcb2, field2, data1, FROM_TOP) == ERROR)
                return ERROR;

            if (dbputn (dbcb1, index_field, (int) dbcb2->cur_rec) == ERROR)
                return ERROR;
        }

    }

    return 0;
}

EXPORT_DLL
int dbskip (struct dbcb *cur_dbcb, int value)
{
    int skip_deleted;
    int current_record;

#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

    if (partitions->eye_catcher == MP_EYECATCHER)
    {
        skip_deleted = partitions->partition[0].dbcb->flag & SKIP_DELETED;
    }
    else
#endif
    {
        skip_deleted = cur_dbcb->flag & SKIP_DELETED;
    }

    current_record = dbgetloc (cur_dbcb);
    if (dbgoto (cur_dbcb, current_record + value) == ERROR)
        return ERROR;

    if (skip_deleted)
        while (db_deleted (cur_dbcb))
            if (dbskip (cur_dbcb, (value < 0) ? -1 : 1))
                return ERROR;

    return 0;
}

EXPORT_DLL
int dbrewind (struct dbcb *cur_dbcb)
{
	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

    if (dbgoto (cur_dbcb, 1) == ERROR)
        return ERROR;

    return dbskip (cur_dbcb, 0);
}

EXPORT_DLL
int dberror_to_text (dbase_error db_err, char *text, int text_size)
{
      int os = FALSE;

      switch (db_err)
      {
      case DE_NOMEM_DBCB:
          strncpy (text, "Out of memory for DBCB", text_size);
          break;

      case DE_OPENFAIL:
          strncpy (text, "Unable to open .DBF File", text_size);
          os = TRUE;
          break;

      case DE_READFAIL_DBCB:
          strncpy (text, "Unable to read DB structure during open or refresh",
                   text_size);
          os = TRUE;
          break;

      case DE_NOMEM_BUF:
          strncpy (text, "Out of memory for buffer", text_size);
          break;

      case DE_WRITEFAIL:
          strncpy (text, "Write error", text_size);
          os = TRUE;
          break;

      case DE_NOTFOUND:
          strncpy (text, "Record not found", text_size);
          break;

      case DE_CLOSEFAIL_NDX:  strncpy (text, "Unable to close .NDX file",
                                       text_size);
          os = TRUE;
          break;

      case DE_CLOSEFAIL_DBF:
          strncpy (text, "Unable to close .DBF file", text_size);
          os = TRUE;
          break;

      case DE_RDONLY:
          strncpy (text, "Can't write read/only file", text_size);
          break;

      case DE_SEEKFAIL_CLOSE:
          strncpy (text, "Seek failure (read during close)", text_size);
          os = TRUE;
          break;

      case DE_READFAIL_CLOSE:
          strncpy (text, "Can't read structure during close", text_size);
          os = TRUE;
          break;

      case DE_SEEKFAIL_CLOSE2:
          strncpy (text, "Seek failure (write during close)", text_size);
          os = TRUE;
          break;

      case DE_WRITEFAIL_CLOSE:
          strncpy (text, "Write error during close", text_size);
          os = TRUE;
          break;

      case DE_SEEKFAIL_READ:
          strncpy (text, "Seek failure (read)", text_size);
          os = TRUE;
          break;

      case DE_READFAIL:
          strncpy (text, "Read failure", text_size);
          os = TRUE;
          break;

      case DE_SEEKFAIL_WRITE:
          strncpy (text, "Seek failure (write)", text_size);
          os = TRUE;
          break;

      case DE_BADFIELDNAME:
          strncpy (text, "Field name not found", text_size);
          break;

      case DE_BADFILE:
          strncpy (text, "Not a valid data base file", text_size);
          break;

      case DE_EMPTY:
          strncpy (text, "Data base is empty", text_size);
          break;

      case DE_BADPARM:
          strncpy (text, "Invalid parameter to dbezloc", text_size);
          break;

      case DE_BADDATE:
          strncpy (text, "Invalid date format", text_size);
          break;

      case DE_TOOBIG:
          strncpy (text, "Numeric value too large for field", text_size);
          break;

      case DE_CREATFAIL:
          strncpy (text, "Unable to create new .DBF file", text_size);
          os = TRUE;
          break;

      case DE_OPENNEWFAIL:
          strncpy (text, "Unable to open new .DBF file", text_size);
          os = TRUE;
          break;

      case DE_CREATMODE:
          strncpy (text, "Invalid operation while create is in progress",
                   text_size);
          break;

      case DE_NOCREATMODE:
          strncpy (text, "Invalid operation if create not in progress",
                   text_size);
          break;

      case DE_BADFIELD_CREAT:
          strncpy (text, "Invalid field-name during create", text_size);
          break;

      case DE_BADTYPE_CREAT:
          strncpy (text, "Invalid field-type during create", text_size);
          break;

      case DE_TOOMANY:
          strncpy (text, "Too many fields in new data base", text_size);
          break;

      case DE_DIRTY:
          strncpy (text, "Trying to refresh header from disk while it's dirty",
                   text_size);
          break;

      case DE_SEEKFAIL_OPEN:
          strncpy (text, "Seek failure (open or refresh)", text_size);
          os = TRUE;
          break;

      case DE_TOOSMALL:
          strncpy (text, "File is too small or header is invalid", text_size);
          break;

      case DE_NOMEM_REF:
          strncpy (text, "Unable to acquire memory for .RI file", text_size);
          break;

      case DE_RI_OPEN_FAIL:
          strncpy (text, "Unable to open the .RI file", text_size);
          break;

      case DE_RI_BAD_FIELD:
          strncpy (text, "Invalid or missing field in .RI file", text_size);
          break;

      case DE_RI_FAIL:
          strncpy (text, "Updated field failed Referencial Integrity check", text_size);
          break;

      case DE_NOMEMO:
          strncpy (text, "No memo file or error opening memo file", text_size);
          break;

      case DE_MEMOBUFTOOSMALL:
          strncpy (text, "The buffer for the memo data is too small", text_size);
          break;

      case DE_CLOSEFAIL_DBT:
          strncpy (text, "The close of the memo file failed", text_size);
          break;

      case DE_BADDBCB:
	      strncpy (text, "Invalid dbcb structured passed", text_size);
	      break;

      case DE_CLOSED:
	      strncpy (text, "dbcb was already closed", text_size);
	      break;

      case DE_NO_DBT:
	      strncpy (text, "no DBT file", text_size);
	      break;

      case DE_BAD_DBT_OFFSET:
	      strncpy (text, "Invalid memo offset", text_size);
	      break;

      default:
	      strncpy (text, "Unknown error code", text_size);
          os = TRUE;

      }

      text[text_size - 1] = 0;
      return (os);
}

EXPORT_DLL
int dbcheckrec (struct dbcb* cur_dbcb, int value)
{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbcheckrec (
                        partitions->partition[partitions->current_entry].dbcb,
                           value);
    }  /*  Multiple partitions  */
#endif

        cur_loc = cur_dbcb->data_buf;

        return (*cur_loc == value);
}

EXPORT_DLL
void dbskip_deleted (struct dbcb* cur_dbcb)
{
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
      return;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        for (i = 0; i < partitions->partition_count; ++i)
        {
            dbskip_deleted (partitions->partition[i].dbcb);
        }

        return;
    }  /*  Multiple partitions  */
#endif

    cur_dbcb->flag |= SKIP_DELETED;
}

EXPORT_DLL
void dbreturn_deleted (struct dbcb* cur_dbcb)
{
    int i;
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
      return;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        for (i = 0; i < partitions->partition_count; ++i)
        {
            dbreturn_deleted (partitions->partition[i].dbcb);
        }

        return;
    }  /*  Multiple partitions  */
#endif

    cur_dbcb->flag &= (~SKIP_DELETED);
}

EXPORT_DLL
int dbget_record_size (struct dbcb* cur_dbcb)
{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbget_record_size (
                        partitions->partition[partitions->current_entry].dbcb);
    }  /*  Multiple partitions  */
#endif

    return (cur_dbcb->rec_size);

}

EXPORT_DLL
char* dbget_record_buffer (struct dbcb* cur_dbcb)
{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
      return NULL;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        return dbget_record_buffer (
                        partitions->partition[partitions->current_entry].dbcb);
    }  /*  Multiple partitions  */
#endif

    return (cur_dbcb->data_buf);

}

/****************************************************************************

        NAME:       dbgetnumrecs

        Purpose:    Return the number of records in the database

        Arguments:  1.  cur_dbcb (IN):  dbcb struct

        Processing: Return the numrecs value

        Returns:    Number of records in database or 0 if empty

****************************************************************************/

EXPORT_DLL
   long dbgetnumrecs (struct dbcb* cur_dbcb)

{
	if (check_dbcb(cur_dbcb) == ERROR)
		return ERROR;

	return cur_dbcb->num_recs;
}  /*  dbgetnumrecs  */

/****************************************************************************

        NAME:       dbgetlasterror

        Purpose:    Return the most recent database error

        Arguments:  none.

        Processing: Return the db_error value

        Returns:    the last error

****************************************************************************/

EXPORT_DLL
   long dbgetlasterror ()

{
    return db_error;
}  /*  dbgetnumrecs  */

/****************************************************************************

        NAME:       py_dbget_memo

        Purpose:        To read a memo field for python interface

        Arguments:      1.  cur_dbcb (IN/OUT):  dbcb for file
                        2.  field (IN):  Field to read

        Processing:     1.  Call dbget_memo with previous buffer
                        2.  Allocate new buffer if failed.
			3.  Read again
        Side Effects:   None

        Returns:        String or NULL if error

****************************************************************************/

EXPORT_DLL
const char* py_dbget_memo(struct dbcb* cur_dbcb, char* field)
{
	int actual_size;
	if (dbget_memo(cur_dbcb, field, py_memo_buffer, py_memo_size,
		       &actual_size) == ERROR)
	{
		if (db_error == DE_MEMOBUFTOOSMALL)
		{
			free(py_memo_buffer);
			py_memo_buffer = malloc(actual_size+1);

			if (py_memo_buffer == NULL)
			{
				db_error = DE_NOMEM_BUF;
				set_error_file(cur_dbcb->real_file_name);
				return NULL;
			}
			py_memo_size = actual_size + 1;

			if (dbget_memo(cur_dbcb, field, py_memo_buffer,
				       py_memo_size, &actual_size) == ERROR)
				return NULL;
		}
		else
			return NULL;
	}

	if (py_memo_buffer != NULL)
		py_memo_buffer[actual_size] = 0;

	return py_memo_buffer;
}


/****************************************************************************

        NAME:       dbget_memo

        Purpose:        To read a memo field

        Arguments:      1.  cur_dbcb (IN/OUT):  dbcb for file
                        2.  field (IN):  Field to read
                        3.  buffer (OUT):  Buffer to contain data
                        4.  size (IN):  Size of buffer
                        5.  actual_size (IN):  Actual size of buffer

        Processing:     1.  Check if database is in correct state
                        2.  Read first (or only) block of data file
                        3.  Read additional blocks if necessary

        Side Effects:   None

        Returns:        ERROR if error occurs

****************************************************************************/

EXPORT_DLL
int dbget_memo (struct dbcb* cur_dbcb, char* field, char* buffer, int size,
                int* actual_size)
{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    struct dbcb* real_dbcb;
    int offset;
    struct db_memo_header header;
    struct db_memo_block* block = (struct db_memo_block*) &header;
    int size2;
    int size3;
    int return_value = ERROR;
    int block_size;
#ifdef BIGENDIAN
    union
    {
        char c[4];
        int i;
        short s;
    } swapper;
#endif

    if (check_dbcb(cur_dbcb) == ERROR)
	    return ERROR;

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        real_dbcb = partitions->partition[0].dbcb;
    }  /*  Multiple partitions  */
    else
#endif
        real_dbcb = cur_dbcb;

    if (real_dbcb->flag & CREATE_IN_PROGRESS)
    {
        db_error = DE_CREATMODE;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
        return ERROR;
    }

    if (real_dbcb->num_recs == 0)
    {
        db_error = DE_EMPTY;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
        return (ERROR);
    }
    if (real_dbcb->memo_file < 0)
    {  /*  No memo file  */
        db_error = DE_NOMEMO;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
        return ERROR;
    }  /*  No memo file  */

    if (dbgetn (cur_dbcb, field, &offset) == ERROR)
    {
        *actual_size = 0;
        return ERROR;
    }

    if (offset == 0)
    {  /*  No memo for this record  */
        if (buffer != NULL)
            *buffer = 0;

        *actual_size = 0;
        return 0;
    }  /*  No memo for this record  */

    if (lseek (cur_dbcb->memo_file, 0, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_READ;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    if (read (cur_dbcb->memo_file, (char*) &header, MEMO_HEADER_SIZE) != MEMO_HEADER_SIZE)
    {
        db_error = DE_READFAIL;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

#ifdef BIGENDIAN
    swapper.c[0] = header.block_size_swapped[3];
    swapper.c[1] = header.block_size_swapped[2];
    swapper.c[2] = header.block_size_swapped[1];
    swapper.c[3] = header.block_size_swapped[0];
    block_size = swapper.i;
#else
    block_size = header.block_size;
#endif

    offset *= block_size;

    if (lseek (cur_dbcb->memo_file, offset, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_READ;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
        return ERROR;
    }

    if (read (cur_dbcb->memo_file, (char*) block, MEMO_HEADER_SIZE)
        != MEMO_HEADER_SIZE)
    {
        db_error = DE_BAD_DBT_OFFSET;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
        return ERROR;
    }

#ifdef BIGENDIAN
    swapper.c[0] = block->size_swapped[3];
    swapper.c[1] = block->size_swapped[2];
    swapper.c[2] = block->size_swapped[1];
    swapper.c[3] = block->size_swapped[0];
    size2 = swapper.i;
#else
    size2 = block->size;
#endif

    if ((size2 - 7) > size)
    {
        *actual_size = size2 - 7;
        db_error = DE_MEMOBUFTOOSMALL;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    size3 = size2 - 8;

    if (size2 > MEMO_HEADER_SIZE)
    {  /*  Size more than one block  */
        block = (struct db_memo_block*) malloc (size2);
	if (block == NULL)
	{
		db_error = DE_NOMEM_BUF;
		set_error_file(cur_dbcb->real_file_name);
		return ERROR;
        }
    }  /*  Size more than one block  */

    if (lseek (cur_dbcb->memo_file, offset, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_READ;
	set_error_file(cur_dbcb->real_file_name);
        *actual_size = 0;
    }
    else
    {
        if (read (cur_dbcb->memo_file, (char*) block, size2) != size2)
        {
            db_error = DE_READFAIL;
	    set_error_file(cur_dbcb->real_file_name);
            *actual_size = 0;
        }
        else
        {
            size3 = size2 - 8;
            strncpy (buffer, block->data, size3);
            buffer[size3] = 0;
            *actual_size = size3 + 1; /*  + 1 for delimeter */
             return_value = 0;
        }
    }

    if (size2 > MEMO_HEADER_SIZE)
        free (block);

    return return_value;
}  /*  dbget_memo  */

/****************************************************************************

        NAME:       dbput_memo

        Purpose:        To write a string to a memo file

        Arguments:      1.  cur_dbcb (IN/OUT):  database control block
                        2.  field (IN):  Name of field to write
                        3.  buffer (IN):  Data to write

        Processing:     1.  Get offset from header block
                        2.  Write data to file
                        3.  Update header block

        Side Effects:   None

        Returns:        ERROR if error occurs

****************************************************************************/

int dbput_memo (struct dbcb* cur_dbcb, char* field, char* buffer)

{
#ifdef PARTITIONS
    struct partition_table* partitions = (struct partition_table*) cur_dbcb;
#endif
    struct dbcb* real_dbcb;
    int offset;
    struct db_memo_header header;
    struct db_memo_block* block = (struct db_memo_block*) &header;
    int size;
    int size2;
    int blocks;
    int return_value = ERROR;
    int next_block;
    int block_size;
    int i;
    int non_blank;
#ifdef BIGENDIAN
    union
    {
        char c[4];
	int i
        short s;
    } swapper;
#endif

#ifdef PARTITIONS
    if (partitions->eye_catcher == MP_EYECATCHER)
    {  /*  Multiple partitions  */
        real_dbcb = partitions->partition[0].dbcb;
    }  /*  Multiple partitions  */
    else
#endif
        real_dbcb = cur_dbcb;

    if (real_dbcb->flag & CREATE_IN_PROGRESS)
    {
        db_error = DE_CREATMODE;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    if (real_dbcb->num_recs == 0)
    {
        db_error = DE_EMPTY;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

    if ((real_dbcb->flag & READ_WRITE) == 0)
    {
        db_error = DE_RDONLY;
	set_error_file(cur_dbcb->real_file_name);
        return (ERROR);
    }

    for (i = 0, non_blank = FALSE; !non_blank && (buffer[i] != 0); ++i)
        if (buffer[i] != ' ')
            non_blank = TRUE;

    if (!non_blank)
        return dbputc (cur_dbcb, field, " ");

    if (lseek (cur_dbcb->memo_file, 0, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_READ;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    if (read (cur_dbcb->memo_file, (char*) &header, MEMO_HEADER_SIZE) != MEMO_HEADER_SIZE)
    {
        db_error = DE_READFAIL;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

#ifdef BIGENDIAN
    swapper.c[0] = header.next_block_swapped[3];
    swapper.c[1] = header.next_block_swapped[2];
    swapper.c[2] = header.next_block_swapped[1];
    swapper.c[3] = header.next_block_swapped[0];
    offset = swapper.i;
#else
    offset = header.next_block;
#endif

#ifdef BIGENDIAN
    swapper.c[0] = header.block_size_swapped[3];
    swapper.c[1] = header.block_size_swapped[2];
    swapper.c[2] = header.block_size_swapped[1];
    swapper.c[3] = header.block_size_swapped[0];
    block_size = swapper.l;
#else
    block_size = header.block_size;
#endif
    offset *= block_size;

    if (lseek (cur_dbcb->memo_file, offset, 0) == ERROR)
    {
        db_error = DE_SEEKFAIL_READ;
	set_error_file(cur_dbcb->real_file_name);
        return ERROR;
    }

    size = strlen (buffer) + 8;
    blocks = (size / block_size) + 1;
    size2 = blocks * block_size;

    if (size2 > MEMO_HEADER_SIZE)
    {
        block = (struct db_memo_block*) malloc (size2);
        if (block == NULL)
        {
            db_error = DE_NOMEM_BUF;
	    set_error_file(cur_dbcb->real_file_name);
            return ERROR;
        }
    }

    block->filler1[0] = 0xff;
    block->filler1[1] = 0xff;
    block->filler1[2] = 8;
    block->filler1[3] = 0;

#ifdef BIGENDIAN
    swapper.i = size;
    block->size_swapped[0] = swapper.c[3];
    block->size_swapped[1] = swapper.c[2];
    block->size_swapped[2] = swapper.c[1];
    block->size_swapped[3] = swapper.c[0];
#else
    block->size = size;
#endif

    strncpy (block->data, buffer, size - 8);

    if (write (cur_dbcb->memo_file, (char*) block, size2) == size2)
    {  /*  Update header block  */
        if (lseek (cur_dbcb->memo_file, 0, 0) == 0)
        {
            if (read (cur_dbcb->memo_file, (char*) &header,MEMO_HEADER_SIZE) == MEMO_HEADER_SIZE)
            {
#ifdef BIGENDIAN
                swapper.c[0] = header.next_block_swapped[3];
                swapper.c[1] = header.next_block_swapped[2];
                swapper.c[2] = header.next_block_swapped[1];
                swapper.c[3] = header.next_block_swapped[0];
                next_block = swapper.i;
#else
                next_block = header.next_block;
#endif
                next_block += blocks;

#ifdef BIGENDIAN
                swapper.i = next_block;
                header.next_block_swapped[0] = swapper.c[3];
                header.next_block_swapped[1] = swapper.c[2];
                header.next_block_swapped[2] = swapper.c[1];
                header.next_block_swapped[3] = swapper.c[0];
#else
                header.next_block = next_block;
#endif

                if (lseek (cur_dbcb->memo_file, 0, 0) == 0)
                {
                    if (write (cur_dbcb->memo_file, (char*) &header, MEMO_HEADER_SIZE) == MEMO_HEADER_SIZE)
                    {
			set_error_file(cur_dbcb->real_file_name);
                        if (dbputn (cur_dbcb, field, offset /block_size) != ERROR)
                            return_value = 0;
                    }
                    else
                        db_error = DE_WRITEFAIL;
                }
                else
                    db_error = DE_SEEKFAIL_WRITE;
            }
            else
                db_error = DE_READFAIL;
        }
        else
            db_error = DE_SEEKFAIL_READ;

        if (size > MEMO_HEADER_SIZE)
            free (block);
    }  /*  Update header block  */

    return return_value;
}  /*  dbput_memo  */

/****************************************************************************

        NAME:       dbgetdatemodified

        Purpose:        To construct a string showing the date modified

        Arguments:      1.  cur_dbcb (IN):  database control block

        Processing:     1.  Format the date from header information

        Side Effects:   None.

        Returns:        Staticly allocated string with the date.

****************************************************************************/

char * dbgetdatemodified (struct dbcb *cur_dbcb)

{
#ifdef PARTITIONS
	struct partition_table* partitions;
#endif
	struct dbcb* my_dbcb;
	int mm,dd,yy;
	static char date[10];

#ifdef PARTITIONS
	partitions = (struct partition_table*) cur_dbcb;

	if (partitions->eye_catcher == MP_EYECATCHER)
		my_dbcb = partitions->partition[0].dbcb;
	else
#endif
		my_dbcb = cur_dbcb;

	mm = my_dbcb->mm;
	dd = my_dbcb->dd;
	yy = my_dbcb->yy;

	sprintf (date, "%02d/%02d/%02d", mm, dd, yy);

	return date;
}  /*  dbgetdatemodified  */

/****************************************************************************

        NAME:       set_error_file

        Purpose:        Set the error file variable

        Arguments:      1.  error_file (IN):  string to save

        Processing:     1.  Free the old variable if set
                        2.  Set the new value

        Side Effects:   None.

        Returns:        Staticly allocated string with the date.

****************************************************************************/

static void set_error_file (const char* error_file)
{
	if (db_error_file != NULL)
		free (db_error_file);

	db_error_file = strdup (error_file);
}

/****************************************************************************

        NAME:       dbget_recno

        Purpose:        Return the current record number

        Arguments:      1.  cur_dbcb (IN):  database file

        Processing:     1.  Return the recno

        Side Effects:   None.

        Returns:        Return the recno

****************************************************************************/

EXPORT_DLL
int dbget_recno(struct dbcb* cur_dbcb)
{
	return cur_dbcb->cur_rec;
}

VERSION_PROC (dbase3_version)
