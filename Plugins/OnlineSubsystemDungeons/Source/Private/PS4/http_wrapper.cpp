#include "http_wrapper.h"

#include <libhttp.h>
#include <libhttp2.h>

namespace HttpLib
{

HRESULT Create(int& newRequestId, int templateId, const char* method, const char* url) noexcept
{
    newRequestId = sceHttp2CreateRequestWithURL(templateId, method, url, 0);
    if (newRequestId < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2CreateRequestWithURL failed: 0x%08X", newRequestId);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT AddHeader(int requestId, const char* name, const char* value) noexcept
{
    int sceResult = sceHttp2AddRequestHeader(requestId, name, value, SCE_HTTP2_HEADER_ADD);
    if (sceResult == SCE_HTTP_ERROR_INVALID_VALUE)
    {
        // Content-Length, Connection, Proxy-Connection headers cannot be set according to documentation
        // Attempting to do so will return error SCE_HTTP_ERROR_INVALID_VALUE (not SCE_HTTP2_ERROR_INVALID_VALUE)
        return S_OK;
    }
    else if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2AddRequestHeader failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT SetRequestContentLength(int requestId, uint64_t contentLength) noexcept
{
    int sceResult = sceHttp2SetRequestContentLength(requestId, contentLength);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2SetRequestContentLength failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT Send(int requestId, const void* postData, size_t size) noexcept
{
    int sceResult = sceHttp2SendRequest(requestId, postData, size);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2SendRequest failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT GetStatus(int requestId, int32_t* statusCode) noexcept
{
    int sceResult = sceHttp2GetStatusCode(requestId, statusCode);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2GetStatusCode failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT GetResponseHeaders(int requestId, char** headers, size_t* headerSize) noexcept
{
    int sceResult = sceHttp2GetAllResponseHeaders(requestId, headers, headerSize);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2GetAllResponseHeaders failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT GetResponseContentLength(int requestId, int32_t* result, uint64_t* contentLength) noexcept
{
    int sceResult = sceHttp2GetResponseContentLength(requestId, result, contentLength);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2GetResponseContentLength failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

HRESULT GetResponseBody(int requestId, void* data, size_t size, int& bytesRead) noexcept
{
    int sceResult = sceHttp2ReadData(requestId, data, size);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2ReadData failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    bytesRead = sceResult;
    return S_OK;
}

HRESULT Delete(int requestId) noexcept
{
    int sceResult = sceHttp2DeleteRequest(requestId);
    if (sceResult < 0)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "sceHttp2DeleteRequest failed: 0x%08X", sceResult);
        return E_FAIL;
    }
    return S_OK;
}

}
