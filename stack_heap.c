#include "stack_heap.h"
#include "str_hash.h"
#include <stddef.h>
#include "time_plat.h"


static int sth_heap_debuggable(sth_ctx_t * heap)
{
	if (heap)
	{
		if (heap->dbg_ctx)
		{
			return 1;
		}
	}
	return 0;
}

static ht_key_t sth_reg_text(sth_ctx_t * heap, const char * const text)
{
	ht_key_t key;
	key.auto_inc = 0;
	key.hash = 0;
	if (sth_heap_debuggable(heap))
	{
		key = ht_reg_string(text, heap->dbg_ctx->hash_table, heap->dbg_ctx->hash_table_size, &heap->dbg_ctx->max_key);
	}
	return key;
}

/*
 * Name of the function is derived from STack Heap INITialization
 */
sth_ctx_t * sth_init(void * address, unsigned long size, sth_ctx_t *parent)
{
    sth_dbg_ctx_t * dbg_ctx;
    sth_ctx_t * res = (sth_ctx_t *) address;
    if (size < (sizeof(sth_ctx_t)+sizeof(sth_rec_t)+MINIMAL_HEAP_SIZE))
    {
    	return NULL;
    }
    res->header_pattern = STHH_CTX;
    res->start_address = address;
    res->size = size;
    res->parent = parent;
    res->dbg_ctx = NULL;
    res->first_record = (sth_rec_t *)(((char *)address)+sizeof(sth_ctx_t));
    //res->root_occupied = NULL;
    while(parent)
    {
        if(parent->dbg_ctx) 
        {
            res->dbg_ctx = parent->dbg_ctx;
        }
        else
        {
            parent=parent->parent;
        }
    }
    res->first_record->block_size = size - sizeof(sth_ctx_t);
    res->first_record->file_key.auto_inc = 0;
    res->first_record->file_key.hash = 0;
    res->first_record->func_key.auto_inc = 0;
    res->first_record->func_key.hash = 0;
    res->first_record->header_pattern = STHH_REC;
    res->first_record->line_number = 0;
    res->first_record->parent_heap = res;
    res->first_record->returned_ptr = NULL;
    res->first_record->returned_size = 0;
    res->first_record->time = 0;
    res->first_record->f_desc_amount = 0;
    res->first_record->f_occupied = 0;
    return res;
}

static int fits_by_size(sth_rec_t *block, unsigned long size)
{
	if (block)
	{
		if ((block->block_size - sizeof(sth_rec_t)) >= size)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

/*
 * Name of this function is derived from STack Heap DeBuG INITialization
 * and means initialization of debug subsystem of stack heap facility
 */
void sth_dbg_init(sth_ctx_t * heap, printf_t pf)
{
    sth_dbg_ctx_t * res = sth_malloc(heap, sizeof(sth_dbg_ctx_t), 1, __FILE__, __FUNCTION__, __LINE__);
    int allocation_line = __LINE__ - 1;
    heap->dbg_ctx = res; 
    res->header_pattern = STHH_DBG_CTX;
    res->hash_table_size = STH_DBG_HASH_TAB_SIZE ;
    res->heap = heap;
    res->pf = pf;
    res->max_key = 0;
    int i;
    for (i = 0; i < STH_DBG_HASH_TAB_SIZE; i++)
    {
        res->hash_table[i].chain = NULL;
    }
    heap->first_record->file_key = ht_reg_string(__FILE__, res->hash_table, res->hash_table_size, &res->max_key);
    heap->first_record->func_key = ht_reg_string(__FUNCTION__, res->hash_table, res->hash_table_size, &res->max_key);
    heap->first_record->line_number = allocation_line;
}

int sth_set_name(sth_ctx_t * heap, const char * const name)
{
	if (sth_heap_debuggable(heap))
	{
       heap->name = sth_reg_text(heap, name);
       return 1;
	}
	return 0;
}

static sth_rec_t *find_smallest_fit(sth_ctx_t *heap, unsigned size)
{
    if (heap == NULL)
    {
    	return NULL;
    }
    sth_rec_t *res = NULL;
    sth_rec_t *cur = heap->first_record;
    unsigned best_size = (unsigned) -1;
    while(cur != NULL)
    {
    	if(fits_by_size(cur, size) && cur->block_size < best_size)
    	{
    		res = cur;
    		best_size = res->block_size;
    	}
    	cur = cur->next_block;
    }
    return res;
}

static void insert_block_after(sth_rec_t *block, sth_rec_t *inserted_block)
{
    inserted_block->next_block = block->next_block;
    inserted_block->prev_block = block;
    if (block->next_block)
    {
        block->next_block->prev_block = inserted_block;
    }
    block->next_block = inserted_block;
}

/*
 * Name of the function is derived from STack Heap MALLOC
 */
void * sth_malloc(sth_ctx_t * heap, unsigned long size, int align, const char * const file_name, const char * const fn_name, const unsigned line_idx)
{
    //we should take the smallest fit free block, it is located as close to root as possible
    //find_most_distant_fit_by_size(heap->root_free, size+align);
    sth_rec_t *free_block = find_smallest_fit(heap, size+align);
    unsigned long original_free_block_size = free_block->block_size;
    unsigned long address = (unsigned long)(free_block) + sizeof(*free_block) + align-1;
    address = address - address % align; //getting aligned address
    unsigned long alignment_loses = address - (unsigned long)(free_block) - sizeof(*free_block);
    if ((free_block->block_size - size - alignment_loses - sizeof(*free_block)) > sizeof(sth_rec_t))
    {
    	//we can devide this free block into 2 blocks: occupied and free
        sth_rec_t *new_free_block = (sth_rec_t *)(address + size);
        insert_block_after(free_block, new_free_block);

        new_free_block->block_size = original_free_block_size - free_block->block_size;
        new_free_block->header_pattern = STHH_REC;
        new_free_block->f_occupied = 0;
        new_free_block->file_key.auto_inc = 0;
        new_free_block->file_key.hash = 0;
        new_free_block->func_key.auto_inc = 0;
        new_free_block->func_key.hash = 0;
        new_free_block->line_number = 0;
        new_free_block->parent_heap = heap;
        new_free_block->returned_ptr = NULL;
        new_free_block->returned_size = 0;
        new_free_block->time = 0;
    }
    free_block->block_size = sizeof(sth_rec_t) + alignment_loses + size;
    free_block->f_occupied = 1;
    free_block->file_key = sth_reg_text(heap, file_name);
    free_block->func_key = sth_reg_text(heap, fn_name);
    free_block->line_number = line_idx;
    free_block->returned_ptr = (void *) address + alignment_loses;
    free_block->returned_size = size;
    free_block->time = plat_get_time();
    return (void*) address;
}

static int check_block(sth_rec_t *block)
{
	if (!block)
	{
		return 0;
	}
	if (block->header_pattern != STHH_REC)
	{
		return 0;
	}
	if (block->next_block)
	{
		if (block->next_block->header_pattern != STHH_REC)
		{
			return 0;
		}
	}
	if (block->prev_block)
	{
		if (block->prev_block->header_pattern != STHH_REC)
		{
		return 0;
		}
	}
	return 1;
}

static sth_rec_t * find_block_on_ptr(void *ptr)
{
    int i;
    sth_rec_t * res;
    if (!ptr) return NULL;

    char *cursor = (char *)ptr - sizeof(sth_rec_t);

    for (i = 0; i < MAX_ALIGNMENT; i++)
    {
        if (check_block((sth_rec_t*)cursor))
        {
        	return (sth_rec_t *)cursor;
        }
        cursor--;
    }
    return res;
}

static int can_print(sth_ctx_t * heap)
{
	if (heap)
	{
		if (heap->dbg_ctx)
		{
			if (heap->dbg_ctx->pf)
			{
				return 1;
			}
		}
	}
	return 0;
}

static void sth_print_msg(sth_ctx_t * heap,
		                  const char * const file_name,
		                  const char * const fn_name,
		                  const int line_idx,
		                  const char * const msg)
{
	char * file = "file not specified,";
	char * fn = "func not specified,";
	if (can_print(heap))
	{
		if (file_name)
		{
			file = (char *)file_name;
		}
		if (fn_name)
		{
			fn = (char *)fn_name;
		}

		if (line_idx < 0)
		{
			heap->dbg_ctx->pf("%s, %s:line is not specified (%s) - %s",
				plat_sprint_time(plat_get_time()),
				file,
				fn,
				msg);
		}
		else
		{
			heap->dbg_ctx->pf("%s, %s:line %d (%s) - %s\n",
				plat_sprint_time(plat_get_time()),
				file,
				line_idx,
				fn,
				msg);
		}
	}
}

static void sth_merge_blocks(sth_rec_t *first, sth_rec_t *second)
{
	if (!first)
	{
		return;
	}
	if (!second)
	{
		return;
	}
	first->block_size += second->block_size;
	first->next_block = second->next_block;
	if (second->next_block)
	{
		second->next_block->prev_block = first;
	}
	second->header_pattern = STHH_UNUSED;
	second->next_block = NULL;
	second->prev_block = NULL;
	second->block_size = 0;
	second->file_key.auto_inc = 0;
	second->file_key.hash = 0;
	second->func_key.auto_inc = 0;
	second->func_key.hash = 0;
	second->line_number = 0;
	second->f_occupied = 0;
}

/*
 * Name of the function is derived from STack Heap FREE
 */
void      sth_free(sth_ctx_t * heap, void * ptr, const char *const file_name, const char *const fn_name, const unsigned line_idx)
{
	if (heap == NULL) return;
	if (ptr == NULL)
	{
		sth_print_msg(heap, file_name, fn_name, line_idx, "attempt to free NULL pointer");
		return;
	}
    //sth_rec_t *parent_node = find_rootest_most_left_unbalanced_branch(heap->root_free);
	sth_rec_t *freeing_block = find_block_on_ptr(ptr);
    if (!check_block(freeing_block))
    {
    	sth_print_msg(heap, file_name, fn_name, line_idx, "freeing block is damaged");
    	return;
    }

	if (!freeing_block->f_occupied)
    {
    	sth_print_msg(heap, file_name, fn_name, line_idx, "attempt to free the same memory twice");
    	return;
    }
	freeing_block->f_occupied = 0;

	if (freeing_block->next_block)
	{
		if (!freeing_block->next_block->f_occupied)
		{
			sth_merge_blocks(freeing_block, freeing_block->next_block);
		}
	}

	if (freeing_block->prev_block)
	{
		if (!freeing_block->prev_block->f_occupied)
		{
			sth_merge_blocks(freeing_block->prev_block, freeing_block);
		}
	}
}

/*
 * Name of the function is derived from STack Heap STATistics
 */
void      sth_print_stat(sth_ctx_t * heap)
{
	ht_rec_t *hash_table = NULL;
    printf_t pf = heap->dbg_ctx->pf;
    unsigned long total_occupied = 0;

    if (can_print(heap))
    {
        hash_table = heap->dbg_ctx->hash_table;
        pf("Statistics for heap '%s'\n",ht_get_string(heap->name, hash_table));
    }
    else
    {
    	return;
    }
    sth_rec_t *cur = heap->first_record;
    pf("pointer      function                         file                 line          time\n");
    while (cur)
    {
    	if (cur->f_occupied)
    	{
    		total_occupied += cur->block_size;
    		char * fn_name = ht_get_string(cur->func_key, hash_table);
    		char * file_name = ht_get_string(cur->file_key, hash_table);
    		char * time = plat_sprint_time(cur->time);
    		if ((fn_name) && (file_name))
    		{
        		pf("%-10p : %-32s(%-20s:%d) allocated @ %s", cur->returned_ptr, fn_name, file_name, cur->line_number, time);
    		}
    	}
    	cur = cur->next_block;
    }
    pf("================================================\n");
    pf("Heap takes %d : occupied %d (%d\%)\n",heap->size, total_occupied, total_occupied*100/heap->size);
}

sth_stat_t sth_get_stat(sth_ctx_t * heap)
{
	ht_rec_t *hash_table = NULL;
    printf_t pf = heap->dbg_ctx->pf;
    sth_stat_t res;
    res.blocks_amount = 0;
    res.blocks_occupied = 0;
    res.heap_name = NULL;
    res.size = heap->memory_size;
    res.size_occupied = 0;


    sth_rec_t *cur = heap->first_record;
    while (cur)
    {
    	if (cur->f_occupied)
    	{
    		res.size_occupied += cur->block_size;
    		res.blocks_occupied++;
    	}
		res.blocks_amount++;
    	cur = cur->next_block;
    }
    if (heap && heap->dbg_ctx)
    {
        hash_table = heap->dbg_ctx->hash_table;
        res.heap_name = ht_get_string(heap->name,heap->dbg_ctx->hash_table);
    }
    return res;
}



