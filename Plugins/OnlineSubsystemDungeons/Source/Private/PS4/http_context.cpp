#include "http_context.h"

#include <cassert>
#include <httpClient/async.h>
#include <httpClient/trace.h>

#include <libhttp2.h>

#include "http_wrapper.h"

namespace HttpLib
{

Context::Context() noexcept:
    m_stopWorker{ false },
    m_templateId{ -1 }
{
}

HRESULT Context::Init(Worker* worker, std::unique_ptr<Context>& newCtx, int libhttp2CtxId)
{
    assert(worker);
    assert(!newCtx);

    std::unique_ptr<Context> ctx{ new (std::nothrow) Context{} };
    if (!ctx)
    {
        HC_TRACE_ERROR(HC_HTTPLIB, "Failed to allocate CurlContext object");
        return E_OUTOFMEMORY;
    }

    ctx->m_worker = std::thread{ worker, ctx.get() };

    HRESULT hr = ctx->_init(libhttp2CtxId);
    if (FAILED(hr)) { return hr; }

    newCtx = std::move(ctx);
    return S_OK;
}

HRESULT Context::_init(int libhttp2CtxId) noexcept
{
    m_templateId = sceHttp2CreateTemplate(libhttp2CtxId, "Http-sample-agent/0.1", SCE_HTTP2_VERSION_2_0, SCE_TRUE);
    if (m_templateId < 0)
    {
        return E_FAIL;
    }

    // This can be used to allow Fiddler to decrypt requests/responses.
    // Adding Fiddler's rootCA via sceSslLoadCert() is supposed to work too.
    //const uint32_t sslFlags = SCE_HTTP2_SSL_FLAG_SERVER_VERIFY;
    //int sceResult = sceHttp2SslDisableOption(m_templateId, sslFlags);
    //if (sceResult < 0) { return E_FAIL; }

    return S_OK;
}

Context::~Context() noexcept
{
    {
        auto lock = Lock();
        m_stopWorker = true;
    }

    m_cv.notify_all();
    m_worker.join();
}

void Context::Push(HCRequest r) noexcept
{
    assert(r.call);
    assert(r.asyncBlock);

    try
    {
        auto lock = Lock();

        m_pendingRequests.emplace_back(r);
    }
    catch (std::bad_alloc&)
    {
        XAsyncComplete(r.asyncBlock, E_OUTOFMEMORY, 0);
    }

    m_cv.notify_all();
}

bool Context::Pop(HCRequest& r) noexcept
{
    auto lock = Lock();

    if (m_pendingRequests.empty())
    {
        return false;
    }

    r = m_pendingRequests.front();
    m_pendingRequests.pop_front();

    assert(r.call);
    assert(r.asyncBlock);

    return true;
}

bool Context::Wait() const noexcept
{
    auto lock = Lock();

    while (m_pendingRequests.empty() && !m_stopWorker)
    {
        m_cv.wait(lock);
    }

    return m_stopWorker;
}

int Context::GetTemplateId() const noexcept
{
    return m_templateId;
}

std::unique_lock<std::mutex> Context::Lock() const noexcept
{
    return std::unique_lock<std::mutex>{ m_mutex };
}

}
