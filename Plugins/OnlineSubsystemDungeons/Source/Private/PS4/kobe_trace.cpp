#include <cassert>
#include <thread>
#include <httpClient/pal.h>
#include <httpClient/trace.h>

namespace
{

uint64_t GetThreadId(void* /*ctx*/) noexcept
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

void TraceMessage(char const* /*area*/, HCTraceLevel /*level*/, char const* message, void* /*ctx*/) noexcept
{
    printf("%s", message);
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
