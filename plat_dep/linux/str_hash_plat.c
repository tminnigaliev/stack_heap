#include <stdlib.h>

void * str_hash_malloc(unsigned size)
{
	return malloc(size);
}

void str_hash_free(void *ptr)
{
	free(ptr);
}
