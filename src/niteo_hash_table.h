#ifndef NITEO_HASH_TABLE_H
#define NITEO_HASH_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <dexh1/http.h>    /* Http parser lib. Supports Http 1.0 and 1.1 */


#define NITEO_HASH_TABLE_SIZE 65537                                                      /* pre-defined size of routers array. */
#define NITEO_INIT_HASH_TABLE() { .router_list = {0}, .size = NITEO_HASH_TABLE_SIZE }    /* Macros for initialization Router hash table. */


typedef struct
{
  void (*router_list[NITEO_HASH_TABLE_SIZE])(dexh1_http* req, dexh1_http* resp);    /* Routers array. */
  int size;                                                                         /* Size of routers array for runtime using. */
} niteo_hash_table_t;                                                               /* Routers hash table structure. */


int niteo_hash(char* name, int length, int size);                                                                                                /* Fnv1a hash function. */
void (*niteo_get_router(niteo_hash_table_t* router_list, char* router_name, int length)) (dexh1_http* req, dexh1_http* resp);                    /* Getting router from hash table. */
int niteo_insert_router(niteo_hash_table_t* router_list, void (*callback)(dexh1_http* req, dexh1_http* resp), char* router_name, int length);    /* Inserting router into hash table. */
int niteo_delete_router(niteo_hash_table_t* router_list, char* router_name, int length);                                                         /* Remove router from hash table. */


#ifdef __cplusplus
}
#endif

#endif
