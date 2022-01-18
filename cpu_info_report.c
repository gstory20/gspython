#include "gs.h"
#include "dbase3.h"
#include "rpt.h"

void print_report (void)
{
    char db_filename[256];
    struct rpt* report;
    struct dbcb* dbf_file;
    char hostname[21];
    char collected[11];
    int cores;
    int threads;
    int processors;
    char model[66];
    char speed[16];
    char type[16];
    char vendor[16];
    char memsize[16];
    char cache[16];
    char os_descr[66];
    char vm[16];

    report = rpt_init ();
    if (report == NULL)
        return;

    rpt_s_head (report, "CPU Info Database Report");
    rpt_s_col (report, "HOSTNAME     ", "%12s ", (char) 's', hostname);
    rpt_s_col (report, "CORES ", "%5i ", (char) 'i', &cores);
    rpt_s_col (report, "THREADS ", "%7i ", (char) 'i', &threads);
    rpt_s_col (report, "SPEED      ", "%8s ", (char) 's', speed);
    rpt_s_col (report, "TYPE     ", "%6s ", (char) 's', type);
    rpt_s_col (report, "MEMSIZE       ", "%11s ", (char) 's', memsize);
    rpt_s_col (report, "CACHE           ", "%15s ", (char) 's', cache);

    sprintf (db_filename, "%s/gs/dat/cpu_info.dbf", getenv("HOME"));
    dbf_file = dbopen (db_filename, 0);
    if (dbf_file == (struct dbcb*) ERROR)
        dberror ("Can't open database file");

    do
    {
        if (dbgetc (dbf_file, "HOSTNAME", hostname) == ERROR)
            dberror ("Can't get HOSTNAME");
	trim(hostname);

        if (dbgetn (dbf_file, "CORES", &cores) == ERROR)
            dberror ("Can't get CORES");

	if (dbgetn (dbf_file, "THREADS", &threads) == ERROR)
		dberror ("Can't get THREADS");
	
        if (dbgetn (dbf_file, "PROCESSORS", &processors) == ERROR)
            dberror ("Can't get PROCESSORS");

        if (dbgetc (dbf_file, "MODEL", model) == ERROR)
            dberror ("Can't get MODEL");
	trim(model);

        if (dbgetc (dbf_file, "SPEED", speed) == ERROR)
            dberror ("Can't get SPEED");
	trim(speed);

        if (dbgetc (dbf_file, "TYPE", type) == ERROR)
            dberror ("Can't get TYPE");

	trim(type);

        if (dbgetc (dbf_file, "MEMSIZE", memsize) == ERROR)
            dberror ("Can't get MEMSIZE");
	trim(memsize);
	
        if (dbgetc (dbf_file, "CACHE", cache) == ERROR)
            dberror ("Can't get CACHE");
	trim(cache);

        if (dbgetc (dbf_file, "OS_DESCR", os_descr) == ERROR)
            dberror ("Can't get OS_DESCR");

        if (dbgetc (dbf_file, "VM", vm) == ERROR)
            dberror ("Can't get VM");

        rpt_print (report);
	rpt_print_line (report, model);
	rpt_print_line (report, os_descr);
	rpt_print_line (report, "");
    }
    while (dbskip (dbf_file, 1) != ERROR);

    rpt_term (report);
    dbclose (dbf_file);
}
