/*
 *  settitle.c
 *
 *  Glenn Story 00 3/19/6
 *
 */

#include "gs.h" 

void set_title_no_reset (const char* title)
{
	static char* term = NULL;

	if (term == NULL)
		term = getenv ("TERM");

	if ((term != NULL) && (strncmp (term, "xterm", 5) == 0))
	{
		printf ("\033]0;%s\007", title);
		fflush (stdout);
	}
}

void reset_title (void)
{
	char title[500];
	char* hostname;
	char* path;
	char* prefix;
	
	hostname = get_hostname ();

	path = getenv ("PWD");

	prefix = getenv ("PS_PREFIX");

	if (prefix == NULL)
		prefix = "";
		 
	sprintf (title, "%s %s %s", hostname, prefix, path);

	set_title_no_reset (title);
}

void set_title (const char* title)
{
	atexit(reset_title);
	set_title_no_reset (title);
}

