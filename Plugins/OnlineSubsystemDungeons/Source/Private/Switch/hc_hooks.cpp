#include "hc_hooks.h"

#include <httpClient/trace.h>
#include <nn/nifm.h>
#include <nn/socket.h>
#include <nn/ssl.h>

HC_DEFINE_TRACE_AREA(HC_HOOKS, HCTraceLevel::Important);

HRESULT InitTraceHooks();
HRESULT InitCurlHttpHooks();
HRESULT InitWebSocketHooks(WebSocket::Context** contextPtr);

namespace
{

nn::socket::Config& SocketConfig()
{
    static nn::socket::ConfigDefaultWithMemory config;
    return config;
}

}

HRESULT InitHttpClientHooks(WebSocket::Context** contextPtr)
{
    HRESULT hr = InitTraceHooks();
    if (FAILED(hr)) { return hr; }

    nn::Result res =  nn::nifm::Initialize();
    if (res.IsFailure())
    {
        HC_TRACE_ERROR(HC_HOOKS, "Failed to init NIFM: %d-%d: %08X",
            res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());
        return E_UNEXPECTED;
    }
/*
    res = nn::socket::Initialize(SocketConfig());
    if (res.IsFailure())
    {
        HC_TRACE_ERROR(HC_HOOKS, "Failed to init socket library: %d-%d: %08X",
            res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());
        return E_UNEXPECTED;
    }
	
    res = nn::ssl::Initialize();
    if (res.IsFailure())
    {
        HC_TRACE_ERROR(HC_HOOKS, "Failed to init ssl library: %d-%d: %08X",
            res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());
        return E_UNEXPECTED;
    }
	*/
    hr = InitCurlHttpHooks();
    if (FAILED(hr)) { return hr; }

    hr = InitWebSocketHooks(contextPtr);
    if (FAILED(hr)) { return hr; }

    return S_OK;
}
