// gdkdungeons.cpp
#include "PCHGDK.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FGDKDungeonsInterface, GDK)

#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
#include "GDKDefs.h"
#include <vector>

struct GDKContext
{
	std::function<void(GDKAsyncBlock*)> Callback;
};

GDKAsyncBlock* CreateAsyncBlock(std::function<void(GDKAsyncBlock*)> callback, GDKTaskQueueHandle taskQueue)
{
	return new GDKAsyncBlock{
		taskQueue,
		new GDKContext{std::move(callback)},
		[](GDKAsyncBlock* asyncBlock) {
			ensureMsgf(asyncBlock->context, TEXT("The asyncBlock context is null, this should never happen."));
			GDKContext* ctx = static_cast<GDKContext*>(asyncBlock->context);
			ctx->Callback(asyncBlock);
			delete ctx;
			delete asyncBlock;
		}
	};
}

HRESULT FGDKDungeonsInterface::InitializeGDK() {
	if (isInitialized) {
		return S_OK;
	}

	HRESULT hr = XGameRuntimeInitialize();
	checkf(SUCCEEDED(hr), TEXT("XGameRuntimeInitialize failed with hr 0x%08lx"), hr);
	if (SUCCEEDED(hr)) {
		hr = GDKTaskQueueCreate(GDKTaskQueueDispatchMode::ThreadPool, GDKTaskQueueDispatchMode::Manual, &queueHandle);
		checkf(SUCCEEDED(hr), TEXT("XTaskQueueCreate failed with hr 0x%08lx"), hr);
		isInitialized = SUCCEEDED(hr);
	}

	return hr;
}

HRESULT FGDKDungeonsInterface::UninitializeGDK() {
	if (!isInitialized) {
		return S_OK;
	}

	HRESULT hr = XGameRuntimeUninitialize();
	checkf(SUCCEEDED(hr), TEXT("XGameRuntimeInitialize failed with hr 0x%08lx"), hr);
	if (SUCCEEDED(hr)) {
		isInitialized = false;
	}

	return hr;
}


HRESULT FGDKDungeonsInterface::RegisterForInvites(
	void *context,
	XGameInviteEventCallback *callback,
	GDKTaskQueueRegistrationToken* token) {
	return XGameInviteRegisterForEvent(queueHandle, context, callback, token);
}

bool FGDKDungeonsInterface::UnregisterForInvites(GDKTaskQueueRegistrationToken token, bool wait) {
	return XGameInviteUnregisterForEvent(token, wait);
}

HRESULT FGDKDungeonsInterface::CreateStoreContext() {
	return XStoreCreateContext(nullptr, &storeContext);
}

HRESULT FGDKDungeonsInterface::QueryLicenseTokenAsync(
	const char** productIds,
	size_t productIdsCount,
	const char* customDeveloperString,
	std::function<void(std::string)> tokenCallback) {

	return XStoreQueryLicenseTokenAsync(
		storeContext,
		productIds,
		productIdsCount,
		customDeveloperString,
		CreateAsyncBlock([tokenCallback](GDKAsyncBlock* asyncBlock) {
			size_t tokenSize = 0;
			HRESULT hr = XStoreQueryLicenseTokenResultSize(asyncBlock, &tokenSize);
			if (SUCCEEDED(hr)) {
				std::vector<char> token(tokenSize, '\0');
				hr = XStoreQueryLicenseTokenResult(asyncBlock, tokenSize, token.data());
				if (SUCCEEDED(hr)) {
					tokenCallback(std::string(token.begin(), token.end()));
				}
			}

			if (FAILED(hr)) {
				tokenCallback(std::string()); // callback with empty string indicating failure
			}
		}, queueHandle)
	);
}

bool FGDKDungeonsInterface::Tick(float DeltaTime) {
	if (queueHandle != nullptr) {
		GDKTaskQueueDispatch(queueHandle, GDKTaskQueuePort::Completion, 0);
	}
	return true;
}

#endif
