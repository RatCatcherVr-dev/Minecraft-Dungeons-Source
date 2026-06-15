#pragma once

#include "Delegates/DelegateCombinations.h"
#include "HAL/ThreadSafeBool.h"

#include <eos_auth.h>

#include "AccountHelpers.h"
#include "PlayerManager.h"

namespace minecraft {
	namespace epicstore {

		class FEosPlatform;

		DECLARE_DELEGATE(FOnEpicAccountServicesSignInSucceeded)
		DECLARE_DELEGATE(FOnEpicGameServicesSignInFailed);

		class FAuthentication
		{
		public:
			FAuthentication(FEosPlatform&, FPlayerManager&);
			FAuthentication(FAuthentication const&) = delete;
			FAuthentication& operator=(FAuthentication const&) = delete;
			
			void Init();

			void AuthLoginEpicLauncher(FString&& EpicLauncherToken);
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
			void AuthLoginDevelopment(FString&& DevelopmentKeyName, FString&& Address);
#endif // UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
			
			void SignIntoEpicGameServices(char const *XboxLiveToken);

			void SetEpicAccountServicesSignInSucceededCallback(FOnEpicAccountServicesSignInSucceeded f) { OnEpicAccountServicesSignInSucceeded = f; }
			void SetEpicGameServicesSignInFailedCallback(FOnEpicGameServicesSignInFailed f) { OnEpicGameServicesSignInFailed = f; }

			void SetLastBootTimestamp(int64 LastBoot) { LastBootTimestamp = LastBoot; }

			/* Called when EOS SDK is shutting down */
			void Shutdown();

		private:

			void SignIntoEpicAccountServices(EOS_ELoginCredentialType loginType, FString&& token, FString&& id);

			/** Login with Connect authentication using the Xbox Live token
			 */
			void SignIntoEpicGameServices(PlayerPtr, char const *XboxLiveToken);

			/* Adds/Removes notification for login status change */
			void AddNotifyLoginStatusChanged();
			void RemoveNotifyLoginStatusChanged();
			/* Adds/Removes expiration notification for connect authentication */
			void AddConnectAuthExpirationNotification();
			void RemoveConnectAuthExpirationNotification();

			/* callback that is fired when Auth login status has changed */
			static void EOS_CALL LoginStatusChangedCallback(EOS_Auth_LoginStatusChangedCallbackInfo const*);

			/** Callback that is fired when the login operation completes, either successfully or in error
			* @param Data - Output parameters for the EOS_Auth_Login Function
			*/
			static void EOS_CALL LoginCompleteCallback(EOS_Auth_LoginCallbackInfo const*);

			/** Callback that is fired when there is limited time left for the connect user auth token
			 * it is expected that the game will attempt to call EOS_Connect_Login again within the remaining time
			 * otherwise future calls to services will fail
			 */
			static void EOS_CALL ConnectAuthExpirationCallback(EOS_Connect_AuthExpirationCallbackInfo const*);

			/** Callback that is fired when the connect login operation completes, either successfully or in error
			* @param Data - Output parameters for the EOS_Auth_Logout Function
			*/
			static void EOS_CALL ConnectLoginCompleteCallback(EOS_Connect_LoginCallbackInfo const*);

			/**
			 * callback that is fired when the creation of a new connect user is complete
			 */
			static void EOS_CALL ConnectCreateUserCompleteCallback(EOS_Connect_CreateUserCallbackInfo const*);

			/** Callback that is fired when the logout operation completes, either successfully or in error
			* @param Data - Output parameters for the EOS_Auth_Logout Function
			*/
			static void EOS_CALL LogoutCompleteCallback(EOS_Auth_LogoutCallbackInfo const*);

			struct ConnectLoginContext
			{
				FAuthentication *Auth;
				PlayerPtr player;
			};

			/**
			 * callback that is fired when an attempted login to connect auth is completed
			 * can end in two success state
			 * - a user was found and auth token generated
			 * - a user was not found in which we will automatically create a user account in this sample
			 * -- typically the user will be prompted if they have other external credentials they'd like to login with
			 * -- this will allow the game to make a linkage between two different external accounts to one account here
			 */
			void OnConnectLoginComplete(EOS_EResult, EOS_ProductUserId, ConnectLoginContext*);

			/* Utility for printing auth token info */
			static void PrintAuthToken(EOS_Auth_Token *InAuthToken);

			/* Utility for printing Connect auth info */
			void PrintEOSAuthUsers();

			bool ShouldRetryEpicAccountServicesLogin();

			/* Notification ID used for Auth login status changed notification */
			EOS_NotificationId LoginStatusChangedId = EOS_INVALID_NOTIFICATIONID;

			/** Notification ID used for Connect Auth expiration notification */
			EOS_NotificationId ConnectAuthExpirationId = EOS_INVALID_NOTIFICATIONID;

			FEosPlatform &Platform;
			FPlayerManager &PlayerManager;

			FOnEpicAccountServicesSignInSucceeded OnEpicAccountServicesSignInSucceeded{};
			FOnEpicGameServicesSignInFailed OnEpicGameServicesSignInFailed{};

			/* Handle for Auth interface */
			EOS_HAuth AuthHandle{ nullptr };

			/* Handle for Connect interface */
			EOS_HConnect ConnectHandle{ nullptr };

			bool bIsInitialised{ false };

			int64 LastBootTimestamp;
		};

	}
}
