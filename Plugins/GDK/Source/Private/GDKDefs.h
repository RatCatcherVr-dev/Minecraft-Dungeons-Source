#pragma once
#include "GDKInterfaceDefs.h"

enum class GDKTaskQueueDispatchMode : uint32_t
{
	Manual,
	ThreadPool,
	SerializedThreadPool,
	Immediate
};

enum class GDKTaskQueuePort : uint32_t
{
	Work,
	Completion
};

extern "C" {

HRESULT __stdcall XGameRuntimeInitialize();

HRESULT __stdcall XGameRuntimeUninitialize();

HRESULT __stdcall GDKTaskQueueCreate(
	GDKTaskQueueDispatchMode workDispatchMode,
	GDKTaskQueueDispatchMode completionDispatchMode,
	GDKTaskQueueHandle* queue
);

bool __stdcall GDKTaskQueueDispatch(
	GDKTaskQueueHandle queue,
	GDKTaskQueuePort port,
	uint32_t timeoutInMs
);

HRESULT __stdcall XGameInviteRegisterForEvent(
	GDKTaskQueueHandle queue,
	void* context,
	XGameInviteEventCallback* callback,
	GDKTaskQueueRegistrationToken* token
);

bool __stdcall XGameInviteUnregisterForEvent(
	GDKTaskQueueRegistrationToken token,
	bool wait
);

HRESULT __stdcall XStoreCreateContext(
	const void* user,
	XStoreContextHandle* storeContextHandle
);

HRESULT XStoreQueryLicenseTokenAsync(
	const XStoreContextHandle storeContextHandle,
	const char** productIds,
	size_t productIdsCount,
	const char* customDeveloperString,
	GDKAsyncBlock* async
);

HRESULT XStoreQueryLicenseTokenResult(
	GDKAsyncBlock* async,
	size_t size,
	char* result
);

HRESULT XStoreQueryLicenseTokenResultSize(
	GDKAsyncBlock* async,
	size_t* size
);

}