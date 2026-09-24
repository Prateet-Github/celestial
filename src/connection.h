#ifndef CELESTIAL_CONNECTION_H
#define CELESTIAL_CONNECTION_H

#include <stddef.h>

#define CONNECTION_BUFFER_SIZE 4096

struct connection
{
  int fd;

  char read_buffer[CONNECTION_BUFFER_SIZE];
  size_t read_length;

  const char *write_buffer;
  size_t write_length;
  size_t write_offset;
};

#endif