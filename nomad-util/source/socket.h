#pragma once

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__linux__)
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
typedef int      SOCKET;
static const int INVALID_SOCKET = -1;
static const int SOCKET_ERROR   = -1;
#endif
