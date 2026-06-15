#if PLATFORM_PS4
#include "PCHOnlineDungeonsSubsystem.h"

#include <memory>
#include <vector>
#include <app_content.h>
#include <fios2.h>
#include <libsysmodule.h>

#include "OnlineIdentityDungeons.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystemDungeons.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"


#include "xal_platform.h"
#include "Xal/xal.h"
#include "xal_user.h"

#include "CoreDelegates.h"
#include "OnlineIdentityDungeonsPS4.h"
#include "trace.h"

#include "HrLog.h"
#include "utils.h"
#include "PS4/xal_hooks.h"
#include "PS4/hc_hooks.h"
#include "PS4/PS4File.h"

#include <libhttp2.h>
#include <libssl.h>

#include "PS4/PS4Application.h"

#define NET_HEAP_SIZE	(32 * 1024)
#define MAX_CONCURRENT_REQUEST	(4)
#define PRIVATE_CA_CERT_NUM		(0)
#define SSL_HEAP_SIZE	((((MAX_CONCURRENT_REQUEST-1) / 3) +1)*256*1024 + 4*1024*PRIVATE_CA_CERT_NUM)
#define HTTP2_HEAP_SIZE	((((MAX_CONCURRENT_REQUEST-1) / 3) +1)*256*1024)

HC_DECLARE_TRACE_AREA(XAL);
HC_DECLARE_TRACE_AREA(XAL_TELEMETRY);
HC_DECLARE_TRACE_AREA(HTTPCLIENT);
HC_DECLARE_TRACE_AREA(HC_HTTPLIB);
HC_DECLARE_TRACE_AREA(HC_WS);


FOnlineIdentityDungeonsPtr dungeonsIdentity::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineIdentityDungeonsPS4, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}


FOnlineIdentityDungeonsPS4::FOnlineIdentityDungeonsPS4(class FOnlineSubsystemDungeons* InSubsystem)
	:FOnlineIdentityDungeons(InSubsystem)
{
	InitializePlatform();
}

void FOnlineIdentityDungeonsPS4::XalShowPrompt(void* ctx, void* userCtx, XalPlatformOperation op, char const* url, char const* code)
{
	FOnlineIdentityDungeonsPS4* IdentityPtr = static_cast<FOnlineIdentityDungeonsPS4*>(ctx);
	IdentityPtr->PS4LinkOperation = op;

	UE_LOG_ONLINE(Verbose, TEXT("Remote Connect: SHOW"));
	UE_LOG_ONLINE(Verbose, TEXT("    context:      %p"), ctx);
	UE_LOG_ONLINE(Verbose, TEXT("    user context: %p"), userCtx);
	UE_LOG_ONLINE(Verbose, TEXT("    operation:    %p"), op);
	UE_LOG_ONLINE(Verbose, TEXT("    url:          %s"), ANSI_TO_TCHAR(url));
	UE_LOG_ONLINE(Verbose, TEXT("    code:         %s"), ANSI_TO_TCHAR(code));

	FCoreDelegates::OnMsaLinkingRequired.Broadcast(url, code);
}

void FOnlineIdentityDungeonsPS4::XalClosePrompt(void* ctx, void* userCtx, XalPlatformOperation op)
{
	FOnlineIdentityDungeonsPS4* IdentityPtr = static_cast<FOnlineIdentityDungeonsPS4*>(ctx);
	IdentityPtr->PS4LinkOperation = nullptr;

	UE_LOG_ONLINE(Verbose, TEXT("Remote Connect: HIDE"));
	UE_LOG_ONLINE(Verbose, TEXT("    context:      %p"), ctx);
	UE_LOG_ONLINE(Verbose, TEXT("    user context: %p"), userCtx);
	UE_LOG_ONLINE(Verbose, TEXT("    operation:    %p"), op);
}

struct StorageContext
{
	std::string userSavegameRoot;
};

class File
{
public:
	SceFiosFH Handle() const noexcept
	{
		return m_handle;
	}

	SceFiosFH* GetAddressOf() noexcept
	{
		return &m_handle;
	}

	bool isOpen() noexcept
	{
		return (SCE_FIOS_FH_INVALID != m_handle);
	}

	void Close() noexcept
	{
		if (isOpen())
		{
			sceFiosFHCloseSync(nullptr, m_handle);
			m_handle = SCE_FIOS_FH_INVALID;
		}
	}

	~File() noexcept
	{
		Close();
	}

private:
	SceFiosFH m_handle;
};

namespace
{
	std::string MakeFileName(std::string userSavegameRoot, char const* key)
	{
		std::string name = userSavegameRoot + "/" + std::string{ key }.substr(0, 40) + ".json";
		return name;
	}

	bool IsSuccess(int32_t result)
	{
		return result >= 0;
	}

	bool IsFailure(int32_t result)
	{
		return !IsSuccess(result);
	}

	bool Exists(const char* filePath)
	{
		return sceFiosFileExistsSync(nullptr, filePath);
	}

	bool CreateFolder(std::string path)
	{
		if (!sceFiosDirectoryExistsSync(nullptr, path.c_str()))
		{
			return IsSuccess(sceFiosDirectoryCreateSync(nullptr, path.c_str()));
		}
		return true;
	}

	int Open(
		SceFiosFH* fileHandle,
		const char* filePath,
		int flags
	)
	{
		SceFiosOpenParams openParams = SCE_FIOS_OPENPARAMS_INITIALIZER;
		openParams.openFlags = flags;
		*fileHandle = SCE_FIOS_FH_INVALID;
		int sceResult = sceFiosFHOpenSync(nullptr, fileHandle, filePath, &openParams);

		return sceResult;
	}

	TOptional<std::string> GetXalPath() {
		SceUserServiceUserId userId = 0;
		if (IsSuccess(sceUserServiceGetInitialUser(&userId))) {
			return "/download0/" + std::to_string(userId);
		}
		HC_TRACE_ERROR(XAL, "[Identity] Get XalPath: Failed to fetch XalPath");
		return {};
	}
}

void FOnlineIdentityDungeonsPS4::PS4XalWriteHandler(
	_In_opt_ void* context,
	_In_opt_ void* /*userContext*/,
	_In_ XalPlatformOperation operation,
	_In_z_ char const* key,
	_In_ size_t dataSize,
	_In_reads_bytes_(dataSize) void const* data
)
{
	auto mountpoint = GetXalPath();
	if (!mountpoint)
	{
		HrLog(XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure), "[Identity] XalPlatformStorageWriteComplete");
		return;
	}

	if (!CreateFolder(mountpoint.GetValue()))
	{
		HC_TRACE_ERROR(XAL, "Create folder: %s - failed to create folder", mountpoint.GetValue().c_str());
		HrLog(XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure), "[Identity] XalPlatformStorageWriteComplete");
		return;
	}

	std::string name = MakeFileName(mountpoint.GetValue(), key);
	bool resize = false;
	bool fileExists = Exists(name.c_str());
	if (fileExists)
	{
		// file already existed, we should resize it after opening
		resize = true;
	}

	File f{};
	int32_t res = Open(f.GetAddressOf(), name.c_str(), SCE_FIOS_O_CREAT | SCE_FIOS_O_WRITE);
	if (IsFailure(res))
	{
		HC_TRACE_ERROR(XAL, "filename: %s - write can't open: 0x%08X", name.c_str(), res);
		XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
		return;
	}

	if (resize)
	{
		res = sceFiosFHTruncateSync(nullptr, f.Handle(), static_cast<SceFiosSize>(dataSize));
		if (IsFailure(res))
		{
			HC_TRACE_ERROR(XAL, "filename: %s - write can't resize to %zu bytes: 0x%08X", name.c_str(), dataSize, res);
			XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
			return;
		}
	}

	res = sceFiosFHWriteSync(nullptr, f.Handle(), data, static_cast<SceFiosSize>(dataSize));
	if (IsFailure(res))
	{
		HC_TRACE_ERROR(XAL, "filename: %s - write can't write: 0x%08X", name.c_str(), res);
		XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
		return;
	}

	res = sceFiosFHSyncSync(nullptr, f.Handle()); // Flush.
	if (IsFailure(res))
	{
		HC_TRACE_ERROR(XAL, "filename: %s - write can't flush: 0x%08X", name.c_str(), res);
		XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
		return;
	}

	f.Close();

	HC_TRACE_INFORMATION(XAL, "filename: %s - write success %zu bytes", name.c_str(), dataSize);
	HC_TRACE_VERBOSE(XAL, ">>>\n%.*s\n<<<", dataSize, data);
	
	XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Success);
}

void FOnlineIdentityDungeonsPS4::PS4XalReadHandler(
	_In_opt_ void* context,
	_In_opt_ void* /*userContext*/,
	_In_ XalPlatformOperation operation,
	_In_z_ char const* key
)
{
	auto mountpoint = GetXalPath();
	if (!mountpoint)
	{
		HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr), "[Identity] XalPlatformStorageReadComplete");
		return;
	}

	std::string name = MakeFileName(mountpoint.GetValue(), key);
	
	File f{};
	int32_t res = Open(f.GetAddressOf(), name.c_str(), SCE_FIOS_O_READ);
	if (IsFailure(res))
	{
		HC_TRACE_INFORMATION(XAL, "filename: %s - read can't open: 0x%08X", name.c_str(), res);
		XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, 0, nullptr);
		return;
	}

	SceFiosSize numBytesReadOrErrorCode = 0;
	numBytesReadOrErrorCode = sceFiosFHGetSize(f.Handle());
	if (numBytesReadOrErrorCode < 0)
	{
		HC_TRACE_ERROR(XAL, "filename: %s - read can't get size: 0x%08X", name.c_str(), numBytesReadOrErrorCode);
		XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr);
		return;
	}

	std::vector<uint8_t> data(static_cast<size_t>(numBytesReadOrErrorCode));
	numBytesReadOrErrorCode = sceFiosFHReadSync(nullptr, f.Handle(), data.data(), numBytesReadOrErrorCode);
	if (numBytesReadOrErrorCode < 0)
	{
		HC_TRACE_ERROR(XAL, "filename: %s - read can't read: 0x%08X", name.c_str(), numBytesReadOrErrorCode);
		XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr);
		return;
	}

	HC_TRACE_INFORMATION(XAL, "filename: %s - read success %zu bytes", name.c_str(), data.size());
	HC_TRACE_VERBOSE(XAL, ">>>\n%.*s\n<<<", data.size(), data.data());

	XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, data.size(), data.data());
	
}

void FOnlineIdentityDungeonsPS4::PS4XalClearHandler(
	_In_opt_ void* context,
	_In_opt_ void* /*userContext*/,
	_In_ XalPlatformOperation operation,
	_In_z_ char const* key
)
{
	auto mountpoint = GetXalPath();
	if (!mountpoint)
	{
		HrLog(XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Failure), "[Identity] XalPlatformStorageClearComplete");
		return;
	}

	std::string name = MakeFileName(mountpoint.GetValue(), key);

	int32_t res = sceFiosDeleteSync(nullptr, name.c_str());
	if (IsFailure(res))
	{
		HC_TRACE_ERROR(XAL, "filename: %s - clear can't delete: 0x%08X", name.c_str(), res);
		XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Failure);
		return;
	}

	HC_TRACE_INFORMATION(XAL, "filename: %s - clear success", name.c_str());
	
	XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success);
}


const char* FOnlineIdentityDungeonsPS4::GetTemporaryMountPoint() {
	return mMountPoint.c_str();
}

void FOnlineIdentityDungeonsPS4::OnPS4LinkCancelled()
{
	// D11.AH - Check to see if we have a valid operation going on
	if (PS4LinkOperation)
	{
		UE_LOG_ONLINE(Log, TEXT("[Identity] PS4 Link Cancelled"));
		HrLog(XalPlatformRemoteConnectCancelPrompt(PS4LinkOperation), "[Identity] PS4 Link Cancelled");
		PS4LinkOperation = nullptr;
	}
}


void FOnlineIdentityDungeonsPS4::InitializeNetworking()
{
	SceInt32 ret = sceNetInit();
	assert(ret >= 0);
	(void)ret;

	mLibnetMemId = sceNetPoolCreate("simple", NET_HEAP_SIZE, 0);
	assert(mLibnetMemId >= 0);

	mLibsslCtxId = sceSslInit(SSL_HEAP_SIZE);
	assert(mLibsslCtxId > 0);

	mLibhttp2CtxId = sceHttp2Init(mLibnetMemId, mLibsslCtxId, HTTP2_HEAP_SIZE, MAX_CONCURRENT_REQUEST);
	assert(mLibhttp2CtxId > 0);

	mDnsResolver = sceNetResolverCreate("resolver", mLibnetMemId, 0);
	assert(mDnsResolver > 0);
}

void FOnlineIdentityDungeonsPS4::FinalizeNetworking()
{
	int sceResult = sceNetResolverDestroy(mDnsResolver);
	assert(sceResult >= 0);

	sceResult = sceHttp2Term(mLibhttp2CtxId);
	assert(sceResult >= 0);

	sceResult = sceSslTerm(mLibsslCtxId);
	assert(sceResult >= 0);

	sceResult = sceNetPoolDestroy(mLibnetMemId);
	assert(sceResult >= 0);

	sceResult = sceNetTerm();
	assert(sceResult >= 0);
}


void FOnlineIdentityDungeonsPS4::InitializePlatform()
{
//	InitializeFilesystem();
	InitializeNetworking();

	PS4LinkCancelled = FCoreDelegates::OnMsaLinkingCancelled.AddRaw(this, &FOnlineIdentityDungeonsPS4::OnPS4LinkCancelled);
	MsaCodeReceivedHandle = FCoreDelegates::OnMsaLinkingRequired.AddRaw(this, &FOnlineIdentityDungeonsPS4::OnMsaCodeReceived);

	XalInitArgs xalInitArgs = {};
	XblInitArgs xblInitArgs = {};

	HCTraceSetTraceToDebugger(!environmentConfig::IsShippingMode());

	const HCTraceLevel logLevel = environmentConfig::IsShippingMode() ? HCTraceLevel::Off : HCTraceLevel::Warning;

	HC_TRACE_SET_VERBOSITY(XAL, logLevel);
	HC_TRACE_SET_VERBOSITY(XAL_TELEMETRY, logLevel);
	HC_TRACE_SET_VERBOSITY(HTTPCLIENT, logLevel);
	HC_TRACE_SET_VERBOSITY(HC_HTTPLIB, logLevel);
	HC_TRACE_SET_VERBOSITY(HC_WS, logLevel);

	HrCheck(InitHttpClientHooks(&mWebsocket, GetLibhttp2CtxId(), GetLibsslCtxId(), GetDnsResolver()), "[Identity] InitHttpClientHooks");

	const auto queueHandle = DungeonsSubsystem->GetQueueHandle();

	mMountPoint = TCHAR_TO_ANSI (*FPS4PlatformFile::GetTempDirectory());

	InitXalHooks(queueHandle, GetTemporaryMountPoint());

	xalInitArgs.clientId = OnlineSubsystemConstants::CLIENTID;
	xalInitArgs.titleId = OnlineSubsystemConstants::TITLEID;
	xalInitArgs.sandbox = environmentConfig::getSandbox();

	// D11.AH - TODO - This might work but need to check if it has to be unique every time
	//                 because I think it incorporates timestamp.
	SceKernelUuid deviceId;
	sceKernelUuidCreate(&deviceId);

	char deviceIdString[sizeof(SceKernelUuid)+1] = {};
	memset(&deviceIdString, 0, sizeof(deviceIdString));
	memcpy(&deviceIdString, &deviceId, sizeof(SceKernelUuid));
	// D11.AH - END

	xalInitArgs.maxSignedInUsers = 1;
	xalInitArgs.useRemoteAuth = true;
	xalInitArgs.deviceInfo.deviceType = "PlayStation";
	xalInitArgs.deviceInfo.deviceId = deviceIdString;
	xalInitArgs.deviceInfo.osVersion = ""; // currently unavailable on Orbis
	xalInitArgs.telemetryInfo.appId = "Dungeons";
	xalInitArgs.telemetryInfo.appVer = "0.0";
	xalInitArgs.telemetryInfo.osName = "PlayStation4";
	xalInitArgs.telemetryInfo.osVersion = xalInitArgs.deviceInfo.osVersion;
	xalInitArgs.telemetryInfo.osLocale = ""; // currently unavailable on PS4
	xalInitArgs.telemetryInfo.deviceClass = xalInitArgs.deviceInfo.deviceType;
	xalInitArgs.telemetryInfo.deviceId = xalInitArgs.deviceInfo.deviceId;
	xalInitArgs.flags = 1u << 31;	// No idea what this is for sample does it

	XalPlatformStorageEventHandlers storage
	{
		&PS4XalWriteHandler,
		&PS4XalReadHandler,
		&PS4XalClearHandler,
		this
	};

	HrCheck(XalPlatformStorageSetEventHandlers(queueHandle, &storage), "[Identity] XalPlatformStorageSetEventHandlers");

	XalPlatformRemoteConnectEventHandlers remoteConnectEventHandlers =
	{
		&XalShowPrompt,
		&XalClosePrompt,
		this
	};

	HrCheck(XalPlatformRemoteConnectSetEventHandlers(queueHandle, &remoteConnectEventHandlers), "[Identity] XalPlatformRemoteConnectSetEventHandlers");

	HrCheck(XalInitialize(&xalInitArgs, queueHandle), "[Identity] XalInitialize");

//	XblLocalStorageSetHandlers(queueHandle,
//		&SwitchXblWriteHandler,
//		&SwitchXblReadHandler,
//		&SwitchXblClearHandler,
//		this);

	xblInitArgs.queue = queueHandle;
	xblInitArgs.appId = xalInitArgs.telemetryInfo.appId;
	xblInitArgs.appVer = xalInitArgs.telemetryInfo.appVer;
	xblInitArgs.osName = xalInitArgs.telemetryInfo.osName;
	xblInitArgs.osVersion = xalInitArgs.telemetryInfo.osVersion;
	xblInitArgs.osLocale = xalInitArgs.telemetryInfo.osLocale;
	xblInitArgs.deviceClass = xalInitArgs.telemetryInfo.deviceClass;
	xblInitArgs.deviceId = xalInitArgs.telemetryInfo.deviceId;
	xblInitArgs.scid = OnlineSubsystemConstants::SCID;
	HrCheck(XblInitialize(&xblInitArgs), "[Identity] XblInitialize");

	AsyncInitialized = true;

	UE_LOG_ONLINE(Log, TEXT("[Identity] XAL Initialized"));
	UE_LOG_ONLINE(Log, TEXT("[Identity] ClientId='%s'"), ANSI_TO_TCHAR(xalInitArgs.clientId));
	UE_LOG_ONLINE(Log, TEXT("[Identity] TitleId='%ud'"), xalInitArgs.titleId);
	UE_LOG_ONLINE(Log, TEXT("[Identity] SandboxId='%s'"), ANSI_TO_TCHAR(xalInitArgs.sandbox));
	UE_LOG_ONLINE(Log, TEXT("[Identity] SCID='%s'"), ANSI_TO_TCHAR(xblInitArgs.scid));
}

void FOnlineIdentityDungeonsPS4::PlatformTick(float DeltaTime)
{
	HcTickTimers();
	UpdateWebSocketHooks(mWebsocket);
}

#endif
