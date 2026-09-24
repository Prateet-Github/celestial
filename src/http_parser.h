#ifndef CELESTIAL_HTTP_PARSER_H
#define CELESTIAL_HTTP_PARSER_H

#include <stddef.h>

#define HTTP_METHOD_MAX 16
#define HTTP_PATH_MAX 1024
#define HTTP_VERSION_MAX 16

#define HTTP_HEADER_NAME_MAX 64
#define HTTP_HEADER_VALUE_MAX 1024
#define HTTP_MAX_HEADERS 32

struct http_header
{
  char name[HTTP_HEADER_NAME_MAX];
  char value[HTTP_HEADER_VALUE_MAX];
};

struct http_request
{
  char method[HTTP_METHOD_MAX];
  char path[HTTP_PATH_MAX];
  char version[HTTP_VERSION_MAX];

  struct http_header headers[HTTP_MAX_HEADERS];
  size_t header_count;
};

int http_parse_request_line(
    const char *buffer,
    size_t length,
    struct http_request *request);

int http_find_header_end(
    const char *buffer,
    size_t length);

int http_parse_header(
    const char *buffer,
    size_t length,
    struct http_header *header);

int http_find_request_line_end(
    const char *buffer,
    size_t length);

int http_find_line_end(
    const char *buffer,
    size_t length);

int http_parse_request(
    const char *buffer,
    size_t length,
    struct http_request *request);

#endif