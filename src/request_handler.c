#include "request_handler.h"

#include <stdio.h>

void request_handle(
    const struct http_request *request)
{
  printf("Method: %s\n", request->method);
  printf("Path: %s\n", request->path);
  printf("Version: %s\n", request->version);

  for (size_t i = 0;
       i < request->header_count;
       i++)
  {
    printf(
        "Header: %s = %s\n",
        request->headers[i].name,
        request->headers[i].value);
  }
}