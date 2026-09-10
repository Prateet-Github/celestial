#include "event_loop.h"

#include <sys/event.h>

int event_loop_create(void)
{
  return kqueue();
}