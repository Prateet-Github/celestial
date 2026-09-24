#include "connection.h"

#include <unistd.h>

void connection_init(
    struct connection *connection,
    int fd)
{
  connection->fd = fd;
  connection->read_length = 0;
  connection->write_buffer = NULL;
  connection->write_length = 0;
  connection->write_offset = 0;
}

ssize_t connection_read(
    struct connection *connection)
{
  ssize_t bytes_read = read(
      connection->fd,
      connection->read_buffer,
      sizeof(connection->read_buffer) - 1);

  if (bytes_read > 0)
  {
    connection->read_length = (size_t)bytes_read;
    connection->read_buffer[bytes_read] = '\0';
  }

  return bytes_read;
}

ssize_t connection_write(
    struct connection *connection)
{
  if (connection->write_offset >=
      connection->write_length)
  {
    return 0;
  }

  ssize_t bytes_written = write(
      connection->fd,
      connection->write_buffer +
          connection->write_offset,
      connection->write_length -
          connection->write_offset);

  if (bytes_written > 0)
  {
    connection->write_offset +=
        (size_t)bytes_written;
  }

  return bytes_written;
}