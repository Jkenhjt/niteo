#include "http.h"

__inline__ int dexh1_idx_delim(unsigned long data, int delim)
{
  /* Fast search char without simd.
   * Using ctzll (count trailing zeros long long) function for count
   * is char in unsigned long variable and on what position.
   * Decreases count of loops, because scans 8 bytes per call
   */

  unsigned long mask_delim = (0x0101010101010101) * delim;
  unsigned long result = data ^ mask_delim;

  result = (result - 0x0101010101010101) & ~result & 0x8080808080808080;
  
  if(result == 0)
  {
    return -1;
  }
  
  return __builtin_ctzll(result) / 8;
}

__inline__ int dexh1_length_field(char* buff, int length, char delim, int* idx, int* field_length)
{
  int result = dexh1_idx_delim(*(unsigned long *)(&buff[*idx]), delim);
  for(; result == -1 && *field_length < length;
	*idx += sizeof(unsigned long),
	result = dexh1_idx_delim(*(unsigned long *)(&buff[*idx]), delim),
	*field_length += sizeof(unsigned long))
  {}

  return result;
}

__inline__ void dexh1_method_request(dexh1_http* req, char* buff, int length)
{
  char* method;
  int method_length = 0;
  char* path;
  int path_length = 0;
  char* http_version;
  int http_version_length = 0;
  int result = 0;
  int idx = 0;
  dexh1_http_field method_;
  dexh1_http_field path_;
  dexh1_http_field http_version_;

  /* Finding method. */

  method = &buff[idx];

  result = dexh1_length_field(buff, length, ' ', &idx, &method_length);

  idx += result+1;
  method_length += result;

  /* Findg path. */

  path = &buff[idx];

  result = dexh1_length_field(buff, length, ' ', &idx, &path_length);

  idx += result;
  path_length += result;
  
  idx++;

  /* Finding http version. */

  http_version = &buff[idx];

  result = dexh1_length_field(buff, length, '\r', &idx, &http_version_length);
  
  idx += result;
  http_version_length += result;
 
  /* Inserting all previous parsed data into hash table. */

  method_.name = "method";
  method_.name_length = 6;
  method_.value = method;
  method_.value_length = method_length;

  path_.name = "path";
  path_.name_length = 4;
  path_.value = path;
  path_.value_length = path_length;

  http_version_.name = "http_version";
  http_version_.name_length = 12;
  http_version_.value = http_version;
  http_version_.value_length = http_version_length;

  dexh1_insert_field(&(req->ht), method_);
  dexh1_insert_field(&(req->ht), path_);
  dexh1_insert_field(&(req->ht), http_version_);
}

__inline__ void dexh1_method_response(dexh1_http** req, char* buff, int length)
{
  char* http_version;
  int http_version_length = 0;
  char* status_code;
  int status_code_length = 0;
  char* status_message;
  int status_message_length = 0;
  int result = 0;
  int idx = 0;
  dexh1_http_field http_version_;
  dexh1_http_field status_code_;
  dexh1_http_field status_message_;

  /* Finding http version. */

  http_version = &buff[idx];
 
  result = dexh1_length_field(buff, length, ' ', &idx, &http_version_length);

  idx += result+1;
  http_version_length += result;

  /* Finding status code. */

  status_code = &buff[idx];

  result = dexh1_length_field(buff, length, ' ', &idx, &status_code_length);

  idx += result;
  status_code_length += result;
  
  idx++;

  /* Finding status message (if exist). */
  
  status_message = &buff[idx];

  result = dexh1_length_field(buff, length, '\r', &idx, &status_message_length);
  
  idx += result;
  status_message_length += result;
  
  /* Inserting all previous parsed data into hash table. */
  
  http_version_.name = "http_version";
  http_version_.name_length = 12;
  http_version_.value = http_version;
  http_version_.value_length = http_version_length;

  status_code_.name = "status_code";
  status_code_.name_length = 11;
  status_code_.value = status_code;
  status_code_.value_length = status_code_length;

  status_message_.name = "status_message";
  status_message_.name_length = 14;
  status_message_.value = status_message;
  status_message_.value_length = status_message_length;

  dexh1_insert_field(&((*req)->ht), http_version_);
  dexh1_insert_field(&((*req)->ht), status_code_);
  dexh1_insert_field(&((*req)->ht), status_message_);
}

void dexh1_find_fields(int headers_length, dexh1_http* req, char* buff)
{
  char* field;
  int field_length = 0;
  char* value;
  int value_length = 0;
  dexh1_http_field field_;
  
  int result = 0;
  int i = 0;
  for(i = 0; i < headers_length;)
  {
    if(headers_length - i > 8)
    {
      result = dexh1_idx_delim(*((unsigned long *) &buff[i]), '\r');
      if(result == -1)
      {
        i += sizeof(unsigned long);
        continue;
      }
      i += result;
    }
    else
    {
      if(buff[i] != '\r')
      {
        i++;
	continue;
      }
    }

    
    buff += 2;
    
    if(buff[i+1] == '\n')
    {
      char* value;
      int value_length;

      value = buff+i+2;

      value_length = headers_length - i;
      
      field_.name = "message_body";
      field_.name_length = 12;
      field_.value = value;
      field_.value_length = value_length;

      dexh1_insert_field(&req->ht, field_);

      break;
    }


    field = buff+i;

    field_length = i;

    for(; i < headers_length ;)
    {
      result = dexh1_idx_delim(*((unsigned long *) &buff[i]), ':');
      if(result != -1)
      {
        i += result;
	break;
      }
      
      i += sizeof(unsigned long);
    }

    field_length = i - field_length;
    
    i += 2;
 

    value = buff+i;

    value_length = i;

    for(; i < headers_length ;)
    {
      result = dexh1_idx_delim(*((unsigned long *) &buff[i]), '\r');
      if(result != -1)
      {
        i += result;
	break;
      }
      
      i += sizeof(unsigned long);
    }
    
    value_length = i - value_length;
    

    field_.name = field;
    field_.name_length = field_length;
    field_.value = value;
    field_.value_length = value_length;
    
    dexh1_insert_field(&req->ht, field_);
  }
}

void dexh1_request_parser(dexh1_http* req, char* buff, int headers_length)
{
  dexh1_method_request(req, buff, headers_length);
  
  dexh1_find_fields(headers_length, req, buff);
}

void dexh1_response_parser(dexh1_http* resp, char* buff, int headers_length)
{
  dexh1_method_response(&resp, buff, headers_length);
  
  dexh1_find_fields(headers_length, resp, buff);
}

void init_dexh1(dexh1_http* var)
{
  DEXH1_INIT_HASH_TABLE(var->ht)
}
