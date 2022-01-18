/**********************************************************************
*
* MODULE: listutl.c
*
* BRIEF DESCRIPTION:
*       This file contains list utility routines .
*
* MODULE TYPE:
*       Source
*
* FULL DESCRIPTION:
*       {@full_description_of_the_module@}
*
* FUNCTIONS DEFINED:
*       list_add_tail                   Add an item to the end of the list
*       list_purge                      puge all items in a list
*       {@functions_defined...@}
*
* GLOBAL DATA DEFINED:
*       {@global_data_defined...@}
*
* REVISION HISTORY:
*       23Sep94      G. Story        Original release
*       {@revision_history...@}
*
**********************************************************************/

/* Global Include Files */

/*  This is a portable source file so we can't use Microsoft's security-enhanced CRT */
#define _CRT_SECURE_NO_DEPRECATE

#include "gs.h"

/* Subsystem Global Include Files */


/* Local Include Files */


/* Local Defines */


/* Global Data Referenced */


/* External Functions Referenced */



/**********************************************************************
*
* FUNCTION: list_add_tail
*
* DESCRIPTION:
*       This function will add an entry at the tail of a list
*
* CALLING SEQUENCE:
*       list_add_tail (list_head, list_entry, descr)
*
* INPUT PARAMETERS:
*       list_head             The list head to which to add the entry
*       list_entry            The entry to add
*       descr                 A character string to add to the list
*
*
* OUTPUT PARAMETERS:
*       None.
*
* RETURNED VALUE:
*       None.
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

void list_add_tail (void *list_head, void *list_entry, const char *descr)
{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_head, *my_list_entry;

    my_list_head = list_head;
    my_list_entry = list_entry;

    my_list_entry->l_descr = malloc (strlen (descr) + 1);
    if (my_list_entry->l_descr == NULL)
    {
        fprintf (stderr, "Out of memory (list_add_tail)\n");
        exit (ERROR_RETURN);
    }

    strcpy (my_list_entry->l_descr, descr);

    my_list_entry->l_prev         = my_list_head->l_prev;
    my_list_entry->l_prev->l_next = my_list_entry;
    my_list_entry->l_next         = my_list_head;
    my_list_head->l_prev          = my_list_entry;
}



/**********************************************************************
*
* FUNCTION: list_add_head
*
* DESCRIPTION:
*       This function will add an entry at the head of a list
*
* CALLING SEQUENCE:
*       list_add_head (list_head, list_entry, descr)
*
* INPUT PARAMETERS:
*       list_head             The list head to which to add the entry
*       list_entry            The entry to add
*       descr                 A character string to add to the list
*
*
* OUTPUT PARAMETERS:
*       None.
*
* RETURNED VALUE:
*       None.
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

void list_add_head (void *list_head, void *list_entry, const char *descr)
{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_head, *my_list_entry;

    my_list_head = list_head;
    my_list_entry = list_entry;

    my_list_entry->l_descr = malloc (strlen (descr) + 1);
    if (my_list_entry->l_descr == NULL)
    {
        fprintf (stderr, "Out of memory (list_add_head)\n");
        exit (ERROR_RETURN);
    }

    strcpy (my_list_entry->l_descr, descr);

    my_list_entry->l_prev         = my_list_head;
    my_list_head->l_next->l_prev  = my_list_entry;
    my_list_entry->l_next         = my_list_head->l_next;;
    my_list_head->l_next          = my_list_entry;
}


/**********************************************************************
*
* FUNCTION: list_add_ascending
*
* DESCRIPTION:
*       This function will add an entry in ascending alphabetical order
*       within the list
*
* CALLING SEQUENCE:
*       list_add_ascending (list_head, list_entry, descr)
*
* INPUT PARAMETERS:
*       list_head             The list head to which to add the entry
*       list_entry            The entry to add
*       descr                 A character string to add to the list
*
*
* OUTPUT PARAMETERS:
*       None.
*
* RETURNED VALUE:
*       None.
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

void list_add_ascending (void *list_head, void *list_entry, const char *descr)
{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_head, *my_list_entry, *current_entry;

    my_list_head = list_head;
    my_list_entry = list_entry;

    my_list_entry->l_descr = malloc (strlen (descr) + 1);
    if (my_list_entry->l_descr == NULL)
    {
        fprintf (stderr, "Out of memory (list_add_ascending)\n");
        exit (ERROR_RETURN);
    }

    strcpy (my_list_entry->l_descr, descr);

    for (current_entry = my_list_head->l_next; current_entry != my_list_head;
         current_entry = current_entry->l_next)
    {  /* loop through list */
        if (strcmp (current_entry->l_descr, descr) > 0)
        {  /* Found place to insert */
            my_list_entry->l_next = current_entry;
            my_list_entry->l_prev = current_entry->l_prev;

            current_entry->l_prev->l_next = my_list_entry;
            current_entry->l_prev = my_list_entry;

            return;
        }  /* Found place to insert */
    }  /* loop through list */

    /*
     *  If we got this far, the list is empty or the new record is greater
     *  than any other
     */

    free (my_list_entry->l_descr); /* Will be reallocated in list_add_tail */
    list_add_tail (list_head, list_entry, descr);

}

/**********************************************************************
*
* FUNCTION: list_find_ascending
*
* DESCRIPTION:
*       This function will finds an entry in ascending alphabetical order
*       within the list
*
* CALLING SEQUENCE:
*       list_find_ascending (list_head, list_entry, descr)
*
* INPUT PARAMETERS:
*       list_head             The list head to which to add the entry
*       descr                 A character string to add to the list
*
*
* OUTPUT PARAMETERS:
*       None.
*
* RETURNED VALUE:
*       Pointer to found entry or NULL
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

void *list_find_ascending (void *list_head, const char *descr)
{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_head, *current_entry;

    int result;

    my_list_head = list_head;

    for (current_entry = my_list_head->l_next; current_entry != my_list_head;
         current_entry = current_entry->l_next)
    {  /* loop through list */
        result = strcmp (current_entry->l_descr, descr);

        if (result > 0)
            return NULL;

        if (result == 0)
            return current_entry;
    }

    return NULL;
}



/**********************************************************************
*
* FUNCTION: list_purge
*
* DESCRIPTION:
*       Purge all entries in a list
*
* CALLING SEQUENCE:
*       list_purge (list_head)
*
* INPUT PARAMETERS:
*       list_head             Root list list to purge
*
*
* OUTPUT PARAMETERS:
*       None.
*
* RETURNED VALUE:
*       None.
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

void list_purge (void *list_head)
{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_head, *cur_list_entry, *next_list_entry;

    my_list_head = list_head;

    for (cur_list_entry = my_list_head->l_next; cur_list_entry != my_list_head;
         cur_list_entry = next_list_entry)
    {
        next_list_entry = cur_list_entry->l_next;
        free (cur_list_entry->l_descr);
        free (cur_list_entry);
    }

    my_list_head->l_next = my_list_head->l_prev = my_list_head;
}

/****************************************************************************

        NAME:       list_remove_entry

        Purpose:    To delete a single entry from a double-linked list

        Arguments:  1.  list_entry:  IN:  entry to delete

        Processing: Unlink the entry (no memory is freed)

        Returns:    Nothing

****************************************************************************/

void list_remove_entry (void *list_entry)

{
    struct list
    {
        struct list *l_next;
        struct list *l_prev;
        char *l_descr;
    } *my_list_entry;

    my_list_entry = list_entry;

    my_list_entry->l_next->l_prev = my_list_entry->l_prev;
    my_list_entry->l_prev->l_next = my_list_entry->l_next;

}  /*  list_remove_entry  */

