#ifndef STR_HASH_H_
#define STR_HASH_H_
#include "sth_types.h"

/*
 * This define means length which limits comparison
 *     i.e. strings like 1        10        20         30
 *                       something_equal_here_until_32_nd_different
 *                       something_equal_here_until_32_nd_other
 *     will be treated as the same string
 */
#define MAX_COMPARE_LENGTH    32

/*
 * Name of type is derived from: STRing RECord Type
 */
typedef struct str_rec_tag
{
    sth_header_t   header_pattern;
    unsigned auto_inc_key;
    char *text;
    struct sth_debug_record_tag *next;
    struct sth_debug_record_tag *prev;
}str_rec_t;


/*
 * Name of type is derived from: Hash Table RECord Type
 */
typedef struct
{
    //int hash_value;  //no hash value field since record index is hash value
    struct str_rec_tag * chain;
}ht_rec_t;

/*
 * Name of type is derived from: Hash Table KEY Type
 */
typedef struct
{
    unsigned hash     : 16;
    unsigned auto_inc : 16;
}ht_key_t;

void ht_init(ht_rec_t hash_table[],
  	          unsigned                hash_table_size,
              unsigned *              max_key);

/*
 *  returns: key
 *  takes:   text - text for indexing
 *           hash_table - pointer to hash_table
 *           hash_table_size
 *           * max_key - pointer to max key value
 *
 *           max_key and hash_table should be members of the same struct
 *
 *  Name of function is derived from: Hash Table REGister STRING
 */
ht_key_t ht_reg_string(const char * const      text,
		                ht_rec_t hash_table[],
		                const unsigned          hash_table_size,
		                unsigned * const        max_key);

/*
 *  Name of function is derived from: Hash Table GET STRING
 */
char * ht_get_string(ht_key_t key,
                      ht_rec_t hash_table[]);


#endif


