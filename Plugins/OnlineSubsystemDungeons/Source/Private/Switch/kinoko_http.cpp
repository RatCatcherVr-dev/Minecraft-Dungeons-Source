#include <cassert>
#include <httpClient/async.h>
#include <httpClient/httpClient.h>
#include <httpClient/httpProvider.h>
#include <httpClient/trace.h>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <nn/os.h>
#include <nn/socket.h>
#include <nn/ssl.h>
#include <string>

#include "curl_context.h"
#include "curl_wrapper.h"
#include "ssl_context.h"

HC_DEFINE_TRACE_AREA(HC_CURL, HCTraceLevel::Important);

namespace Http
{

namespace
{

struct RequestStoreItem
{
    Curl::Request curl;
    Curl::HCRequest hc;

    std::list<std::string> headersBuffer;
    curl_slist* headers = nullptr;

    size_t bodyCopied = 0;

    std::string errorBuffer;
};

using RequestStore = std::map<CURL*, RequestStoreItem>;

class Request
{
public:
    Request() noexcept = default;

    static Request FromCallbackContext(void* cbCtx) noexcept;
    void* ToCallbackContex() const noexcept;

    Curl::Request const& Curl() const noexcept;
    CURL* CurlRaw() const noexcept;
    HCCallHandle HCHandle() const noexcept;
    XAsyncBlock* AsyncBlock() const noexcept;
    std::string const& ErrorBuffer() const noexcept;

    HRESULT DisableHeader(char const* name) noexcept;
    HRESULT MakeHeaders(curl_slist*& headers) noexcept;

    HRESULT CopyNextBodySection(void* buffer, size_t maxSize, size_t& bytesCopied) const noexcept;
private:
    friend class RequestTracker;

    HRESULT AppendHeader(char const* name, char const* value) noexcept;

    RequestStoreItem* m_self; // non owning
};

class RequestTracker
{
public:
    HRESULT Make(Curl::HCRequest hc, Request& r) noexcept;
    HRESULT Find(CURL* curl, Request& r) noexcept;
    void Fail(Request&& r, HRESULT hr) noexcept;
    void Succeed(Request&& r) noexcept;

private:
    void DestroyRequest(Request&& r) noexcept;

    RequestStore m_store;
};

void DoWork(Curl::Context* self) noexcept;

void CurlBeginRequests(
    Curl::Multi const&,
    RequestTracker& tracker,
    Ssl::Context const& ssl,
    Curl::HCRequest request
) noexcept;
HRESULT CurlProcessMessages(Curl::Multi const& curl, RequestTracker& tracker) noexcept;

//------------------------------------------------------------------------------
// Curl callbacks
//------------------------------------------------------------------------------

size_t ReadCallback(char* buffer, size_t size, size_t nitems, void* cbCtx) noexcept;
size_t WriteHeaderCallback(char* buffer, size_t size, size_t nitems, void* cbCtx) noexcept;
size_t WriteDataCallback(char* buffer, size_t size, size_t nmemb, void* cbCtx) noexcept;
int DebugCallback(CURL* curl, curl_infotype type, char* data, size_t size, void* ctx) noexcept;

//------------------------------------------------------------------------------
// libHttpClient perform function
//------------------------------------------------------------------------------

void PerformCurlCall(
    _In_ HCCallHandle call,
    _Inout_ XAsyncBlock* asyncBlock,
    _In_opt_ void* context,
    _In_ HCPerformEnv /*env*/
) noexcept
{
    assert(call);
    assert(asyncBlock);
    assert(context);

    auto ctx = static_cast<Curl::Context*>(context);
    ctx->Push(Curl::HCRequest{ call, asyncBlock });
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

Curl::Request const& Request::Curl() const noexcept
{
    return m_self->curl;
}

CURL* Request::CurlRaw() const noexcept
{
    return m_self->curl.get();
}

HCCallHandle Request::HCHandle() const noexcept
{
    return m_self->hc.call;
}

XAsyncBlock* Request::AsyncBlock() const noexcept
{
    return m_self->hc.asyncBlock;
}

std::string const& Request::ErrorBuffer() const noexcept
{
    return m_self->errorBuffer;
}

HRESULT Request::DisableHeader(char const* name) noexcept
{
    return AppendHeader(name, ""); // curl will add Expect: 100 as soon as we think of using a body
}

HRESULT Request::MakeHeaders(curl_slist*& headers) noexcept
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

        hr = AppendHeader(name, value);
        if (FAILED(hr)) { return hr; }

        HC_TRACE_INFORMATION(HC_CURL, "--- %s: %s", name, value);
    }

    headers = m_self->headers;
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

HRESULT Request::AppendHeader(char const* name, char const* value) noexcept
{
    int required = std::snprintf(nullptr, 0, "%s: %s", name, value);
    assert(required > 0);

    m_self->headersBuffer.emplace_back();
    auto& header = m_self->headersBuffer.back();

    header.resize(static_cast<size_t>(required) + 1, '\0');
    int written = std::snprintf(&header[0], header.size(), "%s: %s", name, value);
    assert(written == required);
    (void)written;

    header.resize(header.size() - 1); // drop null terminator

    curl_slist*& list = m_self->headers;
    list = curl_slist_append(list, header.c_str());

    return S_OK;
}

HRESULT RequestTracker::Make(Curl::HCRequest hc, Request& r) noexcept
{
    try
    {
        Curl::Request curl;
        HRESULT hr = Curl::EasyInit(curl);
        if (FAILED(hr))
        {
            return hr;
        }

        CURL* curlRaw = curl.get();

        auto res = m_store.emplace(curlRaw, RequestStoreItem{ std::move(curl), hc });
        assert(res.second);

        HC_TRACE_INFORMATION(HC_CURL, "Created curl request %p for HCCallHandle %p", curlRaw, hc.call);

        r.m_self = &res.first->second;
        r.m_self->errorBuffer.resize(CURL_ERROR_SIZE);
        return S_OK;
    }
    catch (std::bad_alloc&)
    {
        return E_OUTOFMEMORY;
    }

    return S_OK;
}

HRESULT RequestTracker::Find(CURL* curl, Request& r) noexcept
{
    HC_TRACE_INFORMATION(HC_CURL, "Looking up curl request %p", curl);

    auto it = m_store.find(curl);
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
    HC_TRACE_INFORMATION(HC_CURL, "Failing curl request %p with result 0x%08X", r.CurlRaw(), hr);

    assert(FAILED(hr));
    XAsyncComplete(r.AsyncBlock(), hr, 0);

    DestroyRequest(std::move(r));
}

void RequestTracker::Succeed(Request&& r) noexcept
{
    HC_TRACE_INFORMATION(HC_CURL, "Succeeding curl request %p", r.CurlRaw());

    XAsyncComplete(r.AsyncBlock(), S_OK, 0);

    DestroyRequest(std::move(r));
}

void RequestTracker::DestroyRequest(Request&& r) noexcept
{
    auto& entry = *r.m_self;

    auto curl = entry.curl.get();
    entry.curl.reset();

    curl_slist_free_all(entry.headers);

    m_store.erase(curl);
}

//------------------------------------------------------------------------------
// Curl wrappers
//------------------------------------------------------------------------------

void DoWork(Curl::Context* ctx) noexcept
{
    nn::os::SetThreadName(nn::os::GetCurrentThread(), "CurlContext::DoWork");
    nn::os::SetThreadCoreMask(nn::os::GetCurrentThread(), 1, nn::os::GetThreadAvailableCoreMask());
    nn::os::YieldThread(); // core mask won't change until the thread is resumed

    Curl::Multi curl;
    HRESULT hr = Curl::MultiInit(curl);
    if (FAILED(hr)) { std::terminate(); }

    RequestTracker tracker{};

    Ssl::Context ssl = Ssl::Make();

    CURLMcode res = CURLM_OK;

    while (true)
    {
        int runningHandles = 0;
        while (true)
        {
            Curl::HCRequest r;
            while (ctx->Pop(r))
            {
                CurlBeginRequests(curl, tracker, ssl, r);
            }

            res = curl_multi_perform(curl.get(), &runningHandles);
            assert(res == CURLM_OK);

            if (runningHandles > 0)
            {
                break;
            }

            // process the last batch of messages before going to sleep
            CurlProcessMessages(curl, tracker); // TODO handle hr

            bool shouldStop = ctx->Wait();

            if (shouldStop)
            {
                return;
            }
        }

        nn::socket::FdSet fdread{};
        nn::socket::FdSet fdwrite{};
        nn::socket::FdSet fdexcep{};
        nn::socket::FdSetZero(&fdread);
        nn::socket::FdSetZero(&fdwrite);
        nn::socket::FdSetZero(&fdexcep);

        int maxfd = -1;
        res = curl_multi_fdset(curl.get(), &fdread, &fdwrite, &fdexcep, &maxfd);
        assert(res == CURLM_OK);

        long timeoutMs = 0;
        res = curl_multi_timeout(curl.get(), &timeoutMs);
        assert(res == CURLM_OK);
        if (timeoutMs < 0) { timeoutMs = 100; }

        std::chrono::microseconds timeout{ std::chrono::milliseconds{ timeoutMs } };
        std::chrono::seconds timeout_seconds = std::chrono::duration_cast<std::chrono::seconds>(timeout);
        std::chrono::microseconds timeout_fraction = timeout - timeout_seconds;

        if (maxfd < 0)
        {
            HC_TRACE_SCOPE_VERBOSE(HC_CURL);
            HC_TRACE_VERBOSE(HC_CURL, "--- Sleeping while curl is blocking");
            HC_TRACE_VERBOSE(HC_CURL, "---     timeout suggested: %d.%06d",
                timeout_seconds.count(), timeout_fraction.count());

            std::chrono::microseconds sleepTime = std::chrono::milliseconds{ 100 };
            sleepTime = std::min(sleepTime, timeout); // we sleep for at most 100ms

            if (sleepTime > std::chrono::milliseconds{ 0 })
            {
#if HC_TRACE_VERBOSE_ENABLE
                std::chrono::seconds sleepTime_seconds = std::chrono::duration_cast<std::chrono::seconds>(sleepTime);
                std::chrono::microseconds sleepTime_fraction = sleepTime - sleepTime_seconds;
                HC_TRACE_VERBOSE(HC_CURL, "---     actual sleep time: %d.%06d",
                    sleepTime_seconds.count(), sleepTime_fraction.count());
#endif

                nn::os::SleepThread(sleepTime);
            }
            else
            {
                HC_TRACE_VERBOSE(HC_CURL, "---     timeout is 0ms, yielding");
                nn::os::YieldThread();
            }
        }
        else
        {
            HC_TRACE_SCOPE_VERBOSE(HC_CURL);
            HC_TRACE_VERBOSE(HC_CURL, "Blocking on select");
            HC_TRACE_VERBOSE(HC_CURL, "---     timeout: %d.%06d",
                timeout_seconds.count(), timeout_fraction.count());

            nn::socket::TimeVal timeout{};
            timeout.tv_sec = timeout_seconds.count();
            timeout.tv_usec = timeout_fraction.count();

            int fd = nn::socket::Select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
            assert(fd >= 0);

            if (fd == 0)
            {
                HC_TRACE_VERBOSE(HC_CURL, "---     timed out");
            }
            else
            {
                HC_TRACE_VERBOSE(HC_CURL, "---     something happened");
                CurlProcessMessages(curl, tracker); // TODO handle hr
            }
        }
    }
}

void CurlBeginRequests(
    Curl::Multi const& curl,
    RequestTracker& tracker,
    Ssl::Context const& ssl,
    Curl::HCRequest request
) noexcept
{
    HC_TRACE_SCOPE_IMPORTANT(HC_CURL);

    HRESULT hr = S_OK;

    Request r;
    hr = tracker.Make(request, r);
    if (FAILED(hr))
    {
        XAsyncComplete(request.asyncBlock, hr, 0);
        return;
    }

    // body (first so we can override things curl "helpfully" sets for us)
    uint8_t const* body = nullptr;
    uint32_t bodySize = 0;
    hr = HCHttpCallRequestGetRequestBodyBytes(r.HCHandle(), &body, &bodySize);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    if (bodySize > 0)
    {
        // we set both POSTFIELDSIZE and INFILESIZE because curl uses one or the
        // other depending on method
        hr = Curl::EasySetOpt(r.Curl(), CURLOPT_POSTFIELDSIZE, static_cast<long>(bodySize));
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
        hr = Curl::EasySetOpt(r.Curl(), CURLOPT_INFILESIZE, static_cast<long>(bodySize));
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
        hr = Curl::EasySetOpt(r.Curl(), CURLOPT_READFUNCTION, &ReadCallback);
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
        hr = Curl::EasySetOpt(r.Curl(), CURLOPT_READDATA, r.ToCallbackContex());
        if (FAILED(hr))
        {
            tracker.Fail(std::move(r), hr);
            return;
        }
        r.DisableHeader("Expect");
    }

    // url & method
    char const* url = nullptr;
    char const* method = nullptr;
    hr = HCHttpCallRequestGetUrl(r.HCHandle(), &method, &url);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_URL, url);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

	if (strcmp(method, "DELETE") == 0)
	{
		hr = Curl::EasySetOpt(r.Curl(), CURLOPT_CUSTOMREQUEST, "DELETE");
		if (FAILED(hr))
		{
			tracker.Fail(std::move(r), hr);
			return;
		}
	}
	else
	{
		CURLoption opt = CURLOPT_HTTPGET;

		hr = Curl::MethodStringToOpt(method, opt);
		if (FAILED(hr))
		{
			tracker.Fail(std::move(r), hr);
			return;
		}
		hr = Curl::EasySetOpt(r.Curl(), opt, 1);
		if (FAILED(hr))
		{
			tracker.Fail(std::move(r), hr);
			return;
		}
	}

    HC_TRACE_INFORMATION(HC_CURL, "--- %s %s", method, url);

    // headers
    curl_slist* headers = nullptr;
    hr = r.MakeHeaders(headers);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_HTTPHEADER, headers);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    uint32_t timeoutSeconds = 0;
    hr = HCHttpCallRequestGetTimeout(r.HCHandle(), &timeoutSeconds);

    HC_TRACE_INFORMATION(HC_CURL, "--- Timeout %us", timeoutSeconds);
    HC_TRACE_INFORMATION(HC_CURL, "--- Body size %zu", bodySize);
    HC_TRACE_INFORMATION(HC_CURL, "%.*s", bodySize, body);

    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_TIMEOUT, static_cast<long>(timeoutSeconds));
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    // curl config
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_SSL_CONTEXT, ssl.get());
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_VERBOSE, 0); // verbose logging (0 off, 1 on)
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_HEADER, 0); // do not write headers to the write callback
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_ERRORBUFFER, &r.ErrorBuffer()[0]);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    // callbacks
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_WRITEFUNCTION, &WriteDataCallback);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_WRITEDATA, r.ToCallbackContex());
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_HEADERFUNCTION, &WriteHeaderCallback);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_HEADERDATA, r.ToCallbackContex());
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    hr = Curl::EasySetOpt(r.Curl(), CURLOPT_DEBUGFUNCTION, &DebugCallback);
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }

    // go
    hr = Curl::MultiAddHandle(curl, r.Curl());
    if (FAILED(hr))
    {
        tracker.Fail(std::move(r), hr);
        return;
    }
}

HRESULT CurlProcessMessages(Curl::Multi const& curl, RequestTracker& tracker) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_CURL);

    while (true)
    {
        int remaining = 0;
        CURLMsg* msg = curl_multi_info_read(curl.get(), &remaining);
        if (!msg) { return S_OK; }

        HC_TRACE_VERBOSE(HC_CURL, "--- looking at message %p, %d remaining", msg, remaining);
        HC_TRACE_VERBOSE(HC_CURL, "---     msg: %d, request: %p", msg->msg, msg->easy_handle);

        switch (msg->msg)
        {
        case CURLMSG_NONE:
        {
            HC_TRACE_VERBOSE(HC_CURL, "---     CURLMSG_NONE: this message should not be in use");
            assert(false);
        }
        break;
        case CURLMSG_DONE:
        {
            CURLcode res = CURLE_OK;
            CURLcode requestResult = msg->data.result;
            HC_TRACE_VERBOSE(HC_CURL, "---     CURLMSG_DONE: request completed with %d: '%s'",
                res, curl_easy_strerror(requestResult));

            if (requestResult == CURLE_SSL_CONNECT_ERROR)
            {
                long hsRes = 0;
                res = curl_easy_getinfo(msg->easy_handle, CURLINFO_SSL_HANDSHAKE_RESULT, &hsRes);
                if (res != CURLE_OK) { return Curl::HrFromCurle(res); }

                nn::Result nnRes;
                if (nn::ssl::GetSslResultFromValue(
                    &nnRes,
                    reinterpret_cast<char*>(&hsRes),
                    sizeof(hsRes)).IsSuccess()
                )
                {
                    HC_TRACE_ERROR(HC_CURL, "--- SSL handshake error %d-%d: %08X",
                        nnRes.GetModule(), nnRes.GetDescription(), nnRes.GetInnerValueForDebug());
                }
            }

            char* url = nullptr;
            long httpStatus = 0;
            res = curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &url);
            if (res != CURLE_OK) { return Curl::HrFromCurle(res); }
            res = curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &httpStatus);
            if (res != CURLE_OK) { return Curl::HrFromCurle(res); }
            HC_TRACE_VERBOSE(HC_CURL, "---     url: %s - %d", url, httpStatus);

            CURLMcode multiRes = curl_multi_remove_handle(curl.get(), msg->easy_handle);
            if (multiRes != CURLM_OK) { return Curl::HrFromCurlm(multiRes); }

            Request r;
            HRESULT hr = tracker.Find(msg->easy_handle, r);
            if (FAILED(hr)) { return hr; }
            if (requestResult != CURLE_OK)
            {
                HC_TRACE_ERROR(HC_CURL, "--- Curl error buffer: '%s'", r.ErrorBuffer().c_str());

                hr = HCHttpCallResponseSetNetworkErrorCode(r.HCHandle(), E_FAIL, requestResult);
                if (FAILED(hr)) { return hr; }

                hr = HCHttpCallResponseSetPlatformNetworkErrorMessage(r.HCHandle(), curl_easy_strerror(requestResult));
                if (FAILED(hr)) { return hr; }
            }

            hr = HCHttpCallResponseSetStatusCode(r.HCHandle(), httpStatus);
            if (FAILED(hr)) { return hr; }

            tracker.Succeed(std::move(r));
        }
        break;
        case CURLMSG_LAST:
        {
            HC_TRACE_VERBOSE(HC_CURL, "---     CURLMSG_LAST: this message should not be in use");
            assert(false);
        }
        break;
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
// Curl callbacks
//------------------------------------------------------------------------------

size_t ReadCallback(char* buffer, size_t size, size_t nitems, void* cbCtx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_CURL);
    HC_TRACE_VERBOSE(HC_CURL, "--- reading body data (%llu items of size %llu)", nitems, size);

    size_t bufferSize = size * nitems;

    auto r = Request::FromCallbackContext(cbCtx);

    size_t copied = 0;
    HRESULT hr = r.CopyNextBodySection(buffer, bufferSize, copied);
    if (FAILED(hr))
    {
        assert(false);
        return CURL_READFUNC_ABORT;
    }

    return copied;
}

size_t WriteHeaderCallback(char* buffer, size_t size, size_t nitems, void* cbCtx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_CURL);
    HC_TRACE_INFORMATION(HC_CURL, "--- received header (%llu items of size %llu)", nitems, size);
#if HC_TRACE_INFORMATION_ENABLE
    if (size * nitems > 2)
    {
        HC_TRACE_INFORMATION(HC_CURL, "'%.*s'", size * nitems - 2, buffer); // -2 to avoid printing \r\n
    }
#endif

    size_t bufferSize = size * nitems;
    char const* current = buffer;
    char const* end = buffer + bufferSize;

    // scan for the end of the header name
    char const* name = current;
    size_t nameSize = 0;
    for (; current < end; ++current)
    {
        if (*current == ':')
        {
            nameSize = current - buffer;
            ++current;
            break;
        }
    }
    if (current == end)
    {
        // not a real header, drop it
        return bufferSize;
    }

    // skip whitespace
    for (; current < end && *current == ' '; ++current) // assume that Curl canonicalizes headers
    {}

    // scan for the end of the header value
    char const* value = current;
    size_t valueSize = 0;
    char const* valueStart = current;
    for (; current < end; ++current)
    {
        if (*current == '\r')
        {
            valueSize = current - valueStart;
            break;
        }
    }
    if (current == end)
    {
        // curl should always gives us the new lines at the end of the header
        assert(false);
    }

    auto r = Request::FromCallbackContext(cbCtx);

    HRESULT hr = HCHttpCallResponseSetHeaderWithLength(r.HCHandle(), name, nameSize, value, valueSize);
    assert(SUCCEEDED(hr));

    return bufferSize;
}

size_t WriteDataCallback(char* buffer, size_t /*size*/, size_t nmemb, void* cbCtx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_CURL);
    HC_TRACE_INFORMATION(HC_CURL, "--- received data (%llu bytes)", nmemb);
    HC_TRACE_INFORMATION(HC_CURL, "'%.*s'", nmemb, buffer);

    auto r = Request::FromCallbackContext(cbCtx);

    HRESULT hr = HCHttpCallResponseAppendResponseBodyBytes(r.HCHandle(), reinterpret_cast<uint8_t*>(buffer), nmemb);
    assert(SUCCEEDED(hr));

    return nmemb;
}

int DebugCallback(CURL* /*curl*/, curl_infotype type, char* data, size_t size, void* /*ctx*/) noexcept
{
    char const* event = "<unknown>";
    switch (type)
    {
    case CURLINFO_TEXT: event = "TEXT"; break;
    case CURLINFO_HEADER_OUT: event = "HEADER OUT"; break;
    case CURLINFO_DATA_OUT: event = "DATA OUT"; break;
    case CURLINFO_SSL_DATA_OUT: event = "SSL OUT"; break;
    case CURLINFO_HEADER_IN: event = "HEADER IN"; break;
    case CURLINFO_DATA_IN: event = "DATA IN"; break;
    case CURLINFO_SSL_DATA_IN: event = "SSL IN"; break;
    case CURLINFO_END: event = "END"; break;
    }

    if (type == CURLINFO_TEXT && data[size - 1] == '\n')
    {
        size -= 1;
    }

    HC_TRACE_IMPORTANT(HC_CURL, "CURL %10s - %.*s", event, size, data);

    return CURLE_OK;
}

}

}

HRESULT InitCurlHttpHooks()
{
    HRESULT hr = S_OK;

    std::unique_ptr<Curl::Context> ctx;
    hr = Curl::Context::Init(&Http::DoWork, ctx);
    if (FAILED(hr)) { return hr; }

    hr = HCSetHttpCallPerformFunction(&Http::PerformCurlCall, ctx.get());
    if (FAILED(hr)) { return hr; }

    // leak the context, so it is available from now on
    ctx.release();
    return S_OK;
}