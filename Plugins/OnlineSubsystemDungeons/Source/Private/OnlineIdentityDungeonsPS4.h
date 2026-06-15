#if PLATFORM_PS4

#include "OnlineIdentityDungeons.h"

class FOnlineIdentityDungeonsPS4 : public FOnlineIdentityDungeons
{
public:

	FOnlineIdentityDungeonsPS4(class FOnlineSubsystemDungeons* InSubsystem);
	virtual ~FOnlineIdentityDungeonsPS4() = default;

	void InitializePlatform();
	void PlatformTick(float DeltaTime) override;
	const char* GetTemporaryMountPoint();

private:
	void InitializeFilesystem();
	void FinalizeFilesystem();

	static void XalShowPrompt(void* ctx, void* userCtx, XalPlatformOperation op, char const* url, char const* code);
	static void XalClosePrompt(void* ctx, void* userCtx, XalPlatformOperation op);

	static void PS4XblWriteHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, XblLocalStorageWriteMode mode, char const* key, size_t dataSize, void const* data);
	static void PS4XblReadHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key);
	static void PS4XblClearHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key);
	static void PS4XalWriteHandler(void* context, void*, XalPlatformOperation operation, char const* key, size_t dataSize, void const* data);
	static void PS4XalReadHandler(void* context, void*, XalPlatformOperation operation, char const* key);
	static void PS4XalClearHandler(void* context, void*, XalPlatformOperation operation, char const* key);

	bool FormatTemporaryData();

	void OnPS4LinkCancelled();

	void InitializeNetworking();
	void FinalizeNetworking();

	int GetLibsslCtxId() { return mLibsslCtxId; }
	int GetLibhttp2CtxId() { return mLibhttp2CtxId; }
	int GetDnsResolver() { return mDnsResolver; }

	FDelegateHandle PS4LinkCancelled;
	XalPlatformOperation PS4LinkOperation;
	WebSocket::Context * mWebsocket;

	int mLibnetMemId = 0;
	int mLibsslCtxId = 0;
	int mLibhttp2CtxId = 0;
	int mDnsResolver = 0;
	std::string mMountPoint;
};

typedef TSharedPtr<FOnlineIdentityDungeonsPS4, ESPMode::ThreadSafe> FOnlineIdentityDungeonsPS4Ptr;
#endif