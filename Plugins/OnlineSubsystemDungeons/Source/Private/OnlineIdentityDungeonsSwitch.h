#if PLATFORM_SWITCH
#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"

class FOnlineIdentityDungeonsSwitch : public FOnlineIdentityDungeons
{
public:
	FOnlineIdentityDungeonsSwitch(class FOnlineSubsystemDungeons* InSubsystem);
	virtual ~FOnlineIdentityDungeonsSwitch();

	virtual void PlatformTick(float DeltaTime) override;

private:
	void InitializePlatform();

	static void XalShowPrompt(void* ctx, void* userCtx, XalPlatformOperation op, char const* url, char const* code);
	static void XalClosePrompt(void* ctx, void* userCtx, XalPlatformOperation op);

	static void* WebSocketAllocateFunction(size_t size);
	static void WebSocketFreeFunction(void* ptr);

	static void SwitchXblWriteHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, XblLocalStorageWriteMode mode, char const* key, size_t dataSize, void const* data);
	static void SwitchXblReadHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key);
	static void SwitchXblClearHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key);
	static void SwitchXalWriteHandler(void* context, void*, XalPlatformOperation operation, char const* key, size_t dataSize, void const* data);
	static void SwitchXalReadHandler(void* context, void*, XalPlatformOperation operation, char const* key);
	static void SwitchXalClearHandler(void* context, void*, XalPlatformOperation operation, char const* key);

	static std::string MakeFileName(char const* key);

	void OnSwitchLinkCancelled();

	FDelegateHandle SwitchLinkCancelled;
	XalPlatformOperation SwitchLinkOperation;
	class FOnlineIdentityDungeonsRunnableSwitch* mTickRunnable;
};
typedef TSharedPtr<FOnlineIdentityDungeonsSwitch, ESPMode::ThreadSafe> FOnlineIdentityDungeonsSwitchPtr;

#endif

