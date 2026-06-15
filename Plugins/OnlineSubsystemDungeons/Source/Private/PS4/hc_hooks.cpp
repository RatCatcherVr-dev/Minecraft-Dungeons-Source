#include "hc_hooks.h"

#include <httpClient/trace.h>

HC_DEFINE_TRACE_AREA(HC_HOOKS, HCTraceLevel::Important);

HRESULT InitTraceHooks();
HRESULT InitHttpHooks(int libhttp2CtxId);
HRESULT InitWebSocketHooks(WebSocket::Context** contextPtr, int libsslCtxId, int dnsResolver);

HRESULT InitHttpClientHooks(WebSocket::Context** contextPtr, int libhttp2CtxId, int libsslCtxId, int dnsResolver)
{
    HRESULT hr = InitTraceHooks();
    if (FAILED(hr)) { return hr; }


    hr = InitHttpHooks(libhttp2CtxId);
    if (FAILED(hr)) { return hr; }

    hr = InitWebSocketHooks(contextPtr, libsslCtxId, dnsResolver);
    if (FAILED(hr)) { return hr; }

    return S_OK;
}
