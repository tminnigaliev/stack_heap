#include "str_hash.h"
#include <stdio.h>

#define HASH_TABLE_SIZE           2
ht_rec_t hash_table[HASH_TABLE_SIZE];
unsigned int max_key = 0;

int main(int argc, char*argv[])
{
    ht_init(hash_table, HASH_TABLE_SIZE, &max_key);
    ht_key_t key1 = ht_reg_string(__FUNCTION__, hash_table, HASH_TABLE_SIZE, &max_key);
    ht_key_t key2 = ht_reg_string(__FILE__, hash_table, HASH_TABLE_SIZE, &max_key);
    ht_key_t key3 = ht_reg_string(__FUNCTION__, hash_table, HASH_TABLE_SIZE, &max_key);
    ht_key_t key4 = ht_reg_string(__DATE__, hash_table, HASH_TABLE_SIZE, &max_key);
    ht_key_t key5 = ht_reg_string(__TIME__, hash_table, HASH_TABLE_SIZE, &max_key);

    printf("key1=%x, %s\n", key1.auto_inc, ht_get_string(key1, hash_table));
    printf("key2=%x, %s\n", key2.auto_inc, ht_get_string(key2, hash_table));
    printf("key3=%x, %s\n", key3.auto_inc, ht_get_string(key3, hash_table));
    printf("key4=%x, %s\n", key4.auto_inc, ht_get_string(key4, hash_table));
    printf("key5=%x, %s\n", key5.auto_inc, ht_get_string(key5, hash_table));
    return 0;
}
