#include "../libmpeg3.h"
#include "../mpeg3protos.h"

#include <stdio.h>
#include <string.h>

int mpeg3_mmx_test()
{
	int result = 0;
	FILE *proc;
	char string[MPEG3_STRLEN];


#ifdef HAVE_MMX
	if(!(proc = fopen(MPEG3_PROC_CPUINFO, "r")))
	{
		return 0;
	}
	
	while(!feof(proc))
	{
		fgets(string, MPEG3_STRLEN, proc);
/* Got the flags line */
		if(!strncmp(string, "flags", 5))
		{
			char *needle;
			needle = strstr(string, "mmx");
			if(!needle) return 0;
			if(!strncmp(needle, "mmx", 3)) return 1;
		}
	}
#endif

	return 0;
}
