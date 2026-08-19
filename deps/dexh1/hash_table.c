#include "hash_table.h"


__inline__ int dexh1_hash(int size, char* name, int length)
{
  /* Fnv1a hash function
   * Used for calculation hash of field.
   */
  
  unsigned long result = 0xCBF29CE484222325;

  int i;

  for(i = 0; i < length; i++)
  {
    result ^= name[i];
    result = result * 0x100000001B3;
  }

  return result % size;
}

__inline__ dexh1_http_field dexh1_get_field(dexh1_hash_table_t* ht, char* name, int length)
{
  /* Extraction field from hash table */

  int idx = dexh1_hash(DEXH1_HASH_TABLE_SIZE, name, length);
  return ht->values[idx];
}

__inline__ void dexh1_insert_field(dexh1_hash_table_t* ht, dexh1_http_field value)
{
  /* Insertation field into hash table */

  int idx = dexh1_hash(DEXH1_HASH_TABLE_SIZE, value.name, value.name_length);
  
  ht->values[idx] = value;
}

__inline__ void dexh1_delete_field(dexh1_hash_table_t* ht, char* name, int length)
{
  /* Deletion field from hash table.
   * Using zeroing for deletion. */

  int idx = dexh1_hash(DEXH1_HASH_TABLE_SIZE, name, length);
  
  ht->values[idx].name = 0;
  ht->values[idx].name_length = -1;
  ht->values[idx].value = 0;
  ht->values[idx].value_length = -1;
}
