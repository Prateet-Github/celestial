#include "http.h"

static const char response[] =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 17\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Hello, Celestial!";

const char *http_response(size_t *length)
{
  *length = sizeof(response) - 1;

  return response;
}