#if PLATFORM_SWITCH
#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineIdentityDungeons.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystemDungeons.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"

#include <memory>
#include <vector>
#include "utils.h"

#include "xal_platform.h"
#include "Xal/xal.h"
#include "xal_user.h"

#include <nn/account.h>
#include <nn/account/account_Result.h>
#include <nn/fs.h>
#include <nn/hid/hid_Npad.h>
#include <nn/nn_Log.h>
#include <nn/oe.h>
#include <nn/ssl.h>
#include <nn/websocket.h>

#include "Switch/hc_hooks.h"
#include "Switch/xsapi_switch_platform.h"
#include "Switch/xal_hooks.h"

#include "CoreDelegates.h"
#include "OnlineIdentityDungeonsSwitch.h"
#include "trace.h"

#include "HrLog.h"

HC_DECLARE_TRACE_AREA(XAL);
HC_DECLARE_TRACE_AREA(XAL_TELEMETRY);
HC_DECLARE_TRACE_AREA(HTTPCLIENT);
HC_DECLARE_TRACE_AREA(HC_CURL);
HC_DECLARE_TRACE_AREA(HC_WS);


class FOnlineIdentityDungeonsRunnableSwitch : public FRunnable
{
public:

	FOnlineIdentityDungeonsRunnableSwitch()
	:
	mWebsocket(nullptr),
	mTickThread(nullptr)
	{
		HrCheck(InitHttpClientHooks(&mWebsocket), "[Identity] InitHttpClientHooks");
	}

	~FOnlineIdentityDungeonsRunnableSwitch()
	{
		FPlatformMisc::MemoryBarrier();

		if (mTickThread)
		{
			mTickThread->Kill(true);
			delete mTickThread;
			mTickThread = nullptr;
		}
	}

	virtual uint32 Run() override
	{
		while (!GIsRequestingExit)
		{
			HcTickTimers();
			UpdateWebSocketHooks(mWebsocket);
			FPlatformProcess::Sleep(1.0f / 30.0f); //Sleep thread for 33ms
		}
		return 0;
	}

	virtual void Stop() override
	{
	}
	
	void StartRunnable()
	{
		mTickThread = FRunnableThread::Create(this, TEXT("OnlineIdentityDungeonsRunnableSwitch"), 0, EThreadPriority::TPri_BelowNormal);
		checkf(mTickThread, TEXT("Failed to create OnlineIdentityDungeonsRunnableSwitch thread"));
	}

protected:	

	WebSocket::Context * mWebsocket;
	class FRunnableThread* mTickThread;
};



void FOnlineIdentityDungeonsSwitch::XalShowPrompt(void* ctx, void* userCtx, XalPlatformOperation op, char const* url, char const* code)
{
	FOnlineIdentityDungeonsSwitch* IdentityPtr = static_cast<FOnlineIdentityDungeonsSwitch*>(ctx);
	IdentityPtr->SwitchLinkOperation = op;

	UE_LOG_ONLINE(Verbose, TEXT("Remote Connect: SHOW"));
	UE_LOG_ONLINE(Verbose, TEXT("    context:      %p"), ctx);
	UE_LOG_ONLINE(Verbose, TEXT("    user context: %p"), userCtx);
	UE_LOG_ONLINE(Verbose, TEXT("    operation:    %p"), op);
	UE_LOG_ONLINE(Verbose, TEXT("    url:          %s"), ANSI_TO_TCHAR(url));
	UE_LOG_ONLINE(Verbose, TEXT("    code:         %s"), ANSI_TO_TCHAR(code));

	FCoreDelegates::OnMsaLinkingRequired.Broadcast(url, code);
}

void FOnlineIdentityDungeonsSwitch::XalClosePrompt(void* ctx, void* userCtx, XalPlatformOperation op)
{
	FOnlineIdentityDungeonsSwitch* IdentityPtr = static_cast<FOnlineIdentityDungeonsSwitch*>(ctx);
	IdentityPtr->SwitchLinkOperation = nullptr;

	UE_LOG_ONLINE(Verbose, TEXT("Remote Connect: HIDE"));
	UE_LOG_ONLINE(Verbose, TEXT("    context:      %p"), ctx);
	UE_LOG_ONLINE(Verbose, TEXT("    user context: %p"), userCtx);
	UE_LOG_ONLINE(Verbose, TEXT("    operation:    %p"), op);
}

std::string FOnlineIdentityDungeonsSwitch::MakeFileName(char const* key)
{
	return std::string{ key }.substr(0, 40) + ".json";
}

void FOnlineIdentityDungeonsSwitch::SwitchXblWriteHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, XblLocalStorageWriteMode mode,
	char const* key, size_t dataSize, void const* data)
{
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem)
	{
		HrLog(XblLocalStorageWriteComplete(operation, XblClientOperationResult::Failure, 0), "[Identity] XblLocalStorageWriteComplete - XblClientOperationResult::Failure");
		return;
	}

	auto name = ANSI_TO_TCHAR(key);

	TArray<uint8> writeBlob;
	if (mode == XblLocalStorageWriteMode::Append)
	{
		saveSystem->LoadGame(false, name, 0, writeBlob);
	}
	writeBlob.Append((uint8*)data, dataSize);
	
	if (!saveSystem->SaveGame(false, name, 0, writeBlob))
	{
		HrLog(XblLocalStorageWriteComplete(operation, XblClientOperationResult::Failure, 0), "[Identity] XblLocalStorageWriteComplete - XblClientOperationResult::Failure");
		return;
	}
	HrLog(XblLocalStorageWriteComplete(operation, XblClientOperationResult::Success, writeBlob.Num()), "[Identity] XblLocalStorageWriteComplete - XblClientOperationResult::Success");
}

void FOnlineIdentityDungeonsSwitch::SwitchXblReadHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key)
{
	std::string name = key;
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem->DoesSaveGameExist(ANSI_TO_TCHAR(name.c_str()), 0))
	{
		// Return Success anyway, might not have been created
		HrLog(XblLocalStorageReadComplete(operation, XblClientOperationResult::Success, 0, nullptr), "[Identity] XblLocalStorageReadComplete - XblClientOperationResult::Success");
		return;
	}
	TArray<uint8> readBlob;
	if (!saveSystem->LoadGame(false, ANSI_TO_TCHAR(name.c_str()), 0, readBlob))
	{
		HrLog(XblLocalStorageReadComplete(operation, XblClientOperationResult::Failure, 0, nullptr), "[Identity] XblLocalStorageReadComplete - XblClientOperationResult::Failure");
		return;
	}
	HrLog(XblLocalStorageReadComplete(operation, XblClientOperationResult::Success, 0, nullptr), "[Identity] XblLocalStorageReadComplete - XblClientOperationResult::Success");
}

void FOnlineIdentityDungeonsSwitch::SwitchXblClearHandler(void* context, XblClientOperationHandle operation, XblUserHandle user, const char* key)
{
	std::string name = key;
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem->DoesSaveGameExist(ANSI_TO_TCHAR(name.c_str()), 0))
	{
		// Return Success anyway, might not have been created
		HrLog(XblLocalStorageClearComplete(operation, XblClientOperationResult::Success), "[Identity] XblLocalStorageClearComplete - XblClientOperationResult::Success");
		return;
	}
	if (!saveSystem->DeleteGame(false, ANSI_TO_TCHAR(name.c_str()), 0))
	{
		HrLog(XblLocalStorageClearComplete(operation, XblClientOperationResult::Failure), "[Identity] XblLocalStorageClearComplete - XblClientOperationResult::Failure");
		return;
	}
	HrLog(XblLocalStorageClearComplete(operation, XblClientOperationResult::Success), "[Identity] XblLocalStorageClearComplete - XblClientOperationResult::Success");
}

void FOnlineIdentityDungeonsSwitch::SwitchXalWriteHandler(void* context, void*, XalPlatformOperation operation, char const* key,
	size_t dataSize, void const* data)
{
	std::string name = MakeFileName(key);
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	TArray<uint8> writeBlob;
	writeBlob.Append((uint8*)data, dataSize);

	if (!saveSystem->SaveGame(false, ANSI_TO_TCHAR(name.c_str()), 0, writeBlob))
	{
		HrLog(XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure), "[Identity] XalPlatformStorageWriteComplete(XalPlatformOperationResult_Failure)");
		return;
	}
	HrLog(XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Success), "[Identity] XalPlatformStorageWriteComplete(XalPlatformOperationResult_Success)");
}

void FOnlineIdentityDungeonsSwitch::SwitchXalReadHandler(void* context, void*, XalPlatformOperation operation, char const* key)
{
	std::string name = MakeFileName(key);
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem->DoesSaveGameExist(ANSI_TO_TCHAR(name.c_str()), 0))
	{
		// Return Success anyway, might not have been created
		HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, 0, nullptr), "[Identity] XalPlatformStorageReadComplete - XalPlatformOperationResult_Success");
		return;
	}
	TArray<uint8> readBlob;
	if (!saveSystem->LoadGame(false, ANSI_TO_TCHAR(name.c_str()), 0, readBlob))
	{
		HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr), "[Identity] XalPlatformStorageReadComplete - XalPlatformOperationResult_Failure");
		return;
	}
	HrLog(XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, readBlob.Num(), readBlob.GetData()), "[Identity] XalPlatformStorageReadComplete - XalPlatformOperationResult_Success");
}

void FOnlineIdentityDungeonsSwitch::SwitchXalClearHandler(void* context, void*, XalPlatformOperation operation, char const* key)
{
	std::string name = MakeFileName(key);
	ISaveGameSystem* saveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!saveSystem->DoesSaveGameExist(ANSI_TO_TCHAR(name.c_str()), 0))
	{
		// Return Success anyway, might not have been created
		HrLog(XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success), "[Identity] XalPlatformStorageClearComplete - XalPlatformOperationResult_Success");
		return;
	}
	if (!saveSystem->DeleteGame(false, ANSI_TO_TCHAR(name.c_str()), 0))
	{
		HrLog(XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Failure), "[Identity] XalPlatformStorageClearComplete - XalPlatformOperationResult_Failure");
		return;
	}
	HrLog(XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success), "[Identity] XalPlatformStorageClearComplete - XalPlatformOperationResult_Success");
}

void* FOnlineIdentityDungeonsSwitch::WebSocketAllocateFunction(size_t size) 
{
	return FMemory::Malloc(size);
}

void FOnlineIdentityDungeonsSwitch::WebSocketFreeFunction(void* ptr)
{
	FMemory::Free(ptr);
}

FOnlineIdentityDungeonsPtr dungeonsIdentity::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineIdentityDungeonsSwitch, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}


FOnlineIdentityDungeonsSwitch::FOnlineIdentityDungeonsSwitch(class FOnlineSubsystemDungeons* InSubsystem)
	: FOnlineIdentityDungeons(InSubsystem)
	, mTickRunnable(nullptr)
{
	InitializePlatform();
}

FOnlineIdentityDungeonsSwitch::~FOnlineIdentityDungeonsSwitch()
{
	if(mTickRunnable)
	{
		delete mTickRunnable;
		mTickRunnable = nullptr;
	}

}

void FOnlineIdentityDungeonsSwitch::OnSwitchLinkCancelled()
{
	// D11.AH - Check to see if we have a valid operation going on
	if (SwitchLinkOperation)
	{
		UE_LOG_ONLINE(Log, TEXT("[Identity] Switch Link Cancelled"));
		HrLog(XalPlatformRemoteConnectCancelPrompt(SwitchLinkOperation), "[Identity] Switch Link Cancelled");
		SwitchLinkOperation = nullptr;
	}
}

void FOnlineIdentityDungeonsSwitch::InitializePlatform()
{
	SwitchLinkCancelled = FCoreDelegates::OnMsaLinkingCancelled.AddRaw(this, &FOnlineIdentityDungeonsSwitch::OnSwitchLinkCancelled);
	MsaCodeReceivedHandle = FCoreDelegates::OnMsaLinkingRequired.AddRaw(this, &FOnlineIdentityDungeonsSwitch::OnMsaCodeReceived);

	nn::websocket::SetAllocator(WebSocketAllocateFunction, WebSocketFreeFunction);

	XalInitArgs xalInitArgs = {};
	XblInitArgs xblInitArgs = {};

	HCTraceSetTraceToDebugger(!environmentConfig::IsShippingMode());

	const HCTraceLevel logLevel = environmentConfig::IsShippingMode() ? HCTraceLevel::Off : HCTraceLevel::Warning;
	HC_TRACE_SET_VERBOSITY(XAL, logLevel);
	HC_TRACE_SET_VERBOSITY(XAL_TELEMETRY, logLevel);
	HC_TRACE_SET_VERBOSITY(HTTPCLIENT, logLevel);
	HC_TRACE_SET_VERBOSITY(HC_CURL, logLevel);
	HC_TRACE_SET_VERBOSITY(HC_WS, logLevel);
	
	mTickRunnable = new FOnlineIdentityDungeonsRunnableSwitch();

	const auto queueHandle = DungeonsSubsystem->GetQueueHandle();

	InitXalHooks(queueHandle, OnlineSubsystemConstants::USER_SAVEGAME_ROOT);
	
	xalInitArgs.clientId = OnlineSubsystemConstants::CLIENTID;
	xalInitArgs.titleId = OnlineSubsystemConstants::TITLEID;
	xalInitArgs.sandbox = environmentConfig::getSandbox();
	nn::util::Uuid deviceId{};
	nn::oe::GetPseudoDeviceId(&deviceId);

	char deviceIdString[nn::util::Uuid::StringSize] = {};
	deviceId.ToString(deviceIdString, nn::util::Uuid::StringSize);
	//
	//	XalPlatformArgs pargs{};
	xalInitArgs.maxSignedInUsers = 1;
	xalInitArgs.useRemoteAuth = true;
	xalInitArgs.deviceInfo.deviceType = "Nintendo";
	xalInitArgs.deviceInfo.deviceId = deviceIdString;
	xalInitArgs.deviceInfo.osVersion = ""; // currently unavailable on Switch
	xalInitArgs.telemetryInfo.appId = "Dungeons";
	xalInitArgs.telemetryInfo.appVer = "0.0";
	xalInitArgs.telemetryInfo.osName = "Nintendo";
	xalInitArgs.telemetryInfo.osVersion = xalInitArgs.deviceInfo.osVersion;
	xalInitArgs.telemetryInfo.osLocale = ""; // currently unavailable on Switch
	xalInitArgs.telemetryInfo.deviceClass = xalInitArgs.deviceInfo.deviceType;
	xalInitArgs.telemetryInfo.deviceId = xalInitArgs.deviceInfo.deviceId;
	xalInitArgs.flags = 1u << 31;	// No idea what this is for sample does it

	XalPlatformStorageEventHandlers storage
	{
		&SwitchXalWriteHandler,
		&SwitchXalReadHandler,
		&SwitchXalClearHandler,
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

	XblLocalStorageSetHandlers(queueHandle,
		&SwitchXblWriteHandler,
		&SwitchXblReadHandler,
		&SwitchXblClearHandler,
		this);

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

	if(mTickRunnable)
		mTickRunnable->StartRunnable();

}



void FOnlineIdentityDungeonsSwitch::PlatformTick(float DeltaTime)
{
	//Dont put blocking stuff in here, its called form the main thread	
}

#endif