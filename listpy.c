/****************************************************************************

        PROGRAM DESCRIPTION

        NAME            /home/glenns/src/listpy.c

        AUTHOR          Glenn Story

        DATE WRITTEN    02/21/2011 15:48:14

        DESCRIPTION     Python wrapper for list_pick

        MODIFICATION HISTORY

        {@change_history...@}

 ****************************************************************************/


/****************************************************************************

        Includes

 ****************************************************************************/

#include "gs.h"

/****************************************************************************

        EQUATES

 ****************************************************************************/

/****************************************************************************

        MACRO DEFINITIONS

 ****************************************************************************/

/****************************************************************************

        STRUCTURES

 ****************************************************************************/

struct py_list_element
{
	struct py_list_element* next;
	struct py_list_element* prev;
	char* key;
	int line_number;
};


/****************************************************************************

        TYPEDEFS

 ****************************************************************************/

/****************************************************************************

        Global data

 ****************************************************************************/

int selected_line_number = -1;
int current_line_number = 0;

/****************************************************************************

        Routines defined in other modules

 ****************************************************************************/

/****************************************************************************

        Routines defined in this module

 ****************************************************************************/


/*^*/

void* pylist_start(void)
{
	struct py_list_element* py_list;

	py_list = malloc (sizeof(struct py_list_element));
	if (py_list != NULL)
		py_list->next = py_list->prev = py_list;

	current_line_number = 0;
	
	return py_list;
}

int py_list_add(void* py_list, const char* key)
{

	struct py_list_element* new_element
			= malloc (sizeof(struct py_list_element));

	if (new_element == NULL)
		return FALSE;

	new_element->line_number = ++current_line_number;
	
	list_add_tail (py_list, new_element, key);

	return TRUE;
}

char* py_list_pick(const char* title, const char* noun, const char* verb,
		   const void* list_head)
{
	struct py_list_element* selected_entry;

	/*  This is needed because there ends up being two copies of
	 *  getachar.c in the executalbe--one linked to listpick and
	 *  one lisinked to pyscrn.  They each have their own state so
	 *  we have to put "our" copy into the right state
	 */
	
	reset_terminal();
	
	selected_entry = list_pick (title, noun, verb, list_head);

	if (selected_entry == NULL)
		return NULL;

	selected_line_number = selected_entry->line_number;
	return selected_entry->key;
}

int py_get_selected_line_number(void)
{
	return selected_line_number;
}

void py_list_end(void* list_head)
{
	list_purge (list_head);
	free (list_head);
}
