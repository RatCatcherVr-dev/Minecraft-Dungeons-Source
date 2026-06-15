#include "xal_hooks.h"

#include <cassert>
#include <fstream>
#include <httpClient/pal.h> // trace.h is not self sufficient
#include <httpClient/trace.h>
#include <nn/crypto.h>
#include <nn/oe.h>
#include <nn/fs.h>
#include <nn/time.h>
#include <nn/util/util_Uuid.h>
#include <string>
#include <vector>
#include <Xal/xal_platform.h>

HC_DEFINE_TRACE_AREA(XAL_HOOKS, HCTraceLevel::Important);

namespace
{

XalPlatformOperationResult GenerateUuid(void* /*context*/, XalUuid* newUuid)
{
    assert(newUuid);

    nn::util::Uuid uuid = nn::util::GenerateUuid();

    static_assert(sizeof(XalUuid) == sizeof(nn::util::Uuid), "Incompatible uuid sizes");
    memcpy(newUuid, &uuid, sizeof(XalUuid));

    return XalPlatformOperationResult_Success;
}

XalPlatformOperationResult GenerateRandomBytes(void* /*context*/, size_t bufferSize, uint8_t* buffer)
{
    assert(buffer);

    nn::crypto::GenerateCryptographicallyRandomBytes(buffer, bufferSize);
    return XalPlatformOperationResult_Success;
}

XalPlatformOperationResult UtcTimestamp(void* /*context*/, int64_t* seconds, uint32_t* /*subsecond*/)
{
    assert(seconds);

    nn::time::PosixTime t{};
    nn::Result res = nn::time::StandardNetworkSystemClock::GetCurrentTime(&t);
    if (res.IsFailure()) { return XalPlatformOperationResult_Failure; }

    *seconds = t.value;

    return XalPlatformOperationResult_Success;
}

XalPlatformOperationResult TimestampToComponents(
    void* /*context*/,
    int64_t secondsFromUnixEpoch,
    XalTimestampComponents* components
)
{
    assert(components);

    nn::time::PosixTime posixTime{ secondsFromUnixEpoch };
    nn::time::CalendarTime ct = nn::time::ToCalendarTimeInUtc(posixTime);
    if (!ct.IsValid()) { return XalPlatformOperationResult_Failure; }

    components->year = ct.year;
    components->month = ct.month;
    components->day = ct.day;
    components->hour = ct.hour;
    components->minute = ct.minute;
    components->second = ct.second;

    return XalPlatformOperationResult_Success;
}

XalPlatformOperationResult ComponentsToTimestamp(
    void* /*context*/,
    XalTimestampComponents const* components,
    int64_t* secondsFromUnixEpoch
)
{
    assert(components);
    assert(secondsFromUnixEpoch);

    nn::time::CalendarTime ct{};
    ct.year = components->year;
    ct.month = components->month;
    ct.day = components->day;
    ct.hour = components->hour;
    ct.minute = components->minute;
    ct.second = components->second;

    nn::time::PosixTime posixTime = nn::time::ToPosixTimeFromUtc(ct);

    *secondsFromUnixEpoch = posixTime.value;

    return XalPlatformOperationResult_Success;
}

}

HRESULT InitXalBasicHooks()
{
    HRESULT hr = S_OK;
    nn::Result res{};

    res = nn::time::Initialize();
    if (res.IsFailure())
    {
        return E_FAIL;
    }

    XalPlatformCryptoCallbacks crypto
    {
        &GenerateUuid,
        &GenerateRandomBytes,
        nullptr
    };

    hr = XalPlatformCryptoSetCallbacks(&crypto);
    if (FAILED(hr)) { return hr; }

    XalPlatformDateTimeCallbacks dateTime
    {
        &UtcTimestamp,
        &TimestampToComponents,
        &ComponentsToTimestamp,
        nullptr
    };

    hr = XalPlatformDateTimeSetCallbacks(&dateTime);
    if (FAILED(hr)) { return hr; }

    return S_OK;
}

HRESULT InitXalHooks(XTaskQueueHandle queue, char const* userSavegameRoot)
{
    HRESULT hr = InitXalBasicHooks();
    if (FAILED(hr)) { return hr; }

    // we want the storage event handlers to be invoked on the worker thread

	/*
    XTaskQueuePortHandle work = nullptr; // non owning
    hr = XTaskQueueGetPort(queue, XTaskQueuePort::Work, &work);
    if (FAILED(hr)) { return hr; }

    XTaskQueueHandle storageQueue = nullptr;
    hr = XTaskQueueCreateComposite(work, work, &storageQueue);
    if (FAILED(hr)) { return hr; }

    auto storageContext = std::make_unique<StorageContext>();
    storageContext->userSavegameRoot = userSavegameRoot;

    XalPlatformStorageEventHandlers storage
    {
        &Write,
        &Read,
        &Clear,
        storageContext.release()
    };

    hr = XalPlatformStorageSetEventHandlers(storageQueue, &storage);
    if (FAILED(hr)) { return hr; }

    // we intentionally leak storageQueue to keep the queue alive

    // the remote connect event handlers should run on the "main" thread

    XalPlatformRemoteConnectEventHandlers rceh =
    {
        &ShowPrompt,
        &ClosePropmpt,
        nullptr
    };

    XalPlatformRemoteConnectSetEventHandlers(queue, &rceh);
	*/

    return S_OK;
}