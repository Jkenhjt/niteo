#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>                                                                                           /* Used only for memset. Maybe removed in future. */

#define DEXH1_HASH_TABLE_SIZE 1024                                                                             /* Hash table size macros */
#define DEXH1_INIT_HASH_TABLE(x) memset(&x.values, 0, sizeof(dexh1_http_field) * DEXH1_HASH_TABLE_SIZE); \
                                 x.size = DEXH1_HASH_TABLE_SIZE;    /* Initialization hash table macros. */

typedef struct
{
  char* name;          /* Name of field */
  char* value;         /* Value of filed */

  int name_length;     /* Name length. */
  int value_length;    /* Value length. */
} dexh1_http_field;    /* Http field struct. */

typedef struct
{
  dexh1_http_field values[DEXH1_HASH_TABLE_SIZE];    /* Array of http fields. */
  int size;                                          /* Size of hash table for runtime usage. */
} dexh1_hash_table_t;

dexh1_http_field dexh1_get_field(dexh1_hash_table_t* ht, char* name, int length);    /* Extracts field from hash table. */
void dexh1_insert_field(dexh1_hash_table_t* ht, dexh1_http_field value);             /* Inserts field into hash table. */
void dexh1_delete_field(dexh1_hash_table_t* ht, char* name, int length);             /* Deletes field from hash table. By zeroing it. */

#ifdef __cplusplus
}
#endif

#endif
