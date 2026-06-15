#pragma once

#include <nn/socket/socket_Api.h>

typedef int32 SOCKET;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (SOCKET)(~0)
#endif
#define SOCKET_ERROR            (-1)
