#pragma once

#include <deque>
#include <httpClient/async.h>
#include <memory>
#include <mutex>
#include <thread>

namespace HttpLib
{

struct HCRequest
{
    HCCallHandle call; // non owning
    XAsyncBlock* asyncBlock; // non owning

    HCRequest() : call{ nullptr }, asyncBlock{ nullptr } {}
    HCRequest(HCCallHandle c, XAsyncBlock* b) : call{ c }, asyncBlock{ b } {}
};

struct Context;

using Worker = void(Context*);

struct Context
{
public:
    Context() noexcept;
    static HRESULT Init(Worker* worker, std::unique_ptr<Context>& newCtx, int libhttp2CtxId);
    ~Context() noexcept;

    void Push(HCRequest r) noexcept;

    bool Pop(HCRequest& r) noexcept;
    bool Wait() const noexcept;

    int GetTemplateId() const noexcept;

private:
    HRESULT _init(int libhttp2CtxId) noexcept;

    std::unique_lock<std::mutex> Lock() const noexcept;

    std::thread m_worker;
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_cv;

    std::deque<HCRequest> m_pendingRequests;
    bool m_stopWorker;

    int m_templateId;
};

}
