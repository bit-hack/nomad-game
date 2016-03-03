#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <cassert>
#include <string.h>

#include "net.h"
#include "../../nomad-util/source/log.h"
#include "../../nomad-util/source/socket.h"

namespace {
uint16_t checksum(const uint8_t *data, size_t bytes) {
    uint16_t sum1 = 0xff, sum2 = 0xff;
    size_t tlen;
    while (bytes) {
        tlen = (bytes>=20) ? 20 : bytes;
        bytes -= tlen;
        do {
            sum2 += sum1 += *data++;
        } while (--tlen);
        sum1 = (sum1&0xff)+(sum1>>8);
        sum2 = (sum2&0xff)+(sum2>>8);
    }
    sum1 = (sum1&0xff)+(sum1>>8);
    sum2 = (sum2&0xff)+(sum2>>8);
    return sum2<<8|sum1;
}
} // namespace {}

namespace net {

#if defined(WIN32)
WSADATA g_wsa;
#endif

bool net_init() {
#if defined(WIN32)
    // init win sock library
    memset(&g_wsa, 0, sizeof(WSADATA));
    if (WSAStartup(MAKEWORD(2, 2), &g_wsa)) {
        LOGF(log_t::e_log_net, "WSAStartup() error");
        return false;
    }
#endif
    return true;
}

void net_free() {
#if defined(WIN32)
    // shutdown winsock instance
    if (WSACleanup()) {
        // error
    }
#endif
}

client_t::client_t()
    : socket_(INVALID_SOCKET)
    , error_(false)
{
}

client_t::client_t(net_socket_t socket)
    : socket_(socket)
    , error_(false)
{
}

bool client_t::is_ok() {
    return !error_ && socket_!=INVALID_SOCKET;
}

void client_t::close() {
    if (socket_!=INVALID_SOCKET) {
#if defined(__linux__)
        close(socket_);
#elif defined(WIN32)
        closesocket(socket_);
#endif 
    }
}

bool client_t::pending() {
    // early exit if client is in error
    if (error_)
        return false;
    // use select() to check socket read ability
    fd_set set;
    FD_ZERO(&set);
    FD_SET(socket_, &set);
    timeval time{0, 0};
    return select(0, &set, nullptr, nullptr, &time)>0;
}

bool client_t::recv(nomad::event_header_t & hdr,
                        void * dst,
                        const size_t max_size) {

    static const int c_hdr_size = sizeof(nomad::event_header_t);

    // early exit if client is in error
    if (socket_==INVALID_SOCKET)
        return !(error_ = true);

    // read header
    int r = ::recv(socket_, (char *)&hdr, c_hdr_size, MSG_WAITALL);
    if (r==SOCKET_ERROR||r!=c_hdr_size) {
        LOGF(log_t::e_log_net, "socket recv() error");
        return !(error_ = true);
    }

    // if packet has a body then lets read it
    if (hdr.size_) {
        // check we have enough space
        assert(hdr.size_<max_size);
        // read body
        r = ::recv(socket_, (char *)dst, hdr.size_, MSG_WAITALL);
        if (r==SOCKET_ERROR||r!=hdr.size_) {
            LOGF(log_t::e_log_net, "socket recv() error");
            return !(error_ = true);
        }
        // validate body with checksum
        uint32_t cs = checksum((const uint8_t*)dst, hdr.size_);
        error_ |= (cs!=hdr.checksum_);
    }

    // success
    return !error_;
}

bool client_t::send(const void * src, const size_t size, const uint32_t type) {

    if (socket_==INVALID_SOCKET)
        return !(error_=true);

    nomad::event_header_t hdr;
    hdr.size_ = uint16_t(size);
    hdr.type_ = type;
    hdr.checksum_ = src ? checksum((const uint8_t *)src, size) : -1;

    // send off the packet header
    int r = ::send(socket_, (char *)&hdr, sizeof(nomad::event_header_t), 0);
    if (r==SOCKET_ERROR||r!=sizeof(hdr)) {
        LOGF(log_t::e_log_net, "socket send() error");
        return !(error_ = true);
    }

    // if the packet has a body
    if (hdr.size_) {
        assert(src);
        // send off the packet body
        r = ::send(socket_, (char *)src, int(size), 0);
        if (r==SOCKET_ERROR||r!=hdr.size_) {
            LOGF(log_t::e_log_net, "socket send() error");
            return !(error_ = true);
        }
    }

    return !error_;
}

bool client_t::connect(const char * address) {
    LOGF(log_t::e_log_net, "connect to: %s", address);

    uint8_t ip[4];
    uint16_t port = 0;
    // extract each field
    int r = sscanf(address, "%hhu.%hhu.%hhu.%hhu:%hu",
                   &ip[0], &ip[1], &ip[2], &ip[3], &port);
    // if we parsed all arguments
    return (r==5) ? connect(ip, port) : false;
}

bool client_t::connect(const uint8_t ip[4],
                       const uint16_t port) {

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
#if (__linux__)
    addr.sin_addr.s_addr = ip;
#elif(WIN32)
    addr.sin_addr.S_un.S_addr = *(uint32_t*)ip; // <-- dont htol()
#endif
    addr.sin_port = htons(port);

    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_==INVALID_SOCKET) {
        LOGF(log_t::e_log_net, "socket() error");
        return !(error_ = true);
    }

    {
        timeval timeout = {8, 0};
        if (setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                       sizeof(timeval))) {
            LOG(log_t::e_log_net, "unable to set timeout");
        }
    }

    int res = ::connect((SOCKET)socket_, (sockaddr *)&addr, sizeof(addr));
    if (res==SOCKET_ERROR) {
        LOG(log_t::e_log_net, "socket connect() failed");
        return !(error_ = true);
    }
    LOG(log_t::e_log_net, "socket connected");
    return !error_;
}

bool client_t::send(const nomad::event_t & src) {

    if (socket_==INVALID_SOCKET)
        return !(error_ = true);

    nomad::event_header_t hdr;
    hdr.size_ = src.header_.size_;
    hdr.type_ = src.header_.type_;
    hdr.checksum_ = checksum((const uint8_t *)src.body_.data(), hdr.size_);

    assert(hdr.checksum_==src.header_.checksum_);

    // send off the packet header
    int r = ::send(socket_, (char *)&hdr, sizeof(hdr), 0);
    if (r==SOCKET_ERROR||r!=sizeof(hdr)) {
        LOGF(log_t::e_log_net, "socket send() error");
        return !(error_ = true);
    }

    // if the packet has a body
    if (hdr.size_) {
        assert(src.body_.data());
        // send off the packet body
        r = ::send(socket_, (char *)src.body_.data(), hdr.size_, 0);
        if (r==SOCKET_ERROR||r!=hdr.size_) {
            LOGF(log_t::e_log_net, "socket send() error");
            return !(error_ = true);
        }
    }

    return !error_;
}

} // namespace net
