#pragma once
#include <stdint.h>

namespace server
{
struct server_t;

// new server
server_t * server_start(uint16_t port);

// stop the server
void server_stop(server_t *);

}  // namespace server
