#ifndef STACK_HEAP_H_
#define STACK_HEAP_H_
#include "sth_types.h"
#include "str_hash.h"

struct sth_ctx_tag;

typedef int(*printf_t)(const char * format, ...);

/*
 * define's name derived from STack Heap HASH TABLE SIZE
 */
#define STH_DBG_HASH_TAB_SIZE      127

#define MINIMAL_HEAP_SIZE          20

#define MAX_ALIGNMENT              16

/*
 * struct's name derived from STack Heap DeBuG ConTeXt Type
 */
typedef struct
{
    sth_header_t          header_pattern;
    unsigned              hash_table_size;
    struct                sth_ctx_tag * heap;
    unsigned              max_key;
    printf_t              pf;
    ht_rec_t hash_table[STH_DBG_HASH_TAB_SIZE];
}sth_dbg_ctx_t;


/*
 * struct's name is derived from STack Heap RECord Type
 */
typedef struct sth_rec_tag
{
	sth_header_t   header_pattern;        //special pattern (as a measure to check integrity)
	unsigned long  block_size;            //size of block including memory and this header
	struct sth_ctx_tag *parent_heap;      //pointer to heap
	unsigned  line_number;                //number of line where allocation occured
	ht_key_t  func_key;                   //key for getting function name
	ht_key_t  file_key;                   //key for getting filename
	unsigned long  time;                  //allocation time
	struct sth_rec_tag *next_block;       //pointer to next block in memory (necessary for gathering freed blocks)
	struct sth_rec_tag *prev_block;       //pointer to previous block in memory (necessary for gathering freed blocks)
	unsigned long  returned_size;         //size of segment, requested by user
	void           *returned_ptr;         //pointer to returned segment
	unsigned int   f_occupied:1;          //flag occupied
	unsigned int   f_desc_amount:11;      //field amount of descendants
}sth_rec_t;

/*
 * struct's name is derived from STack Heap ConTeXt Type
 */
typedef struct sth_ctx_tag
{
    sth_header_t    header_pattern;
    struct sth_ctx_tag    *start_address;
    unsigned long   size;
    ht_key_t        name;
    sth_dbg_ctx_t   *dbg_ctx;
    struct sth_ctx_tag * parent;
    unsigned long   memory_size;
    //sth_rec_t       *root_occupied;
    sth_rec_t       *first_record;

    char            memory[0];
}sth_ctx_t;


/*
 *
 *
 */
typedef struct sth_stat_tag
{
	const char * heap_name;
	unsigned int blocks_amount;
	unsigned int blocks_occupied;
	unsigned long size;
	unsigned long size_occupied;
}sth_stat_t;

/*
 *  sth_init creates new heap at the place you specified via parameters:
 *   address
 *   size
 *
 *   parent may be used, if you create heaps recursively, to specify parent heap.
 */
sth_ctx_t * sth_init(void * address, unsigned long size, sth_ctx_t *parent);

/*
 *  sth_set_name permits to set name for heap (this may be useful for debug purposes)
 */
int         sth_set_name(sth_ctx_t * heap, const char * const name);

/*
 *  sth_dbg_init initializes debug subsystem for this heap
 *  if you have a set of nested heaps, the debug subsystem should be initialized only for root heap
 */
void      sth_dbg_init(sth_ctx_t * heap, printf_t pf);

/*
 *  sth_malloc does the same as normal malloc does
 */
void    * sth_malloc(sth_ctx_t * heap, unsigned long size, int align, const char * const file_name, const char * const fn_name, const unsigned line_idx);

/*
 *  sth_free does the same as normal free does
 */
void      sth_free(sth_ctx_t * heap, void * ptr, const char *const file_name, const char *const fn_name, const unsigned line_idx);

/*
 *  sth_stat prints statistics about non freed pointers
 */
void      sth_print_stat(sth_ctx_t * heap);

sth_stat_t sth_get_stat(sth_ctx_t * heap);

#endif

