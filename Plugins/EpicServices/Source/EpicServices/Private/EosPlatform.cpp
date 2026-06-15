#include "EosPlatform.h"

#include "EpicProductConfig.h"

#include "Containers/Ticker.h"
#include "Containers/StringConv.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "GeneralProjectSettings.h"
#include "HAL/UnrealMemory.h"

#include <eos_logging.h>

namespace minecraft {
	namespace epicstore {

		DEFINE_LOG_CATEGORY(LogEpicOnlineSDK)

		/* free standing function wrappers around the UE4's memory management static methods to pass to the EOS SDK */
		void* UnrealAlloc(size_t const size, size_t const alignment)
		{
			return FMemory::Malloc(size, alignment);
		}

		void* UnrealRealloc(void* Original, size_t const SizeInBytes, size_t const Alignment)
		{
			return FMemory::Realloc(Original, SizeInBytes, Alignment);
		}

		void UnrealFree(void* ptr)
		{
			FMemory::Free(ptr);
		}
		/*---------------------------------------------------------------*/

		void FEosPlatform::Init()
		{
			bool initDone = InitPlatform();
			if (!initDone)
				return;

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Initialized. Creating the platform..."));

			bool platformCreated = CreatePlatform();
			if (!platformCreated)
				return;

			/* Start ticking every 100 ms */
			TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FEosPlatform::Tick), 0.1f);			
			bIsInitialized = true;
		}

		void FEosPlatform::Shutdown()
		{
			/* Stop ticking the EOS SDK */
			if (TickDelegateHandle.IsValid())
			{
				FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
				TickDelegateHandle.Reset();
			}
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Releasing the platform..."));
			EOS_Platform_Release(PlatformHandle);
			PlatformHandle = nullptr;

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Shutting down the SDK..."));
			EOS_EResult ShutdownResult = EOS_Shutdown();
			if (EOS_EResult::EOS_Success != ShutdownResult)
			{
				UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Failed to shutdown Error: %s"), UTF8_TO_TCHAR(EOS_EResult_ToString(ShutdownResult)));
			}
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Shutdown Complete"));
		}

		/**
		* Callback function to use for EOS SDK log messages
		*/
		void EOS_CALL EOSSDKLoggingCallback(EOS_LogMessage const *InMsg)
		{
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] %s: %s"), UTF8_TO_TCHAR(InMsg->Category), UTF8_TO_TCHAR(InMsg->Message));
		}

		/**
		 * Load EOS and Initializes the platform
		 */
		bool FEosPlatform::InitPlatform()
		{
			ANTICHEAT_VIRT_BEGIN
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Initializing ..."));

			const UGeneralProjectSettings& ProjectSettings = *GetDefault<UGeneralProjectSettings>();

			EOS_InitializeOptions SDKOptions = {};
			memset(&SDKOptions, 0, sizeof(EOS_InitializeOptions));
			SDKOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
			SDKOptions.AllocateMemoryFunction = UnrealAlloc;
			SDKOptions.ReallocateMemoryFunction = UnrealRealloc;
			SDKOptions.ReleaseMemoryFunction = UnrealFree;
			SDKOptions.ProductName = StringCast<ANSICHAR>(*ProjectSettings.ProjectName).Get();
			SDKOptions.ProductVersion = StringCast<ANSICHAR>(*ProjectSettings.ProjectVersion).Get();
			SDKOptions.Reserved = nullptr;
			SDKOptions.SystemInitializeOptions = nullptr;
			SDKOptions.OverrideThreadAffinity = nullptr;

			EOS_EResult InitResult = EOS_Initialize(&SDKOptions);
			if (InitResult != EOS_EResult::EOS_Success)
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Init Failed!"));
				return false;
			}
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Initialized. Setting Logging Callback ..."));
			EOS_EResult SetLogCallbackResult = EOS_Logging_SetCallback(&EOSSDKLoggingCallback);
			if (SetLogCallbackResult != EOS_EResult::EOS_Success)
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Set Logging Callback Failed!"));
			}
			else
			{
				UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Logging Callback Set"));
#if UE_BUILD_SHIPPING || UE_BUILD_TEST
				EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_Error);
#elif UE_BUILD_DEBUG
				EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_Verbose);
#else
				EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, EOS_ELogLevel::EOS_LOG_Info);
#endif // UE_BUILD_SHIPPING || UE_BUILD_TEST
			}
			return true;
			ANTICHEAT_VIRT_END
		}

		bool FEosPlatform::CreatePlatform()
		{
			ANTICHEAT_VIRT_BEGIN
			EOS_Platform_Options PlatformOptions = {};
			memset(&PlatformOptions, 0, sizeof(EOS_Platform_Options));
			PlatformOptions.ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
			PlatformOptions.Reserved = nullptr;

			PlatformOptions.ProductId = productconfig::ProductId;
			PlatformOptions.SandboxId = productconfig::SandboxId;

			PlatformOptions.ClientCredentials.ClientId = productconfig::ClientCredentialsId;
			PlatformOptions.ClientCredentials.ClientSecret = productconfig::ClientCredentialsSecret;

			PlatformOptions.bIsServer = EOS_FALSE;

			PlatformOptions.EncryptionKey = nullptr;
			PlatformOptions.OverrideCountryCode = nullptr;
			PlatformOptions.OverrideLocaleCode = nullptr;

			PlatformOptions.DeploymentId = productconfig::DeploymentId;
			
			PlatformOptions.Flags = EOS_PF_DISABLE_OVERLAY;
#if WITH_EDITOR
			PlatformOptions.Flags |= EOS_PF_LOADING_IN_EDITOR;
#endif // WITH_EDITOR

			// disabling this: we don't have Player or Title storage yet managed by Epic
			//PlatformOptions.CacheDirectory = FPlatformProcess::UserTempDir()->c_str();
			PlatformOptions.CacheDirectory = nullptr;
			PlatformOptions.TickBudgetInMilliseconds = 50;

			PlatformHandle = EOS_Platform_Create(&PlatformOptions);

			if (!PlatformHandle)
			{
				UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Failed to create the platform."));
				return false;
			}
			return true;
			ANTICHEAT_VIRT_END
		}

		bool FEosPlatform::Tick(float DeltaTime)
		{
			if (PlatformHandle) 
			{
				EOS_Platform_Tick(PlatformHandle);
				return true;
			}
			return false;
		}

	}
}
