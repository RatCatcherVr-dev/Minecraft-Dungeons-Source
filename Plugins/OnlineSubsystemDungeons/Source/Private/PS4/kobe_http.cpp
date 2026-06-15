#include <cassert>
#include <httpClient/async.h>
#include <httpClient/httpClient.h>
#include <httpClient/httpProvider.h>
#include <httpClient/trace.h>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "http_context.h"
#include "http_wrapper.h"

HC_DEFINE_TRACE_AREA(HC_HTTPLIB, HCTraceLevel::Important);

namespace Http {

namespace {

std::unique_ptr<HttpLib::Context> sHttpContext;

struct RequestStoreItem
{
    int requestId;
    HttpLib::HCRequest hc;

    std::list<std::string> headersBuffer;

    size_t bodyCopied = 0;
};

using RequestStore = std::map<int, RequestStoreItem>;

class Request
{
public:
    Request() noexcept = default;

    static Request FromCallbackContext(void* cbCtx) noexcept;
    void* ToCallbackContex() const noexcept;

    int RequestId() const noexcept;
    HCCallHandle HCHandle() const noexcept;
    XAsyncBlock* AsyncBlock() const noexcept;

    HRESULT AddHeaders() noexcept;

    HRESULT CopyNextBodySection(void* buffer, size_t maxSize, size_t& bytesCopied) const noexcept;
private:
    friend class RequestTracker;

    RequestStoreItem* m_self; // non owning
};

class RequestTracker
{
public:
    HRESULT Make(HttpLib::HCRequest hc, Request& r, int templateId, const char *method, const char* url) noexcept;
    HRESULT Find(int requestId, Request& r) noexcept;
    void Fail(Request&& r, HRESULT hr) noexcept;
    void Succeed(Request&& r) noexcept;

private:
    void DestroyRequest(Request&& r) noexcept;

    RequestStore m_store;
};

void DoWork(HttpLib::Context* self) noexcept;

void HttpBeginRequests(
    RequestTracker& tracker,
    HttpLib::HCRequest request,
    int templateId
) noexcept;

//------------------------------------------------------------------------------
// libHttpClient perform function
//------------------------------------------------------------------------------

void PerformHttpCall(
    _In_ HCCallHandle call,
    _Inout_ XAsyncBlock* asyncBlock,
    _In_opt_ void* context,
    _In_ HCPerformEnv /*env*/
) noexcept
{
    assert(call);
    assert(asyncBlock);
    assert(context);

    auto ctx = static_cast<HttpLib::Context*>(context);
    ctx->Push(HttpLib::HCRequest{ call, asyncBlock });
}

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

Request Request::FromCallbackContext(void* cbCtx) noexcept
{
    assert(cbCtx);
    Request r;
    r.m_self = static_cast<RequestStoreItem*>(cbCtx);
    return r;
}

void* Request::ToCallbackContex() const noexcept
{
    return m_self;
}

int Request::RequestId() const noexcept
{
    return m_self->requestId;
}

HCCallHandle Request::HCHandle() const noexcept
{
    return m_self->hc.call;
}

XAsyncBlock* Request::AsyncBlock() const noexcept
{
    return m_self->hc.asyncBlock;
}

HRESULT Request::AddHeaders() noexcept
{
    uint32_t headerCount = 0;
    HRESULT hr = HCHttpCallRequestGetNumHeaders(HCHandle(), &headerCount);
    if (FAILED(hr)) { return hr; }

    for (auto i = 0u; i < headerCount; ++i)
    {
        char const* name = nullptr;
        char const* value = nullptr;
        hr = HCHttpCallRequestGetHeaderAtIndex(HCHandle(), i, &name, &value);
        if (FAILED(hr)) { return hr; }

        hr = HttpLib::AddHeader(RequestId(), name, value);
        if (FAILED(hr)) { return hr; }

        HC_TRACE_INFORMATION(HC_HTTPLIB, "--- %s: %s", name, value);
    }

    return S_OK;
}

HRESULT Request::CopyNextBodySection(void* buffer, size_t maxSize, size_t& bytesCopied) const noexcept
{
    assert(buffer);

    uint8_t const* body = nullptr;
    uint32_t bodySize = 0;
    HRESULT hr = HCHttpCallRequestGetRequestBodyBytes(HCHandle(), &body, &bodySize);
    if (FAILED(hr)) { return hr; }

    size_t toCopy = 0;

    if (m_self->bodyCopied == bodySize)
    {
        bytesCopied = 0;
        return S_OK;
    }
    else if (maxSize >= bodySize - m_self->bodyCopied)
    {
        // copy everything
        toCopy = bodySize - m_self->bodyCopied;
    }
    else
    {
        // copy as much as we can
        toCopy = maxSize;
    }

    void const* startCopyFrom = body + m_self->bodyCopied;
    assert(startCopyFrom < body + bodySize);

    m_self->bodyCopied += toCopy;
    assert(m_self->bodyCopied <= bodySize);

    assert(toCopy <= maxSize);
    memcpy(buffer, startCopyFrom, toCopy);

    bytesCopied = toCopy;
    return S_OK;
}

HRESULT RequestTracker::Make(HttpLib::HCRequest hc, Request& r, int templateId, const char *method, const char* url) noexcept
{
    try
    {
        int requestId;
        HRESULT hr = HttpLib::Create(requestId, templateId, method, url);
        if (FAILED(hr))
        {
            return hr;
        }

        auto res = m_store.emplace(requestId, RequestStoreItem{ requestId, hc });
        assert(res.second);

        HC_TRACE_INFORMATION(HC_HTTPLIB, "Created http request %d for HCCallHandle %p", requestId, hc.call);

        r.m_self = &res.first->second;
        return S_OK;
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT RequestTracker::Find(int requestId, Request& r) noexcept
{
    HC_TRACE_INFORMATION(HC_HTTPLIB, "Looking up http request %d", requestId);

    auto it = m_store.find(requestId);
    if (it == m_store.end())
    {
        assert(false);
        return E_FAIL;
    }

    r.m_self = &it->second;
    return S_OK;
}

void RequestTracker::Fail(Request&& r, HRESULT hr) noexcept
{
    HC_TRACE_INFORMATION(HC_HTTPLIB, "Failing http request %d with result 0x%08X", r.RequestId(), hr);

    assert(FAILED(hr));
    XAsyncComplete(r.AsyncBlock(), hr, 0);

    DestroyRequest(std::move(r));
}

void RequestTracker::Succeed(Request&& r) noexcept
{
    HC_TRACE_INFORMATION(HC_HTTPLIB, "Succeeding http request %d", r.RequestId());

    XAsyncComplete(r.AsyncBlock(), S_OK, 0);

    DestroyRequest(std::move(r));
}

void RequestTracker::DestroyRequest(Request&& r) noexcept
{
    int requestId = r.RequestId();
    HttpLib::Delete(requestId);

    m_store.erase(requestId);
}

//------------------------------------------------------------------------------
// Http wrappers
//------------------------------------------------------------------------------

void DoWork(HttpLib::Context* ctx) noexcept
{
    scePthreadSetaffinity(pthread_self(), 0b00001111);
    scePthreadRename(pthread_self(), "HttpContext::DoWork");

    RequestTracker tracker{};

    while (true)
    {
        int runningHandles = 0;
        while (true)
        {
            HttpLib::HCRequest r;
            while (ctx->Pop(r))
            {
                HttpBeginRequests(tracker, r, ctx->GetTemplateId());
            }

            //res = curl_multi_perform(curl.get(), &runningHandles);
            //assert(res == CURLM_OK);

            if (runningHandles > 0)
            {
                break;
            }

            // process the last batch of messages before going to sleep
            //CurlProcessMessages(curl, tracker); // TODO handle hr

            bool shouldStop = ctx->Wait();

            if (shouldStop)
            {
                return;
            }
        }


    }
}

void HttpBeginRequests(
    RequestTracker& tracker,
    HttpLib::HCRequest request,
    int templateId
) noexcept
{
    HC_TRACE_SCOPE_IMPORTANT(HC_HTTPLIB);

    HRESULT hr = S_OK;

    // url & method
    char const* url = nullptr;
    char const* method = nullptr;
    hr = HCHttpCallRequestGetUrl(request.call, &method, &url);
    if (FAILED(hr))
    {
        XAsyncComplete(request.asyncBlock, hr, 0);
        return;
    }

    Request r;
    hr = tracker.Make(request, r, templateId, method, url);
    if (FAILED(hr))
    {
        XAsyncComplete(request.asyncBlock, hr, 0);
        return;
    }

    // headers
    hr = r.AddHeaders();
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    // body
    uint8_t const* body = nullptr;
    uint32_t bodySize = 0;
    hr = HCHttpCallRequestGetRequestBodyBytes(request.call, &body, &bodySize);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    if (bodySize > 0)
    {
        hr = HttpLib::SetRequestContentLength(r.RequestId(), bodySize);
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
    }

    hr = HttpLib::Send(r.RequestId(), body, bodySize);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    // response
    int32_t statusCode;
    hr = HttpLib::GetStatus(r.RequestId(), &statusCode);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = HCHttpCallResponseSetStatusCode(r.HCHandle(), statusCode);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    int responseResult;
    uint64_t contentLength;
    hr = HttpLib::GetResponseContentLength(r.RequestId(), &responseResult, &contentLength);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    char* header;
    size_t headerSize;
    hr = HttpLib::GetResponseHeaders(r.RequestId(), &header, &headerSize);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    {
        // parse headers
        const char* current = header;
        const char* end = header + headerSize;
        const char* headerName = current;
        const char* headerValue = nullptr;
        size_t headerNameSize = 0;
        for (; current < end; ++current)
        {
            if (*current == ':')
            {
                if (headerNameSize == 0)
                {
                    headerNameSize = current - headerName;
                    headerValue = current + 1;
                }
            }
            else if (*current == ' ')
            {
                if (headerValue == current)
                {
                    ++headerValue;
                }
            }
            else if (*current == '\r')
            {
                if (headerNameSize > 0)
                {
                    size_t headerValueSize = current - headerValue;
                    hr = HCHttpCallResponseSetHeaderWithLength(request.call, headerName, headerNameSize, headerValue, headerValueSize);
                    assert(SUCCEEDED(hr));
                }
                headerName = current + 1;
                headerNameSize = 0;
                headerValue = nullptr;
            }
            else if (*current == '\n')
            {
                headerName = current + 1;
            }
        }
    }

    int totalBytesRead = 0;
    uint8_t responseBuffer[1024];
    while (totalBytesRead < contentLength)
    {
        int bytesRead = 0;
        hr = HttpLib::GetResponseBody(r.RequestId(), responseBuffer, 1024, bytesRead);
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
        totalBytesRead += bytesRead;

        hr = HCHttpCallResponseAppendResponseBodyBytes(request.call, responseBuffer, bytesRead);
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
    }

    tracker.Succeed(std::move(r));
}

}

}

HRESULT InitHttpHooks(int libhttp2CtxId)
{
    HRESULT hr = S_OK;

    hr = HttpLib::Context::Init(&Http::DoWork, Http::sHttpContext, libhttp2CtxId);
    if (FAILED(hr)) { return hr; }

    hr = HCSetHttpCallPerformFunction(&Http::PerformHttpCall, Http::sHttpContext.get());
    if (FAILED(hr)) { return hr; }

    return S_OK;
}