#pragma once
#include <array>
#include <assert.h>
#include <stdint.h>

#include "../../nomad-sdk/source/nomad.h"

namespace net {

// new net instance
bool net_init();

// release net instance
void net_free();

#if defined(WIN32)
typedef unsigned long long net_socket_t;
#elif defined(__linux__)
typedef int net_socket_t;
#endif

struct client_t
{
    net_socket_t socket_;
    bool error_;

    client_t();
    client_t(net_socket_t socket);

    bool is_ok();

    void close();

    bool pending();

    bool recv(nomad::event_header_t & hdr,
              void * dst,
              const size_t max_size);

    bool send(const void * src,
              const size_t size,
              const uint32_t type);

    bool send(const nomad::event_t & src);

    bool connect(const char * address);

    bool connect(const uint8_t ip[4], const uint16_t port);

    template <typename type_t>
    bool send(const type_t & src) {
        return send((const void*)&src, sizeof(type_t), type_t::type());
    }

    bool recv(nomad::event_t & out) {
        return recv(out.header_, out.body_.data(), out.body_.size());
    }
};

} // namespace net
