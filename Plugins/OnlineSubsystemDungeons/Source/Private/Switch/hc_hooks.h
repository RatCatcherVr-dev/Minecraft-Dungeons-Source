#pragma once

#include <httpClient/pal.h>
#include "hc_switch_platform.h"

namespace WebSocket
{

class Context;

}

HRESULT InitHttpClientHooks(WebSocket::Context** contextPtr);

void UpdateWebSocketHooks(WebSocket::Context* contextPtr);

void CleanupWebSocketHooks(WebSocket::Context* contextPtr);
