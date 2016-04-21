#include <stdio.h>
#include "stack_heap.h"
#include <stdlib.h>

int main(int argc, char*argv[])
{
    void * memory = malloc(10000);

    sth_ctx_t *heap = sth_init(memory, 10000, NULL);
    sth_dbg_init(heap, printf);
    sth_set_name(heap, "main");

    sth_print_stat(heap);

    void *ptr = sth_malloc(heap, 40, 2, __FILE__, __FUNCTION__, __LINE__);

    sth_print_stat(heap);

    void *ptr1 = sth_malloc(heap, 50, 4, __FILE__, __FUNCTION__, __LINE__);

    sth_print_stat(heap);

    sth_free(heap, ptr, __FILE__, __FUNCTION__, __LINE__);
    sth_free(heap, ptr, __FILE__, __FUNCTION__, __LINE__);

    sth_print_stat(heap);

    sth_free(heap, ptr1, __FILE__, __FUNCTION__, __LINE__);

    sth_print_stat(heap);

    sth_stat_t stat = sth_get_stat(heap);
    printf("heap: %s\n",stat.heap_name);
    printf("blocks_amount = %d\n", stat.blocks_amount);
    printf("blocks_occupied = %d\n",stat.blocks_occupied);
    printf("size = %d\n",stat.size);
    printf("size_occupied = %d\n",stat.size_occupied);

    free(memory);
}
