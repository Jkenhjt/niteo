#include "niteo_hash_table.h"

int niteo_hash(char* name, int length, int size)
{
  /* Fnv1a hash function
   * Used for calculation hash of field. Optimized with Little-endian.
   * If Lerngth >= 8, using Little-endian optimization, if less
   * using scalar.
   */
  
  unsigned long result = 0xCBF29CE484222325;

  if(length >= 8)
  {
    for(int i = 0; i < length; i += 8)
    {
      result ^= *(unsigned long *) &name[i];
      result = result * 0x100000001B3;
    }
  }
  else
  {
    for(int i = 0; i < length; i++)
    {
      result ^= name[i];
      result = result * 0x100000001B3;
    }
  }
  return result % size;
}

void (*niteo_get_router(niteo_hash_table_t* router_list, char* router_name, int length)) (dexh1_http* req, dexh1_http* resp)
{
  /* Extraction field from hash table */
  
  int idx = niteo_hash(router_name, length, router_list->size);
  return router_list->router_list[idx];
}

int niteo_insert_router(niteo_hash_table_t* router_list, void (*callback)(dexh1_http* req, dexh1_http* resp), char* router_name, int length)
{
  /* Insertation field into hash table */
  
  int idx = niteo_hash(router_name, length, router_list->size);
  
  if(router_list->router_list[idx] != 0)
  {
    return -1;
  }

  router_list->router_list[idx] = callback;
  return 0;
}

int niteo_delete_router(niteo_hash_table_t* router_list, char* router_name, int length)
{
  /* Deletion field from hash table.
   * Using zeroing for deletion. */
  
  int idx = niteo_hash(router_name, length, router_list->size);

  router_list->router_list[idx] = 0;
  return 0;
}
