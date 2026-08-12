#ifndef NITEO_H
#define NITEO_H

#ifdef __cplusplus
extern "C" {
#endif


#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <threads.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <fcntl.h>

#include <dexh1/http.h>
#include "niteo_hash_table.h"


#define NITEO_HTTP_RESPONSE_400 "HTTP/1.1 400 Bad request\r\nConnection: close\r\n\r\n"
#define NITEO_HTTP_RESPONSE_404 "HTTP/1.1 404 Not found\r\nConnection: close\r\n\r\n"
#define NITEO_HTTP_RESPONSE_414 "HTTP/1.1 414 URI Too Long\r\nConnection: close\r\n\r\n"
#define NITEO_HTTP_RESPONSE_411 "HTTP/1.1 411 Length Required\r\nConnection: close\r\n\r\n"
#define NITEO_HTTP_RESPONSE_501 "HTTP/1.1 501 Not implemented\r\nConnection: close\r\n\r\n"
#define NITEO_EVENTS_NUMBER 1024
#define NITEO_INIT_ROUTERS NITEO_INIT_HASH_TABLE()
#define NITEO_SET_RESPONSE_DATA(ht, value_, value_length_) dexh1_insert_field(ht, (dexh1_http_field) { .name = "message_body", .name_length = 12, .value = value_, .value_length = value_length_})    /* Maros for setting response from router */


extern niteo_hash_table_t routers;


int niteo_StartServer(void* arg);                         /* Main server loop function. */
void niteo_server_launcher(int threads, int is_async);    /* Runner server loop. */


#ifdef __cplusplus
}
#endif

#endif
