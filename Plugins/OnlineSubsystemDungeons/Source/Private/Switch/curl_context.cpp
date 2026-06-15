#include "curl_context.h"

#include <cassert>
#include <httpClient/async.h>
#include <httpClient/trace.h>

#include "curl_wrapper.h"

namespace Curl
{

Context::Context() noexcept:
    m_stopWorker{ false }
{}

HRESULT Context::Init(Worker* worker, std::unique_ptr<Context>& newCtx)
{
    assert(worker);
    assert(!newCtx);

    std::unique_ptr<Context> ctx{ new (std::nothrow) Context{} };
    if (!ctx)
    {
        HC_TRACE_ERROR(HC_CURL, "Failed to allocate CurlContext object");
        return E_OUTOFMEMORY;
    }

    ctx->m_worker = std::thread{ worker, ctx.get() };

    newCtx = std::move(ctx);
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

std::unique_lock<std::mutex> Context::Lock() const noexcept
{
    return std::unique_lock<std::mutex>{ m_mutex };
}

}
