#include "xal_hooks.h"

#include <cassert>
#include <fstream>
#include <httpClient/pal.h> // trace.h is not self sufficient
#include <httpClient/trace.h>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <Xal/xal_platform.h>

#include <fios2.h>
#include <kernel.h>
#include <libsysmodule.h>
#include <rtc.h>
#include <sce_random.h>

HC_DEFINE_TRACE_AREA(XAL_HOOKS, HCTraceLevel::Important);

namespace
{

	XalPlatformOperationResult GenerateUuid(void* /*context*/, XalUuid* newUuid)
	{
		assert(newUuid);
		SceKernelUuid kernelUuid;
		int result = sceKernelUuidCreate(&kernelUuid);
		if (result < 0) { return XalPlatformOperationResult_Failure; }

		static_assert(sizeof(SceKernelUuid) == sizeof(XalUuid), "Incompatible uuid sizes");
		memcpy(newUuid, &kernelUuid, sizeof(XalUuid));

		return XalPlatformOperationResult_Success;
	}

	XalPlatformOperationResult GenerateRandomBytes(void* /*context*/, size_t bufferSize, uint8_t* buffer)
	{
		assert(buffer);
		int result = sceRandomGetRandomNumber(buffer, bufferSize);
		if (result < 0) { return XalPlatformOperationResult_Failure; }

		return XalPlatformOperationResult_Success;
	}

	XalPlatformOperationResult UtcTimestamp(void* /*context*/, int64_t* seconds, uint32_t* /*subsecond*/)
	{
		assert(seconds);
		time_t t = time(nullptr);
		if (t < 0) { return XalPlatformOperationResult_Failure; }

		*seconds = t;

		return XalPlatformOperationResult_Success;
	}

	XalPlatformOperationResult TimestampToComponents(
		void* /*context*/,
		int64_t secondsFromUnixEpoch,
		XalTimestampComponents* components
	)
	{
		assert(components);
		time_t t{ secondsFromUnixEpoch };
		SceRtcDateTime dateTime{};
		if (sceRtcSetTime_t(&dateTime, t) < 0) {
			return XalPlatformOperationResult_Failure;
		}

		components->year = dateTime.year;
		components->month = dateTime.month;
		components->day = dateTime.day;
		components->hour = dateTime.hour;
		components->minute = dateTime.minute;
		components->second = dateTime.second;

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

		SceRtcDateTime dateTime{};
		dateTime.year = components->year;
		dateTime.month = components->month;
		dateTime.day = components->day;
		dateTime.hour = components->hour;
		dateTime.minute = components->minute;
		dateTime.second = components->second;
		dateTime.microsecond = 0;

		time_t t = 0;
		if (sceRtcGetTime_t(&dateTime, &t) < 0) {
			return XalPlatformOperationResult_Failure;
		}

		*secondsFromUnixEpoch = t;
		return XalPlatformOperationResult_Success;
	}

}
HRESULT InitXalBasicHooks()
{
    HRESULT hr = S_OK;
    int sceResult;

    sceResult = sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM);
    if (sceResult < 0) {
        HC_TRACE_ERROR(XAL_HOOKS, "sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM) failed: 0x%08X", sceResult);
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