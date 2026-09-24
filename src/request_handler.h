#ifndef CELESTIAL_REQUEST_HANDLER_H
#define CELESTIAL_REQUEST_HANDLER_H

#include "http_parser.h"

void request_handle(
    const struct http_request *request);

#endif