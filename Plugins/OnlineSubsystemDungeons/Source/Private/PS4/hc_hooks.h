#pragma once

#include <httpClient/pal.h>
#include <hc_orbis_platform.h>

namespace WebSocket
{

class Context;

}

HRESULT InitHttpClientHooks(WebSocket::Context** contextPtr, int libhttp2CtxId, int libsslCtxId, int dnsResolver);

void UpdateWebSocketHooks(WebSocket::Context* contextPtr);

void CleanupWebSocketHooks(WebSocket::Context* contextPtr);
