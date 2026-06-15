#if PLATFORM_WINDOWS
#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineExternalUIInterfaceDungeons.h"
#include <memory>
#include <vector>
#include "utils.h"
#include "xal_platform.h"
#include "Xal/xal.h"
#include "xal_user.h"
#include <XalExtra/Win32/file_storage.h>
#include "OnlineIdentityDungeonsWindows.h"
#include "HrLog.h"


HC_DEFINE_TRACE_AREA(DUNGEONS, HCTraceLevel::Verbose);
HC_DECLARE_TRACE_AREA(XAL);
HC_DECLARE_TRACE_AREA(XAL_TELEMETRY);
HC_DECLARE_TRACE_AREA(HTTPCLIENT);
HC_DECLARE_TRACE_AREA(HC_CURL);
HC_DECLARE_TRACE_AREA(HC_WS);

namespace dungeonsIdentity {
auto CreateUrlChangedLambda(const FString& finalUrl, XalPlatformOperation operation, FOnlineExternalUIDungeonsPtr context)
{
	ensureMsgf(!finalUrl.IsEmpty(), TEXT("Final url is empty."));
	return [finalUrl, operation, context](const FString& newURL) {
		if (newURL.StartsWith(finalUrl)) {
			HrLog(XalPlatformWebShowUrlComplete(operation, XalPlatformOperationResult_Success, TCHAR_TO_ANSI(*newURL)), "[Identity] XalPlatformWebShowUrlComplete for " + newURL);
			context->OnRedirectUrl.Unbind();
		}
		else if (newURL.Contains("USER_CANCEL")) {
			HrLog(XalPlatformWebShowUrlComplete(operation, XalPlatformOperationResult_Canceled, TCHAR_TO_ANSI(*finalUrl)), "[Identity] USER_CANCEL: XalPlatformWebShowUrlComplete");
		}
		return FLoginFlowResult();
	};
}
}


void Xal_PlatformWebShowUrlEventHandler(
	_In_opt_ void* context,
	_In_opt_ void* userContext,
	_In_ XalPlatformOperation operation,
	_In_z_ char const* startUrl,
	_In_z_ char const* finalUrl,
	_In_ XalShowUrlType showUrlType
)
{
	FOnlineSubsystemDungeons* dungeonsOSS = static_cast<FOnlineIdentityDungeons*>(context)->GetDungeonsOnlineSubsystem();
	ensure(dungeonsOSS);

	FOnlineExternalUIDungeonsPtr dungeonsUI = dungeonsOSS->GetExternalUIDungeons();
	if (!dungeonsUI->OnLoginFlowUIRequiredDelegates.IsBound()) {
		UE_LOG_ONLINE(Log, TEXT("[Identity] Login Request delegate Not Bound (restart editor?)"));
		return;
	}

	//Always unbind to update the operation
	dungeonsUI->OnRedirectUrl.Unbind();
	dungeonsUI->OnRedirectUrl.BindLambda(dungeonsIdentity::CreateUrlChangedLambda(finalUrl, operation, dungeonsUI));

	bool shouldContinueToLoginOut;
	dungeonsUI->TriggerOnLoginFlowUIRequiredDelegates(startUrl, dungeonsUI->OnRedirectUrl, FOnLoginFlowComplete(), shouldContinueToLoginOut);
}

FOnlineIdentityDungeonsPtr dungeonsIdentity::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineIdentityDungeonsWindows, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}

FOnlineIdentityDungeonsWindows::FOnlineIdentityDungeonsWindows(class FOnlineSubsystemDungeons* InSubsystem)
	:FOnlineIdentityDungeons(InSubsystem)
{
	InitializePlatform();
}

void FOnlineIdentityDungeonsWindows::InitializePlatform()
{
	XalInitArgs xalInitArgs = {};
	xalInitArgs.clientId = OnlineSubsystemConstants::CLIENTID;
	xalInitArgs.titleId = OnlineSubsystemConstants::TITLEID;
	xalInitArgs.flags = OnlineSubsystemConstants::FIRSTPARTYTITLEFLAG | OnlineSubsystemConstants::FOCIFLAG;
	xalInitArgs.sandbox = nullptr; //On Windows we should not set sandbox - XAL will automatically read this from the registry (if not present default to "RETAIL")

	const auto queueHandle = GetDungeonsOnlineSubsystem()->GetQueueHandle();


#if XAL_STORAGE
	xalStorage = std::make_unique<XalStorage>();
	if (xalStorage->Initialize(queueHandle))
	{
		/**
		 * Only apply the Win32 file flag if initialization of XalStorage was successful.
		 * If it wasn't, we can default back to the File storage included in XalExtras instead.
		 */
		xalInitArgs.flags = xalInitArgs.flags | OnlineSubsystemConstants::WIN32_USE_FILE_STORAGE;
	}
	else
	{
		XalExtra::Win32::FileStorage::Init(nullptr, nullptr);
	}
#else
	XalExtra::Win32::FileStorage::Init(nullptr, nullptr);
#endif

	HrCheck(XalPlatformWebSetEventHandler(
		queueHandle,
		static_cast<void*>(this),
		Xal_PlatformWebShowUrlEventHandler
	), "[Identity] XalPlatformWebSetEventHandler");

	HrCheck(XalInitialize(&xalInitArgs, queueHandle), "[Identity] XalInitialize");

	XblInitArgs args = { };
	args.scid = OnlineSubsystemConstants::SCID;
	args.localStoragePath = "TODO: When XAPI really needs it:\\Mojang\\Dungeons";
	args.queue = queueHandle;
	HrCheck(XblInitialize(&args), "[Identity] XblInitialize");

	AsyncInitialized = true;

	const HCTraceLevel logLevel = environmentConfig::IsShippingMode() ? HCTraceLevel::Off : HCTraceLevel::Error;
	HC_TRACE_SET_VERBOSITY(XAL, logLevel);
	HC_TRACE_SET_VERBOSITY(XAL_TELEMETRY, logLevel);
	HC_TRACE_SET_VERBOSITY(HTTPCLIENT, logLevel);

	UE_LOG_ONLINE(Log, TEXT("[Identity] XAL Initialized"));
	UE_LOG_ONLINE(Log, TEXT("[Identity] ClientId='%s'"), ANSI_TO_TCHAR(xalInitArgs.clientId));
	UE_LOG_ONLINE(Log, TEXT("[Identity] TitleId='%ud'"), xalInitArgs.titleId);
	UE_LOG_ONLINE(Log, TEXT("[Identity] SandboxId='%s'"), ANSI_TO_TCHAR(xalInitArgs.sandbox));
	UE_LOG_ONLINE(Log, TEXT("[Identity] SCID='%s'"), ANSI_TO_TCHAR(args.scid));
}

#endif