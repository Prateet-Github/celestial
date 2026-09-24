#include "http_parser.h"
#include <string.h>

#include "http_parser.h"
#include <string.h>

int http_find_request_line_end(
    const char *buffer,
    size_t length)
{
  for (size_t i = 0; i + 1 < length; i++)
  {
    if (buffer[i] == '\r' &&
        buffer[i + 1] == '\n')
    {
      return (int)i;
    }
  }

  return -1;
}

int http_find_header_end(
    const char *buffer,
    size_t length)
{
  if (length < 4)
  {
    return -1;
  }

  for (size_t i = 0; i <= length - 4; i++)
  {
    if (buffer[i] == '\r' &&
        buffer[i + 1] == '\n' &&
        buffer[i + 2] == '\r' &&
        buffer[i + 3] == '\n')
    {
      return (int)i;
    }
  }

  return -1;
}

int http_parse_request_line(
    const char *buffer,
    size_t length,
    struct http_request *request)
{
  size_t i = 0;
  size_t method_start = 0;

  while (i < length && buffer[i] != ' ')
  {
    i++;
  }

  if (i == length)
  {
    return -1;
  }

  size_t method_length = i - method_start;

  if (method_length == 0 ||
      method_length >= HTTP_METHOD_MAX)
  {
    return -1;
  }

  memcpy(
      request->method,
      buffer + method_start,
      method_length);

  request->method[method_length] = '\0';

  i++;

  size_t path_start = i;

  while (i < length && buffer[i] != ' ')
  {
    i++;
  }

  if (i == length)
  {
    return -1;
  }

  size_t path_length = i - path_start;

  if (path_length == 0 ||
      path_length >= HTTP_PATH_MAX)
  {
    return -1;
  }

  memcpy(
      request->path,
      buffer + path_start,
      path_length);

  request->path[path_length] = '\0';

  i++;

  size_t version_start = i;

  while (i < length &&
         buffer[i] != '\r' &&
         buffer[i] != '\n')
  {
    i++;
  }

  size_t version_length = i - version_start;

  if (version_length == 0 ||
      version_length >= HTTP_VERSION_MAX)
  {
    return -1;
  }

  memcpy(
      request->version,
      buffer + version_start,
      version_length);

  request->version[version_length] = '\0';

  return 0;
}

int http_parse_header(
    const char *buffer,
    size_t length,
    struct http_header *header)
{
  size_t i = 0;

  // Find ':' separating name and value
  while (i < length && buffer[i] != ':')
  {
    i++;
  }

  if (i == length || i == 0)
  {
    return -1;
  }

  size_t name_length = i;

  if (name_length >= HTTP_HEADER_NAME_MAX)
  {
    return -1;
  }

  memcpy(
      header->name,
      buffer,
      name_length);

  header->name[name_length] = '\0';

  // Skip ':' character
  i++;

  // Skip optional spaces
  while (i < length && buffer[i] == ' ')
  {
    i++;
  }

  size_t value_start = i;

  // Find end of header
  while (i < length &&
         buffer[i] != '\r' &&
         buffer[i] != '\n')
  {
    i++;
  }

  size_t value_length = i - value_start;

  if (value_length >= HTTP_HEADER_VALUE_MAX)
  {
    return -1;
  }

  memcpy(
      header->value,
      buffer + value_start,
      value_length);

  header->value[value_length] = '\0';

  return 0;
}

int http_find_line_end(
    const char *buffer,
    size_t length)
{
  for (size_t i = 0; i + 1 < length; i++)
  {
    if (buffer[i] == '\r' &&
        buffer[i + 1] == '\n')
    {
      return (int)i;
    }
  }

  return -1;
}

int http_parse_request(
    const char *buffer,
    size_t length,
    struct http_request *request)
{
  *request = (struct http_request){0};

  // Parse request line

  if (http_parse_request_line(
          buffer,
          length,
          request) == -1)
  {
    return -1;
  }

  // Find request line end

  int request_line_end =
      http_find_request_line_end(
          buffer,
          length);

  if (request_line_end == -1)
  {
    return -1;
  }

  // Move to first header

  const char *header_ptr =
      buffer + request_line_end + 2;

  size_t remaining_length =
      length -
      (size_t)(header_ptr - buffer);

  //    Parse headers

  while (remaining_length > 2)
  {

    // Empty line means end of headers

    if (header_ptr[0] == '\r' &&
        header_ptr[1] == '\n')
    {
      break;
    }

    int line_end =
        http_find_line_end(
            header_ptr,
            remaining_length);

    if (line_end == -1)
    {
      return -1;
    }

    if (request->header_count >=
        HTTP_MAX_HEADERS)
    {
      return -1;
    }

    struct http_header *header =
        &request->headers[request->header_count];

    if (http_parse_header(
            header_ptr,
            (size_t)line_end,
            header) == -1)
    {
      return -1;
    }

    request->header_count++;

    size_t consumed =
        (size_t)line_end + 2;

    header_ptr += consumed;
    remaining_length -= consumed;
  }

  return 0;
}