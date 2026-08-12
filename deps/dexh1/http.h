#ifndef HTTP_H
#define HTTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hash_table.h" /* Implements hash table functionality (inserting, extracting and deleting). */

typedef struct
{
  dexh1_hash_table_t ht;
} dexh1_http;


void init_dexh1(dexh1_http* var);                                               /* Initialization function. */
void dexh1_request_parser(dexh1_http* req, char* buff, int headers_length);     /* Processing request header. */
void dexh1_response_parser(dexh1_http* req, char* buff, int headers_length);    /* Processing response header. */

#ifdef __cplusplus
}
#endif

#endif
