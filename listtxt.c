/**********************************************************************
*
* MODULE: listtxt.c
*
* BRIEF DESCRIPTION:
*       This module contains a text-based implementation of a set of list
*       picking subroutine
*
* MODULE TYPE:
*       Source
*
* FULL DESCRIPTION:
*       This module contains a routine that will let the user select an item
*       from a list.
*
* FUNCTIONS DEFINED:
*       {@functions_defined...@}
*
* GLOBAL DATA DEFINED:
*       {@global_data_defined...@}
*
* REVISION HISTORY:
*       16Sep94      G. Story        Original release
*       06Dec94      G. Story        Dynamically allocate entry_table
*       10Aug95      G. Story        Get rid of Tandem logic to find cursor
*       15Apr04      G. Story        Make sure line is < 78 chars. wide
*                                    {@reason...@}
*       30Dec13      G. Story        Add list_pick_ex
*       10Jan14      G. Story        Fix bug in handling Enter key in
*                                    search mode
*       03Feb14      G. Story        Enhance list_pick_ex 
*       10Feb14      G. Story        Fix null-pointer dereference
*       25Oct14      G. Story	     Add return_is_edit to list_pick_ex
*       08Sep15      G. Story        Don't change CR to 'e' in search
*                                    mode
*	9Oct15	     G. Story	     Add starting entry to _ex interface
*       24Nov15      G. Story        Allow escape sequences that cause
*                                    color changes
*       9Aug16       G. Story        Add C(urrent to top)
*       6Nov16       G. Story        Fix long titles
*       11Feb17      G. Story        Allow no sub-title (for more.py)
*       26May17      G. Story        Count filtered items
*       3Mar18       G. Story        Enhancements to saved position
*       4Mar18       G. Story        Reset saved position when searching
*       6Mar18       G. Story        Fix C99 warnings
*       18May18      G. Story        Allow "q" as an escape character
*       13Jun19      G. Story        Handle lists of more than a
*				     million entries
*
*       {@revision_history...@}
*
**********************************************************************/

/* Global Include Files */

#define _GNU_SOURCE
#include "gs.h"
#include "lscrn.h"
#include "hscrn.h"

/* Subsystem Global Include Files */


/* Local Include Files */


/* Local Defines */

typedef struct list_entry
{
    struct list_entry *l_next;
    struct list_entry *l_prev;
    char *l_data;
} list_entry;

/* Global Data Referenced */

static list_entry **entry_table;
static int entry_table_max_size;
static int entry_table_current_size;
static int previous_line;
static list_entry *previous_entry;
static int previous_rec;                 /*  previously current record      */
int screen_width;		       /*  Number of characters per line  */
int save_position = 1;
const void* save_list = NULL;

/* External Functions Referenced */
static void show_current (int record_nr, int line_nr);

/****************************************************************************
*****************************************************************************
*****************************************************************************

        Name:           set_position

        Purpose:        Set the save_position to a specified value

        Description:    1.  Set the global variable

        Arguments:      1.  list pointer
                        2.  new value 

        Processing:     See description

        Returns:        nothing

*****************************************************************************
*****************************************************************************
****************************************************************************/

void set_position(const void* list, int new_position)
{
	save_position = new_position;
	save_list = list;
}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        Name:           get_position

        Purpose:        get the save_position

        Description:    Return save_position if save_list matches

        Arguments:      1.  list

        Processing:     See description

        Returns:        The position

*****************************************************************************
*****************************************************************************
****************************************************************************/

int get_position(const void* list)
{
	if (list == save_list)
		return save_position;
	else
		return 1;
}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        Name:           filter

        Purpose:        See if current line matches pattern

        Description:    1.  See if pattern is contained in line
                        2.  See if wild-carded pattern is contained in
			    line

        Arguments:
                        1.  Line to be checked
                        2.  Pattern string
                        3.  Prompt string

        Processing:     See description

        Returns:        nothing

*****************************************************************************
*****************************************************************************
****************************************************************************/

int filter(char* line, char* search_string, char* search_prompt)
{
	if (search_string[0] != 0)
	{
		if (strcasestr (line, search_string)
		    == NULL)
		{
			if (search_string[0] != 0)
			{
				char search_pattern[80];
				int j, k;

				strcpy(search_pattern, "^*");

				for (
				     j = 0, k = 2;
				     search_string[j] != 0;
				     j++, k++)
					if (search_string[j] == ' ')
						search_pattern[k] = '*';
					else
						search_pattern[k] =
							search_string[j];

				search_pattern[k++] = '*';
				search_pattern[k] = 0;

				if (!patcmp(line,
					    search_pattern))
					return FALSE;
				else
					strcpy (search_prompt,
						  "Pattern search for:");

			}
			else
				return FALSE;
		}
		else
			strcpy (search_prompt, "String search for:");

	}
	return TRUE;
}


/**********************************************************************
*
* FUNCTION: list_pick
*
* DESCRIPTION:
*       This function lets the user pick an entry from a list
*
* CALLING SEQUENCE:
*       list_entry = list_pick (title, noun, verb, list_head)
*
* INPUT PARAMETERS:
*       title                 Program name or other screen title
*       noun                  The type of item to be manipulated to complete
*                             the phrase, "Which <noun> do you wish to <verb>?"
*       verb                  Word to complete the phrase, "Which <noun> do
*                             you wish to <verb>?"
*       list_head             Head of list to present
*       {@input_parameter_list...@}
*
*
* OUTPUT PARAMETERS:
*       None.
*       {@output_parameter_list...@}
*
* RETURNED VALUE:
*       void *          list entry selected or NULL
*                       {@return_value_description...@}
*
* EXTERNAL FUNCTIONS CALLED:
*       {@external_functions_called...@}
*
* GLOBAL DATA MODIFIED:
*       {@global_data_modified...@}
*
* PSEUDO CODE:
*       {@detailed_pseudo_code@}
*
**********************************************************************/

void * list_pick (const char *title, const char *noun, const char *verb,
		  const void *list_head)

{
	int dont_care;
	int position;

	if (list_head == save_list)
		position = save_position;
	else
		position = 1;
	return list_pick_ex(title, noun, verb, list_head, "", "", NULL, NULL,
			    FALSE, &dont_care, position, TRUE, NULL, NULL);
}

/**********************************************************************
*
* FUNCTION: list_pick_ex
*
* DESCRIPTION:
*       This function lets the user pick an entry from a list
*       In this extended form, the caller can add an extra
*       menu line and specify responses to be returned to the caller
*       for processing.
*
* CALLING SEQUENCE:
*       list_entry = list_pick (title, noun, verb, list_head, menu,
*       commands, command)
*
* INPUT PARAMETERS:
*       title                 Program name or other screen title
*       noun                  The type of item to be manipulated to complete
*                             the phrase, "Which <noun> do you wish to <verb>?"
*       verb                  Word to complete the phrase, "Which <noun> do
*                             you wish to <verb>?"
*       list_head             Head of list to present
*	menu                  An additional menu line supplied by caller
*	commands              Additional commands handled by caller
	callback              a callback function provided by the
			      caller that is called when a command in
			      commands is entered by the user
	state                 an opaque data structure that will be
			      passed to callback.
	return_is_edit        TRUE means return key = "e" command
	
*       The signature of the callback function is:
*
*       int callback(struct *list_entry, int command, void* state)
*
*       where the return value is an action code to be performed by
*       list_pick_ex:
*
*       0 = Continue displaying values
*       1 = Return to list_pick_ex's caller with the selected list entry
*       2 = Return to list_pick_ex's caller with NULL
*
*       list_entry is the structure currently selected by the user
*
*       command is the letter entered by the user is one of the letters
*       in the commands parameter to list_pick_ex.
*
*	state is the same opaque data structure originally passed to
*	list_pick_ex.
*
*	instruction is a string giving instructions to the user.  It
*	replaces the line that uses noun and verb.  Pass NULL to use
*	the noun/verb instruction.
*
*	filter is an initial value for the filter string.  Pass NULL or
*	an empty string to start with an empty filter.
*	
*       {@input_parameter_list...@}
*
*
* OUTPUT PARAMETERS:
*       last_key - the last key pressed by the user
*       {@output_parameter_list...@}
*
* RETURNED VALUE:
*       void *          list entry selected or NULL
*                       {@return_value_description...@}
*
* EXTERNAL FUNCTIONS CALLED:
*       {@external_functions_called...@}
*
* GLOBAL DATA MODIFIED:
*       {@global_data_modified...@}
*
* PSEUDO CODE:
*       {@detailed_pseudo_code@}
*
**********************************************************************/

void * list_pick_ex (const char *title,
		     const char *noun,
		     const char *verb,
		     const void *list_head,
		     const char* menu,
		     const char* commands,
		     int (*callback) (void* list_entry, int key, void* state),
		     void* state,
		     int return_is_edit,
		     int* last_key,
		     int top_rec,
		     int auto_return,
		     const char* instruction,
		     const char* filter_string)
{
    int key;                           /*  input key pressed              */
    int i;                             /*  loop counter                   */
    int current_rec;                   /*  highlighted record on screen   */
    int save_rec;                      /*  saved current_rec during search*/
    const int heading_lines = 10;      /*  save this many lines at top    */
    int current_line = heading_lines;  /*  current screen line            */
    int redisplay = FALSE;             /*  need to rebuild screen         */
    int max_rec = 0;                   /*  maximum record for line 1      */
    int screen_lines;		       /*  Number of lines on the screen  */
    int doing_up = FALSE;	       /*  TRUE if moving in up direction */
    list_entry *cur_entry;	       /*  Currently selected entry       */
    list_entry *cur_entry2;            /*  Used by filter counting        */
    char line4[180];		       /*  Display line 4                 */
    list_entry *my_list_head;	       /*  List of entries                */
    char search_string[80];            /*  Search string                  */
    int search_mode = FALSE;	       /*  TRUE if in search mode         */
    char display_data[800];	       /*  Contains partial line to be displayed */
    int selected_count;		       /*  Count of records selected      */
    int top_line;                      /*  Top line on the screen         */
    char search_prompt[80];            /*  Text to display for search     */
    char dont_care[80];                /*  Text to display for search     */
    int filter_count = 0;              /*  total count of matching lines  */

    /*
     *  Tell me when the window recizes
     */
    enum winch saved_winch = handle_winch(WINCH_NOTIFY);
    
    /*
     *  Set the search string
     */
    if (filter_string == NULL)
	    search_string[0] = 0;
    else
    {
	    strncpy(search_string, filter_string, 79);
	    search_string[79] = 0;
    }
    
    /*
     * Convert list-head pointer to internal format
     */
    
    my_list_head = (list_entry*)list_head;

    /*
     * There's no data in the list head
     */
    
    my_list_head->l_data = NULL;

    /*
     *  Count the number of entries in the list
     */
    
    for (cur_entry = my_list_head->l_next; cur_entry != my_list_head;
         cur_entry = cur_entry->l_next)
        ++max_rec;

    if (auto_return)
    {
	    /*
	     *  If the list is empty, then return NULL
	     */

	    if (max_rec == 0)
	    {
		    *last_key = '\033';
		    handle_winch(saved_winch);
		    return NULL;
	    }

	    /*
	     *  If there is only one entry in the list then return that entry
	     *  We don't want to do this if the user supplied extra commands
	     *  or set auto_return to false
	     */

	    if ((max_rec == 1) && (strlen(commands) == 0))	
	    {
		    *last_key = '\r';
		    handle_winch(saved_winch);
		    return my_list_head->l_next;
	    }
    }
    
    /*
     *  Allocate memory so we can create an array from the list
     */

    entry_table_max_size = max_rec + heading_lines;
    entry_table = (struct list_entry **) malloc (sizeof (struct list_entry *)
                                                 * entry_table_max_size);

    if (entry_table == NULL)
    {
        fprintf (stderr, "list_pick: Out of memory\n");
        exit (ERROR_RETURN);
    }

    /*
     *  Main loop - loop while the user navigates around in the list
     */

    strcpy (search_prompt, "String search for:");
    
    while (TRUE)
    {
	/*
	 *  Make sure the top_rec (top record to be displayed) is
	 *  within  range
	 */
	    
        if (top_rec > max_rec)
            top_rec = max_rec;

        if (top_rec < 1)
            top_rec = 1;

	/*
	 *  Compute the number of lines we can display at one time
	 */
	
        screen_lines = screen_size () - heading_lines;
	screen_width = line_size () - 2;
	/*
	 *  Display the lines at the top of the screen
	 */
	
	center (2, title);
	
	if (strlen(title) < (screen_width-35))
	{
		char c[30];
		sprintf(c, "%d", max_rec);
		saya (2, 1, "Entries:");
		saya (2, 10, c);
	}

	if (search_string[0] != 0)
	{
		char c[30];
		sprintf(c, "%d", filter_count);
		
		saya (3, 1, "Filtered:");
		saya (3, 10, c);
	}

	if (instruction != NULL)
	{
		strncpy(line4, instruction, 179);
		line4[179] = 0;
	}
	else if ((noun[0] != 0) || (verb[0] != 0))
	{
		sprintf (line4, "Which %s do you want to %s?", noun, verb);
	}
	else
		line4[0] = 0;

	center (4, line4);

	/*
	 *  Loop through the list for the entries before the first one
	 *  to be displayed
	 */
	selected_count = 0;
	for (cur_entry = my_list_head->l_next, current_rec = 1;
	     (current_rec < top_rec) && (cur_entry != my_list_head);
	     cur_entry = cur_entry->l_next, ++current_rec)
		;

	/*
	 *  Now loop through the entries to be displayed
	 */
	for (i = heading_lines; (i < screen_lines + heading_lines)
                     && (cur_entry != my_list_head);
             cur_entry = cur_entry->l_next)
	{
		/*
		 *  If the current entry doesn't match then skip it
		 */

		if (!filter(cur_entry->l_data, search_string, search_prompt))
		    continue;
		
		selected_count++;

		/*
		 *  Display the portion of the current entry that will
		 *  fit on the screen
		 */
		
		ztrim (cur_entry->l_data);
		if (strlen (cur_entry->l_data) > screen_width)
		{
			strncpy (display_data, cur_entry->l_data, screen_width);
			display_data[screen_width] = 0;
			saya (i, 1, display_data);
		}
		else
			saya (i, 1, cur_entry->l_data);

		normal();

		/*
		 *  Add the current entry to the array
		 */

		entry_table_current_size = i++;
		if (entry_table_current_size >= entry_table_max_size)
		{
			printf (
	    "\n\nentry_table_current_size (%d) >= entry_table_max_size(%d)\n",
				  entry_table_current_size,
				  entry_table_max_size);
			abort();
		}

		entry_table[entry_table_current_size] = cur_entry;
        }

	if (selected_count == 0)
	{
		saya (heading_lines + 3, 1, "<No lines selected>");
	}
	else
	{
		if (strlen(title) < (screen_width-35))
		{
			saya (2, screen_width-18, "Displayed:");
			sayn (2, screen_width-8, selected_count);
		}
		
		/*
		 *  Determine the initial values of current_rec (used to count
		 *  records in the list of entries) and current_line (the line
		 *  on the screen where line is displayed.  These values
		 *  determine which line is to be highlighted.
		 */

		if (doing_up)
		{
		    if (top_rec == 1)
		    {
			current_rec = (previous_rec <= 1) ? 1 : --previous_rec;
			current_line = heading_lines;
		    }
		    else
		    {
			current_rec = previous_rec - 1;
			current_line = i - 1;
		    }
		    doing_up = FALSE;
		}
		else
		{
		    current_rec = top_rec;
		    current_line = heading_lines;
		}

		/*
		 *  Since we're displaying a new screen there is no previous
		 *  line
		 */

		previous_line = 0;

		/*
		 *  Highlight the current line
		 */

		show_current (current_rec, current_line);

		/*
		 *  The following loop is repeated as long as we are within the
		 *  same page.  It processes keystrokes from the user.  When
		 *  the user selects a record (RETURN) or bails out (ESC) then
		 *  we return from inside this loop directly to the caller.  If
		 *  the user selects an option that changes pages of display,
		 *  then the loop is broken by setting redisplay to TRUE.  When
		 *  this inner loop breaks, the outer loop iterates to display
		 *  a new page.
		 */
	}
	
        redisplay = FALSE;

	while (!redisplay)
        {
		if (search_mode)
		{
			saya (6, 1, search_prompt);
			saya (6, 21, search_string);
			if (line4[0] == 0)
				saya (7, 1, "Escape = exit search mode");
			else
				saya (7, 1,
				 "Enter = selects; Escape = exit search mode");
			erase_line (8);
		}
		else
		{
			saya (6, 1,
			      "F(orward) P(revious) T(op) B(ottom) U(p) D(own) S(earch) C(urrent to top)");
			if (line4[0] == 0)
				saya (7, 1,
				      "Escape = Return to previous screen");
			else
				saya (7, 1,
		        "Enter = selects; Escape = Return to previous screen");
			saya (8, 1, menu);
		}
		
		key = getachar ();

		if ((key == 13) && return_is_edit)
		{
			search_mode = FALSE;
			key = 'e';
		}
				
            if (search_mode)
            {   /* We're in search mode */
    	        /*
		 *  If we're searching, we want to search from the top
		 */

		if (search_string[0] != 0)
		    top_rec = 1;

                switch (key)
                {
                case 27:  search_mode = FALSE;
                          redisplay = TRUE;
                          continue;

                case 0x5100:  /* PgDn */
                case 0x4900:  /* PgUp */
                case 0x4700:  /*  home */
                case 0x4F00:  /* end */
                case 0x5000:  /* down arrow */
                case 0x4800:  /* up arrow */
		case 13:      /* carriage return */
			search_mode = FALSE;
			break;  /* will fall through to non-search logic */

                default:  i = (int) strlen (search_string);
		          if ((key == 127 /* DEL */) 
			  ||  (key == 0x5300 /* delete */)
                          ||  (key == 8 /*backspace */))
			    search_string[i-1] = 0;
			  else 
			  {
			    search_string[i] = (char) key;
			    search_string [i+1] = 0;
			  }

			  /*  Now count the number of filtered lines */
			  filter_count = 0;
			  for (cur_entry2 = my_list_head->l_next;
			      (cur_entry2 != my_list_head);
			      cur_entry2 = cur_entry2->l_next)
				  if (filter(cur_entry2->l_data, search_string,
					     dont_care))
					  filter_count++;

			  redisplay = TRUE;
			  continue;
                }
            }   /* We're in search mode */

            if (key < 256)
                key = chlower (key);

	    *last_key = key;
            switch (key)
            {
		case 'q':
                case 27:    /* esc */
                            free (entry_table);
			    handle_winch(saved_winch);
                            return NULL;

		case 12:    /* ^L */
			    redisplay = TRUE;
			    break;
			    
		case 'c':   /* Current to top of screen */
			for (cur_entry = my_list_head->l_next,
			     top_rec = 1;
			     entry_table[current_line]->l_data !=
					     cur_entry->l_data;
			     ++top_rec, cur_entry = cur_entry->l_next)
				;
			    search_string[0] = 0;
			    redisplay = TRUE;
			    break;
			    
                case 0x5100:  /* PgDn */
                case 'f':   top_rec += screen_lines;
                            redisplay = TRUE;
                            break;

                case 0x4900:  /* PgUp */
                case 'p':   top_rec -= screen_lines;
                            if (top_rec < 1)
                                top_rec = 1;

                             redisplay = TRUE;
                             break;

                case 0x4700:  /*  home */
                case 't':    top_rec = 1;
                             redisplay = TRUE;
                             break;

                case 0x4F00:  /* end */
                case 'b':   top_rec = max_rec;
                            redisplay = TRUE;
                            break;

                case 0x5000:  /* down arrow */
                case 'n':
		case 'd':   if  (selected_count > 0)
			    {
				    if (++current_line < i) 
					show_current (++current_rec,
					    current_line);
				    else
				    {
					    top_rec += screen_lines;
					    redisplay = TRUE;
				    } /* else */
			    }
                            break;

                case 0x4800:  /* up arrow */
		case 'u':   if (selected_count > 0)
			    {
				    if (--current_line >= heading_lines)
					    show_current (--current_rec,
						    current_line);
				    else
				    {
					    top_rec -= screen_lines;
					    redisplay = TRUE;
					    doing_up = TRUE;
				    } /* else */
			    }
                            break;

                case 's':   /* search */
                            search_mode = TRUE;
                            redisplay = TRUE;
                            break;

                case 13:  /*  Return */
			if (line4[0] == 0)
				break;

			if (selected_count == 0)
				break;
			
			cur_entry = entry_table[current_line];
			free (entry_table);
			handle_winch(saved_winch);

			/*
			 *  Compute the record number within the linked
			 *  list to save for the next operation
			 */
			for (cur_entry2 = my_list_head->l_next,
			     top_rec = 1;
			     (cur_entry2 != cur_entry) &&
				(cur_entry2 != my_list_head);
			     ++top_rec, cur_entry2 = cur_entry2->l_next)
					     ;  /*  empty loop body */
  		        if (cur_entry2 == my_list_head)
			{
				fprintf(stderr,
				    "Couldn't find current entry in list");
				abort();
			}
			else
			{
				save_position = top_rec;
				save_list = list_head;
				return cur_entry;
			}
		default:  /* Look for user commands */
			if (callback == NULL)
				break;
			
			if (strchr(commands, key))
			{
				cur_entry = entry_table[current_line];
				int rc = (*callback)(cur_entry, key, state);
				redisplay = TRUE;
				top_rec = current_rec;
				
				if (rc == 0)
					break;

				free (entry_table);

				if (rc == 1)
				{
					handle_winch(saved_winch);
					return (cur_entry);
				}
				else if (rc == 2)
				{
					handle_winch(saved_winch);
					return NULL;
				}
				else
					fprintf (stderr, "Unexpected return code from callback\n");
					abort();
			}
				
            } /* switch */
        } /* while */
    } /* while */
    NO_EXIT ();
}

/****************************************************************************
*****************************************************************************
*****************************************************************************

        Name:           show_current

        Purpose:        highlights the current record

        Description:    1.  Unlightlight the old line
                        2.  Highlight the current line

        Arguments:
                        1.  Current record number
                        2.  Current line number

        Processing:     See description

        Returns:        nothing

*****************************************************************************
*****************************************************************************
****************************************************************************/

static void show_current (int record_nr, int line_nr)

{
    char display_data[800];

    if (line_nr > entry_table_current_size)
    {
	    printf ("\n\nline_nr (%d) > entry_table_current_size (%d)\n",
		      line_nr, entry_table_current_size);
	    abort();
    }
    
    if (previous_line)
    {
        gotoxy (1, previous_line);
	normal ();
	strncpy (display_data, previous_entry->l_data, screen_width);
	display_data[screen_width] = 0;
	ztrim (display_data);
	saya (previous_line, 1, display_data);
    } /* if */

    reverse ();
    strncpy (display_data, entry_table[line_nr]->l_data, screen_width);
    display_data[screen_width] = 0;
    ztrim (display_data);
    saya (line_nr, 1, display_data);
    normal ();

    previous_line = line_nr;
    previous_entry = entry_table[line_nr];
    previous_rec = record_nr;

}  /* show_current  */
