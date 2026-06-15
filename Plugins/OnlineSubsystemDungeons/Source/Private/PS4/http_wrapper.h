#pragma once

#include <httpClient/pal.h>
#include <httpClient/trace.h>

HC_DECLARE_TRACE_AREA(HC_HTTPLIB);

namespace HttpLib
{

HRESULT Create(int& newRequestId, int templateId, const char* method, const char* url) noexcept;
HRESULT AddHeader(int requestId, const char* name, const char* value) noexcept;
HRESULT SetRequestContentLength(int requestId, uint64_t contentLength) noexcept;
HRESULT Send(int requestId, const void* postData, size_t size) noexcept;

HRESULT GetStatus(int requestId, int32_t* statusCode) noexcept;
HRESULT GetResponseHeaders(int requestId, char** headers, size_t* headerSize) noexcept;
HRESULT GetResponseContentLength(int requestId, int32_t* result, uint64_t* contentLength) noexcept;
HRESULT GetResponseBody(int requestId, void* data, size_t size, int& bytesRead) noexcept;
HRESULT Delete(int requestId) noexcept;
}