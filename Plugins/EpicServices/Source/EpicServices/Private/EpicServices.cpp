#include "EpicServices.h"
#include "EosPlatform.h"
#include "PlayerManager.h"
#include "Authentication.h"
#include "EpicStore.h"

#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

#include <Anticheat.hpp>

namespace minecraft {
	namespace epicstore {

		class FEpicServicesModule::Impl
		{
		public:
			void Init() 
			{ 
				Platform.Init();
				Authentication.Init();
				Authentication.SetEpicAccountServicesSignInSucceededCallback(FOnEpicAccountServicesSignInSucceeded::CreateLambda(
					[this]() {
						PlayerPtr player{ nullptr };
						if (Store.HasPendingEntitlementRequest() && (player = PlayerManager.GetCurrentPlayer()))
							Store.QueryEntitlements(player->GetUserID());
					}
				));
			}

			void Shutdown()
			{
				Authentication.Shutdown();
				Platform.Shutdown();
			}

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
			/* Used to authenticate players in Development/Debug builds.
			 * Use the Developer Authentication Tool to set up the credentials.
			 * https://dev.epicgames.com/docs/services/en-US/DeveloperAuthenticationTool/index.html
			 * Not available in Test and Shipping builds.
			 */
			void AuthLoginDevAuthTool()
			{
				/* Default values used unless there is a command-line override */
				FString devKeyName{ "DevAuthToken" };
				FString address{ "localhost:7777" };
				/* if a specific key name has been provided - use it */
				FParse::Value(FCommandLine::Get(), TEXT("-DevKeyName="), devKeyName);
				/* if a custom host:port has been provided - use it */
				FParse::Value(FCommandLine::Get(), TEXT("-AuthToolAddress="), address);
				Authentication.AuthLoginDevelopment(MoveTemp(devKeyName), MoveTemp(address));
			}
#endif // UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

			/* Used to authenticate players in Test/Shipping builds */
			void AuthLoginEpicLauncher()
			{
				FString epicLauncherToken{ "noTokenPassed" };
				if (!FParse::Value(FCommandLine::Get(), TEXT("-AUTH_PASSWORD="), epicLauncherToken))
				{
					UE_LOG(LogEpicOnlineSDK, Error,
						TEXT("[EOS SDK] Failed to authenticate: no Epic Launcher Token passed. Process will request the launcher to restart it with an exchange code.")
					);
				}
				UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Signing into Epic Account Services with exchange code %s"), *epicLauncherToken);
				/* Pass even an invalid token, so that the SDK can initiate a retry */
				Authentication.AuthLoginEpicLauncher(MoveTemp(epicLauncherToken));
			}

			void SignIntoEpicGameServices(bool currentToken, char const *XboxLiveToken)
			{
				if (!PlayerManager.bSignedInEpicAccountServices)
				{
					UE_LOG(LogEpicOnlineSDK, Log
						, TEXT("[EOS SDK] The request to connect login with an Xbox Live Token is ignored "
								"because player has not authenticated via the Launcher yet. "
								"It is not an error and game will retry the operation later."));
				}
				else if (!currentToken || !PlayerManager.bSignedInEpicGameServices)
					Authentication.SignIntoEpicGameServices(XboxLiveToken);
			}

			void LogoutOfEpicGameServices()
			{
				UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Logging out of Epic Game Services..."));
				PlayerManager.RemoveAllPlayersEGS();
			}

			void RequestEntitlements(FOnEpicEntitlementsReceived successCallback, FOnEpicEntitlementsRequestFailed failedCallback)
			{
				PlayerPtr player{ nullptr };
				if (!PlayerManager.bSignedInEpicAccountServices || !(player=PlayerManager.GetCurrentPlayer()))
				{
					Store.SavePendingEntitlementRequest(successCallback, failedCallback);
					UE_LOG(LogEpicOnlineSDK, Log
						, TEXT("[EOS SDK] The entitlements request is ignored "
							 "because game has not authenticated with the Epic Account Services yet. "
							 "It is not an error and game will retry the operation later."));
				}
				else
					Store.QueryEntitlements(player->GetUserID(), successCallback, failedCallback);
			}

			void SetEpicGameServicesSignInFailedCallback(FOnEpicGameServicesSignInFailed f)
			{
				Authentication.SetEpicGameServicesSignInFailedCallback(f);
			}

			void SetLastBootTimestamp(int64 LastBootTimestamp)
			{
				Authentication.SetLastBootTimestamp(LastBootTimestamp);
			}

		private:
			FEosPlatform Platform{};
			FPlayerManager PlayerManager{};
			FAuthentication Authentication{ Platform, PlayerManager };
			FEpicStore Store{ Platform };
		};

		FEpicServicesModule::FEpicServicesModule() : PImpl{ MakeUnique<Impl>() }
		{}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
		FEpicServicesModule& FEpicServicesModule::Get()
		{
			ANTICHEAT_PROTECT_STRINGS_BEGIN
			const FName moduleName = "EpicServices";
			ANTICHEAT_PROTECT_STRINGS_END
			return FModuleManager::LoadModuleChecked<FEpicServicesModule>(moduleName);
		}

		void FEpicServicesModule::StartupModule()
		{
			ANTICHEAT_VIRT_BEGIN
			PImpl->Init();
			ANTICHEAT_VIRT_END
		}

		void FEpicServicesModule::ShutdownModule()
		{
			PImpl->Shutdown();
		}

		void FEpicServicesModule::SignIntoEpicAccountServices()
		{
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
			PImpl->AuthLoginDevAuthTool();
#else
			ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN
			PImpl->AuthLoginEpicLauncher();
			ANTICHEAT_VIRT_PROTECT_STRINGS_END
#endif
		}

		void FEpicServicesModule::SignIntoEpicGameServices(bool currentToken, char const *XboxLiveToken)
		{
			ANTICHEAT_VIRT_BEGIN
			PImpl->SignIntoEpicGameServices(currentToken, XboxLiveToken);
			ANTICHEAT_VIRT_END
		}
ANTICHEAT_NO_OPTIMIZATION_END

		void FEpicServicesModule::RequestEntitlements(FOnEpicEntitlementsReceived successCallback, FOnEpicEntitlementsRequestFailed failedCallback)
		{
			PImpl->RequestEntitlements(successCallback, failedCallback);
		}

		void FEpicServicesModule::SetEpicGameServicesSignInFailedCallback(FOnEpicGameServicesSignInFailed f)
		{
			PImpl->SetEpicGameServicesSignInFailedCallback(f);
		}

		void FEpicServicesModule::LogoutOfEpicGameServices()
		{
			PImpl->LogoutOfEpicGameServices();
		}

		void FEpicServicesModule::SetLastBootTimestamp(int64 LastBootTimestamp)
		{
			PImpl->SetLastBootTimestamp(LastBootTimestamp);
		}
	}
}

IMPLEMENT_MODULE(minecraft::epicstore::FEpicServicesModule, EpicServices)
