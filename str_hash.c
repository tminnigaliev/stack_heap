#include "str_hash.h"
//#include "stddef.h"
#include "str_hash_plat.h"
#include "string.h"

void ht_init(ht_rec_t hash_table[],
		      unsigned                hash_table_size,
		      unsigned *              max_key)
{
    int i;
    for (i = 0; i < hash_table_size; i++)
    {
    	hash_table[i].chain = NULL;
    }
    *max_key = 0;
}

static str_rec_t * str_record_init(
		                       str_rec_t * first_item,
		                       const int key,
		                       const char * const text)
{
    str_rec_t * res = str_hash_malloc(sizeof(str_rec_t));
    res->auto_inc_key = key;
    res->header_pattern = STHH_DBG_REC;
    res->next = (struct sth_debug_record_tag *)first_item;
    if (first_item)
    {
        res->prev = first_item->prev;
    	first_item->prev = (struct sth_debug_record_tag*)(res);
    }
    else
    {
    	res->prev = NULL;
    }
    int text_size=strlen(text)+1;
    res->text = str_hash_malloc(text_size);
    memcpy(res->text, text, text_size);
    return res;
}

static unsigned calc_hash(const char * const text, int max_len, const unsigned hash_table_size)
{
	unsigned seed = 1;
	int i=0;

	if (text)
	{
	    while (text[i] && i < max_len)
	    {
	    	seed *= text[i];
	    	i++;
	    }
	    return (seed % hash_table_size);
	}
	else return (unsigned)-1;
}

/*
 *  returns: key
 *  takes:   text - text for indexing
 *           hash_table - pointer to hash_table
 *           hash_table_size
 *           * max_key - pointer to max key value
 *
 *           max_key and hash_table should be members of the same struct
 */
ht_key_t ht_reg_string(const char * const      text,
		                ht_rec_t hash_table[],
		                const unsigned          hash_table_size,
		                unsigned * const        max_key)
{
    ht_key_t res;
    int idx = calc_hash(text, MAX_COMPARE_LENGTH, hash_table_size);
	res.hash = idx;

	if (hash_table[idx].chain != NULL)
    {
    	str_rec_t * ptr = hash_table[idx].chain;
    	while(ptr)
    	{
    		if (strncmp(ptr->text, text, MAX_COMPARE_LENGTH) == 0)
    		{
    			res.auto_inc = ptr->auto_inc_key;
    			return res;
    		}
    		ptr = (str_rec_t *)ptr->next;
    	}

    }
	//there is no text in hash table
	hash_table[idx].chain = str_record_init(hash_table[idx].chain, *max_key, text);
	res.auto_inc = *max_key;
	(*max_key)++;
	return res;
}


char * ht_get_string(ht_key_t key,
                      ht_rec_t hash_table[])
{
	int idx = key.hash;
	int auto_inc = key.auto_inc;
	char * res;
	if (hash_table[idx].chain != NULL)
    {
    	str_rec_t * ptr = hash_table[idx].chain;
    	while(ptr)
    	{
    		if (ptr->auto_inc_key == auto_inc)
    		{
    			return ptr->text;
    		}
    		ptr = (str_rec_t *)ptr->next;
    	}
    }
	return NULL;
}


