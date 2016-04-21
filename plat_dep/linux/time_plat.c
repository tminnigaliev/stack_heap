#include "time_plat.h"
#include <time.h>
#include <stddef.h>

unsigned long plat_get_time(void)
{
	return (unsigned int)time(NULL);
}

char * plat_sprint_time(unsigned long time)
{
	return ctime((time_t *)&time);
}
