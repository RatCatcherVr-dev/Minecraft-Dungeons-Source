#include <cassert>
#include <httpClient/pal.h>
#include <httpClient/trace.h>
#include <nn/nn_Log.h>
#include <nn/os.h>

namespace
{

uint64_t GetThreadId(void* /*ctx*/) noexcept
{
    nn::os::ThreadType* t = nn::os::GetCurrentThread();
    nn::os::ThreadId id = nn::os::GetThreadId(t);

    return id;
}

void TraceMessage(char const* /*area*/, HCTraceLevel /*level*/, char const* message, void* /*ctx*/) noexcept
{
    NN_PUT(message, strlen(message));
}

}

// For this test code we want to manually init tracing before initializing
// libHttpClient.
void HCTraceImplInit() noexcept;

HRESULT InitTraceHooks()
{
    HRESULT hr = HCTraceSetPlatformCallbacks(&GetThreadId, nullptr, &TraceMessage, nullptr);
    if (FAILED(hr)) { assert(false); return hr; }

    HCTraceImplInit();

    return S_OK;
}
