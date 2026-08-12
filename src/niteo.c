#include "niteo.h"


static int niteo_server_loop = 1;

void niteo_stop_loop(int x)
{
  niteo_server_loop = 0;
}

niteo_hash_table_t routers = NITEO_INIT_ROUTERS;


int niteo_receiving_data(int socket, char* buff)
{
  int received_bytes = 0;
  
  for(;;)
  {
    int bytes = recv(socket, buff, 102400, MSG_DONTWAIT);
    
    if(bytes == -1)
    {
      if(errno == EAGAIN || errno == EWOULDBLOCK)
      {
	continue;
      }
      if(errno == ECONNRESET || errno == EBADF)
      {
        return -2;
      }
      
      return -1;
    }

    if(bytes == 0)
    {
      return -2;
    }

    if(bytes > 0)
    {
      received_bytes += bytes;
      break;
    }
  }
  return received_bytes;
}

int niteo_sending_data(int socket, char* buff, int length)
{
  int sended_bytes = 0;
  
  for(;;)
  {
    int bytes = send(socket, buff, length, MSG_DONTWAIT);
    
    if(bytes == -1)
    {
      if(errno == EAGAIN || errno == EWOULDBLOCK)
      {
        continue;
      }
      if(errno == ECONNRESET || errno == EBADF)
      {
        return -2;
      }

      return -1;
    }

    if(bytes == 0)
    {
      return -2;
    }
    
    if(bytes == length)
    {
      sended_bytes += bytes;
      break;
    }
  }
  return sended_bytes;
}

char* niteo_int_to_char(int x, char* buff)
{
  char buff_reversed[22+1];
  buff_reversed[22] = '\0';

  int length = 0;
  for(int i = 0; x > 0; x /= 10, i++, length++)
  {
    buff_reversed[i] = (x % 10) + '0';
  }

  length--;

  buff[length+1] = '\0';

  for(int i = 0; length >= 0; i++, length--)
  {
    buff[i] = buff_reversed[length];
  }
  
  return buff;
}


int niteo_response_sending(dexh1_http* resp, char* buff)
{
  int length = 0;
  char buff_content_length[22+1];

  buff_content_length[22] = '\0';

  dexh1_http_field temp_field = dexh1_get_field(&resp->ht,
		                                "http_version",
						12);
  int temp = temp_field.value_length;

  strncpy(buff, temp_field.value, temp);
  length += temp;
  
  strncpy(buff+length, " ", 1+1);
  length += 1;
 
  temp_field = dexh1_get_field(&resp->ht,
		               "status_code",
			       11);
  temp = temp_field.value_length;
  strncat(buff, temp_field.value, temp);
  length += temp;
  
  strncat(buff, "\r\nConnection: ", 14+1);
  length += 14;
  
  temp_field = dexh1_get_field(&resp->ht,
		               "Connection",
			       10);
  temp = temp_field.value_length;
  strncat(buff, temp_field.value, temp);
  length += temp;
  
  strncat(buff, "\r\nContent-Length: ", 18+1);
  length += 18;
  
  dexh1_http_field temp_field_mb = dexh1_get_field(&resp->ht,
		                                   "message_body",
						   12);
  temp = temp_field_mb.value_length;
  strncat(buff, niteo_int_to_char(temp, buff_content_length), temp);
  length += temp;
  
  strncat(buff, "\r\nContent-Type: ", 16+1);
  length += 16;
  
  temp_field = dexh1_get_field(&resp->ht,
		               "Content-Type",
			       12);
  temp = temp_field.value_length;
  strncat(buff, temp_field.value, temp);
  length += temp;
  
  strncat(buff, "\r\n\r\n", 4+1);
  length += 4;
  
  temp = temp_field_mb.value_length;
  strncat(buff, temp_field_mb.value, temp);
  length += temp;
  
  strncat(buff, "\r\n", 2+1);
  length += 2;

  return length;
}

void niteo_base_request_header(dexh1_http* resp)
{
    dexh1_insert_field(&resp->ht,
		       (dexh1_http_field) { .name = "http_version",
		                            .name_length = 12,
					    .value = "HTTP/1.1",
					    .value_length = 8 });
    dexh1_insert_field(&resp->ht,
		       (dexh1_http_field) { .name = "status_code",
		                            .name_length = 11,
					    .value = "200",
					    .value_length = 3 });
    dexh1_insert_field(&resp->ht,
		       (dexh1_http_field) { .name = "Content-Type",
		                            .name_length = 12,
					    .value = "text/plain",
					    .value_length = 10 });
    dexh1_insert_field(&resp->ht,
		       (dexh1_http_field) { .name = "Content-Length",
		                            .name_length = 14,
					    .value = "",
					    .value_length = 1 });
}

int niteo_routing_request(dexh1_http* req, dexh1_http* resp, int socket_accept)
{
    void (*result)(dexh1_http* req, dexh1_http* resp);

    result = niteo_get_router(&routers, "/", 1);
    if(result == 0)
    {
      niteo_sending_data(socket_accept,
		         NITEO_HTTP_RESPONSE_404,
			 strlen(NITEO_HTTP_RESPONSE_404));

      return -1;
    }

    niteo_base_request_header(resp);

    result(req, resp);

    return 0;
}

int niteo_request(dexh1_http* req, dexh1_http* resp, int socket_accept, char* buff)
{
    int received_bytes = niteo_receiving_data(socket_accept, buff);
    if(received_bytes == -1)
    {
      return -1;
    }

    if(received_bytes == -2)
    {
      return 1;
    }

    buff[received_bytes] = '\0';

    dexh1_request_parser(req, buff, strlen(buff));

    return niteo_routing_request(req, resp, socket_accept);
}

int niteo_response(dexh1_http* resp, int socket_accept, char* buff)
{
    niteo_response_sending(resp, buff);

    int sended_bytes = niteo_sending_data(socket_accept,
		                          buff,
					  strlen(buff) - 1);
    if(sended_bytes == -1)
    {
      return -1;
    }

    if(sended_bytes == -2)
    {
      return 1;
    }
    
    return 0;
}

int niteo_request_process(int socket_accept, dexh1_http* req, dexh1_http* resp, int mode)
{
  char buff[102400+1];

  if(mode == 0)
  {
    return niteo_request(req, resp, socket_accept, buff);
  }
  
  if(mode == 1)
  {
    return niteo_response(resp, socket_accept, buff);
  }

  return 0;
}

int set_nonblocking_mode(int socket)
{
  int flags = fcntl(socket, F_GETFL, 0);
  
  if(flags == -1)
  {
    return -1;
  }
  
  return fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

void set_socket_base_options(int* sock)
{
  int yes = 1;
  int socket_buff_size = 4194304;
  struct linger sl;
  sl.l_onoff = 1;
  sl.l_linger = 0;
  
  setsockopt(*sock,
	     SOL_SOCKET,
	     SO_REUSEADDR,
	     &yes, sizeof(yes));
  setsockopt(*sock,
	     SOL_SOCKET,
	     15,
	     &yes, sizeof(yes));
  setsockopt(*sock,
	     SOL_SOCKET,
	     SO_LINGER,
	     &sl, sizeof(sl));
  setsockopt(*sock,
	     SOL_SOCKET,
	     SO_ZEROCOPY,
	     &yes, sizeof(yes));
  setsockopt(*sock,
	     IPPROTO_TCP,
	     TCP_NODELAY,
	     &yes, sizeof(yes));
  setsockopt(*sock,
	     SOL_SOCKET,
	     SO_SNDBUF,
	     &socket_buff_size,
	     sizeof(socket_buff_size));
  setsockopt(*sock,
	     SOL_SOCKET,
	     SO_RCVBUF,
	     &socket_buff_size,
	     sizeof(socket_buff_size));
}

int events_manipulation(struct epoll_event* ev, int sock, int epollfd, int event, int command)
{
  ev->events = event;
  ev->data.fd = sock;

  int result = epoll_ctl(epollfd,
		         command,
		         sock,
		         ev);
  if(result == -1)
  {
    close(sock);
    return -1;
  }

  return 0;
}

int niteo_Start_server(void* is_async)
{
  int socket_server;

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  struct epoll_event ev, events[NITEO_EVENTS_NUMBER];
  
  int epollfd, nfds;

  int result;
  
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, "0.0.0.0", &(addr.sin_addr));
  addr.sin_port = htons(8000);

  socket_server = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if(socket_server == -1)
  {
    return -1;
  }

  set_nonblocking_mode(socket_server);

  set_socket_base_options(&socket_server);
  
  result = bind(socket_server,
	        (const struct sockaddr*) &addr,
		sizeof(addr));
  if(result == -1)
  {
    close(socket_server);
    return -2;
  }

  result = listen(socket_server, 4096);
  if(result == -1)
  {
    close(socket_server);
    return -3;
  }
  
  epollfd = epoll_create1(EPOLL_CLOEXEC);
  if(epollfd == -1)
  {
    close(socket_server);
    return -4;
  }

  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = socket_server;

  result = events_manipulation(&ev,
		               socket_server, epollfd,
			       EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
  if(result == -1)
  {
    close(epollfd);
    return -5;
  }

  memset(events, 0, sizeof(struct epoll_event) * NITEO_EVENTS_NUMBER);
  
  dexh1_http req[NITEO_EVENTS_NUMBER];
  dexh1_http resp[NITEO_EVENTS_NUMBER];

  for(int i = 0; i < NITEO_EVENTS_NUMBER; i++)
  {
    init_dexh1(&req[i]);
    init_dexh1(&resp[i]);
  }
  
  for(; niteo_server_loop == 1 ;) {
    nfds = epoll_wait(epollfd, events, NITEO_EVENTS_NUMBER, -1);
    if(nfds == -1)
    {
      result = -6;
      break;
    }

    for(int i = 0; i < nfds; i++)
    {
      if(events[i].data.fd == socket_server &&
         events[i].events & EPOLLIN)
      {
	int socket_accept;
	for(;;)
	{
          socket_accept = accept4(socket_server,
			          (struct sockaddr*) &addr,
				  &addr_len,
				  SOCK_NONBLOCK | SOCK_CLOEXEC);

	  if(socket_accept == -1)
	  {
	    if(errno == EAGAIN || errno == EWOULDBLOCK)
	    {
	      break;
	    }
	    else
	    {
	      break;
	    }
	  }
	  
	  result = events_manipulation(&ev,
			               socket_accept, epollfd,
				       EPOLLIN | EPOLLET, EPOLL_CTL_ADD);
	}
      }
      if(events[i].data.fd != socket_server &&
         events[i].events & EPOLLIN)
      {
        result = niteo_request_process(events[i].data.fd,
			               &req[i], &resp[i],
				       0);

	if(result == 1)
	{
	  epoll_ctl(epollfd,
		    EPOLL_CTL_DEL,
		    events[i].data.fd,
		    NULL);
          close(events[i].data.fd);
	}
	else
	{
	  result = events_manipulation(&ev,
			               events[i].data.fd, epollfd,
			               EPOLLOUT | EPOLLET, EPOLL_CTL_MOD);
	}
      }
      if(events[i].data.fd != socket_server &&
         events[i].events & EPOLLOUT)
      {
        result = niteo_request_process(events[i].data.fd,
			               &req[i], &resp[i],
				       1);

	if(result == 1)
	{
	    epoll_ctl(epollfd,
		      EPOLL_CTL_DEL,
		      events[i].data.fd,
		      NULL);
            close(events[i].data.fd);
	}
	else
	{
	  result = events_manipulation(&ev,
			               events[i].data.fd, epollfd,
			               EPOLLIN | EPOLLET, EPOLL_CTL_MOD);
	}
      }
    }
  }
  close(socket_server);
  close(epollfd);
  return 0;
}

void niteo_server_launcher(int threads, int is_async)
{
  if(threads == 0)
  {
    threads = 1;
  }

  puts("Listen 8000 port...");

  thrd_t t[threads];
  for(int i = 0; i < threads; i++)
  {
    thrd_create(&t[i], niteo_Start_server, (void*) 1);
  }

  for(int i = 0; i < threads; i++)
  {
    printf("Thread %d starting...\n", i);
    thrd_detach(t[i]);
  }
  
  //niteo_Start_server((void*) 1);
  
  signal(SIGINT, niteo_stop_loop);
  
  for(;niteo_server_loop == 1;)
  {
    sleep(1);
  }
}
