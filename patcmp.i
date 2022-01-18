%module patcmp
   %include typemaps.i

   int patcmp (const char *string, const char *pattern);
   const char* patcmp_help_text(void);
