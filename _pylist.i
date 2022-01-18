%module _pylist
   %include typemaps.i

   %{
#include "pylist.h"
	%}

   void* pylist_start(void);

int py_list_add(void* py_list, const char* key);

char* py_list_pick(const char* title, const char* noun, const char* verb,
		   const void* list_head);

void py_list_end(const void* list_head);

void set_position(const void* list_head, int new_position);

int get_position(const void*list_head);

int py_get_selected_line_number(void);
