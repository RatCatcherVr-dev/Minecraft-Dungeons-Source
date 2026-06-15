#include "Authentication.h"

#include "Logging/LogMacros.h"
#include "Containers/StringConv.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Misc/DateTime.h"

#include "EosPlatform.h"
#include "PlayerManager.h"
#include "EpicProductConfig.h"

namespace minecraft {
	namespace epicstore {

		FAuthentication::FAuthentication(FEosPlatform &p, FPlayerManager &m) : 
			Platform{p}, PlayerManager{m}
		{}

		void FAuthentication::Init()
		{
			if (!Platform.IsInitialized())
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Can't Log In - Platform Not Initialized!"));
				return;
			}
			AuthHandle = EOS_Platform_GetAuthInterface(Platform.GetHandle());
			checkf(AuthHandle != nullptr, TEXT("[EOS SDK] Failed to get the authentication handle."));

			ConnectHandle = EOS_Platform_GetConnectInterface(Platform.GetHandle());
			checkf(ConnectHandle != nullptr, TEXT("[EOS SDK] Failed to get the connect handle."));
			
			bIsInitialised = true;
		}

		void FAuthentication::AuthLoginEpicLauncher(FString&& EpicLauncherToken)
		{
			SignIntoEpicAccountServices(
				EOS_ELoginCredentialType::EOS_LCT_ExchangeCode,
				MoveTemp(EpicLauncherToken),
				""
			);
		}

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
		void FAuthentication::AuthLoginDevelopment(FString&& DevelopmentKeyName, FString&& Address)
		{		
			SignIntoEpicAccountServices(
				EOS_ELoginCredentialType::EOS_LCT_Developer,
				MoveTemp(DevelopmentKeyName),
				MoveTemp(Address)
			);
		}
#endif

		void FAuthentication::SignIntoEpicAccountServices(EOS_ELoginCredentialType loginType, FString&& token, FString&& id)
		{
			if (!bIsInitialised)
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Can't Log In - Authentication component Not Initialized!"));
				return;
			}
			ensure(AuthHandle);
			ensure(ConnectHandle);

			AddNotifyLoginStatusChanged();

			EOS_Auth_Credentials credentials{};
			memset(&credentials, 0, sizeof(EOS_Auth_Credentials));
			credentials.ApiVersion = EOS_AUTH_CREDENTIALS_API_LATEST;
			if (!id.IsEmpty())
				credentials.Id = StringCast<ANSICHAR>(*id).Get();
			credentials.Token = StringCast<ANSICHAR>(*token).Get();
			credentials.Type = loginType;


			EOS_Auth_LoginOptions LoginOptions{};
			memset(&LoginOptions, 0, sizeof(LoginOptions));
			LoginOptions.ApiVersion = EOS_AUTH_LOGIN_API_LATEST;
			LoginOptions.Credentials = &credentials;
			LoginOptions.ScopeFlags = EOS_EAuthScopeFlags::EOS_AS_BasicProfile;
						
			EOS_Auth_Login(AuthHandle, &LoginOptions, this, LoginCompleteCallback);
			AddConnectAuthExpirationNotification();
		}

		void EOS_CALL FAuthentication::LoginCompleteCallback(EOS_Auth_LoginCallbackInfo const *Data)
		{
			ensure(Data);
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Login Complete - User ID: %s")
				, EpicAccountIDToString(Data->LocalUserId));

			FAuthentication* Auth = static_cast<FAuthentication*>(Data->ClientData);
			ensure(Auth);
			EOS_HAuth AuthHandle = EOS_Platform_GetAuthInterface(Auth->Platform.GetHandle());
			ensure(AuthHandle);

			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				int32_t const AccountsCount = EOS_Auth_GetLoggedInAccountsCount(AuthHandle);
				for (int32_t AccountIdx = 0; AccountIdx < AccountsCount; ++AccountIdx)
				{
					FEpicAccountId AccountId;
					AccountId = EOS_Auth_GetLoggedInAccountByIndex(AuthHandle, AccountIdx);
					Auth->PlayerManager.AddPlayer(AccountId);

					EOS_ELoginStatus LoginStatus;
					LoginStatus = EOS_Auth_GetLoginStatus(AuthHandle, Data->LocalUserId);

					auto LoginStatusStr = LoginStatusStrings[static_cast<int32>(LoginStatus)];
					UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] [%d] - Account ID: %s, Status: %s")
						, AccountIdx, EpicAccountIDToString(AccountId), LoginStatusStr);
				}
				Auth->OnEpicAccountServicesSignInSucceeded.ExecuteIfBound();
			}
			else if (EOS_EResult_IsOperationComplete(Data->ResultCode))
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Login Failed - Error Code: %s")
					, UTF8_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

				if (Data->ResultCode == EOS_EResult::EOS_Auth_Expired
					|| Data->ResultCode == EOS_EResult::EOS_Auth_ExchangeCodeNotFound
					|| Data->ResultCode == EOS_EResult::EOS_Auth_OriginatingExchangeCodeSessionExpired)
				{
#if UE_BUILD_TEST || UE_BUILD_SHIPPING
					UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] The passed exchange code is invalid, requesting the Epic Games Launcher to restart the process."));
					/* Request the Epic Games launcher to restart the game and pass fresh exchange codes.
					* The condition is used to prevent the infinite boot loop in case Epic Launcher cannot assign a valid exchange code at all.
					*/
					if (Auth->ShouldRetryEpicAccountServicesLogin())
					{
						FPlatformProcess::LaunchURL(minecraft::epicstore::productconfig::LauncherGameURL, nullptr, nullptr);
						FGenericPlatformMisc::RequestExit(false);
					}
#endif // UE_BUILD_TEST || UE_BUILD_SHIPPING
					return;
				}
				else
				{
					/* Retry the authentication after a timeout */
					Auth->OnEpicGameServicesSignInFailed.ExecuteIfBound();
				}
			}
		}

		void FAuthentication::AddConnectAuthExpirationNotification()
		{
			if (ConnectAuthExpirationId == EOS_INVALID_NOTIFICATIONID)
			{
				EOS_Connect_AddNotifyAuthExpirationOptions Options{};
				Options.ApiVersion = EOS_CONNECT_ADDNOTIFYAUTHEXPIRATION_API_LATEST;
				ConnectAuthExpirationId = EOS_Connect_AddNotifyAuthExpiration(ConnectHandle, &Options, this, ConnectAuthExpirationCallback);
			}
		}

		void FAuthentication::RemoveConnectAuthExpirationNotification()
		{
			if (ConnectAuthExpirationId != EOS_INVALID_NOTIFICATIONID)
			{
				EOS_Connect_RemoveNotifyAuthExpiration(ConnectHandle, ConnectAuthExpirationId);
				ConnectAuthExpirationId = EOS_INVALID_NOTIFICATIONID;
			}
		}

		bool FAuthentication::ShouldRetryEpicAccountServicesLogin()
		{
			FTimespan elapsedTime = FDateTime::UtcNow() - FDateTime::FromUnixTimestamp(LastBootTimestamp);
			return elapsedTime.GetTotalMinutes() > 5;
		}

		void EOS_CALL FAuthentication::ConnectAuthExpirationCallback(EOS_Connect_AuthExpirationCallbackInfo const *Data)
		{
			ensure(Data);
			FAuthentication *Auth = static_cast<FAuthentication*>(Data->ClientData);
			ensure(Auth);

			FProductUserId productUserId(Data->LocalUserId);
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Connect Auth Expiring - ProductUserId: %s"), ProductUserIDToString(productUserId));

			/* No action needed here: the token is periodically refreshed by the Online Identity Dungeons
			* and player is signed in with a fresh token every so often.
			*/
		}

		void FAuthentication::SignIntoEpicGameServices(char const *XboxLiveToken)
		{
			for (auto const &Player : PlayerManager.GetEASLoggedInPlayers())
			{
				EOS_Auth_Token *UserAuthToken = nullptr;
				EOS_Auth_CopyUserAuthTokenOptions CopyTokenOptions = { 0 };
				CopyTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;

				if (EOS_Auth_CopyUserAuthToken(AuthHandle, &CopyTokenOptions, Player->GetUserID(), &UserAuthToken) == EOS_EResult::EOS_Success)
				{
					PrintAuthToken(UserAuthToken);
					EOS_Auth_Token_Release(UserAuthToken);
				}
				SignIntoEpicGameServices(Player, XboxLiveToken);
			}
		}		

		void FAuthentication::SignIntoEpicGameServices(PlayerPtr player, char const *XboxLiveToken)
		{
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Trying sign into Epic Game Services with User ID: %s"), EpicAccountIDToString(player->GetUserID()));

			EOS_Auth_Token *UserAuthToken = nullptr;

			ensure(AuthHandle);

			EOS_Auth_CopyUserAuthTokenOptions CopyTokenOptions = { 0 };
			CopyTokenOptions.ApiVersion = EOS_AUTH_COPYUSERAUTHTOKEN_API_LATEST;
			if (EOS_Auth_CopyUserAuthToken(AuthHandle, &CopyTokenOptions, player->GetUserID(), &UserAuthToken) == EOS_EResult::EOS_Success)
			{
				EOS_Connect_Credentials Credentials;
				Credentials.ApiVersion = EOS_CONNECT_CREDENTIALS_API_LATEST;
				Credentials.Token = XboxLiveToken;
				Credentials.Type = EOS_EExternalCredentialType::EOS_ECT_XBL_XSTS_TOKEN;

				EOS_Connect_LoginOptions Options = { 0 };
				Options.ApiVersion = EOS_CONNECT_LOGIN_API_LATEST;
				Options.Credentials = &Credentials;
				Options.UserLoginInfo = nullptr;

				ensure(ConnectHandle);

				ConnectLoginContext *context = new ConnectLoginContext();
				context->Auth = this;
				context->player = player;

				EOS_Connect_Login(ConnectHandle, &Options, context, ConnectLoginCompleteCallback);
				EOS_Auth_Token_Release(UserAuthToken);
			}
		}

		void EOS_CALL FAuthentication::ConnectLoginCompleteCallback(EOS_Connect_LoginCallbackInfo const *Data)
		{
			ensure(Data);
			ConnectLoginContext *context = static_cast<ConnectLoginContext*>(Data->ClientData);
			ensure(context);

			FAuthentication *Auth = context->Auth;
			if (Data->ResultCode == EOS_EResult::EOS_Success)
			{
				UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Epic Game Services Login using XBL is complete - ProductUserId: %s"), ProductUserIDToString(Data->LocalUserId));
				Auth->OnConnectLoginComplete(Data->ResultCode, Data->LocalUserId, context);
			}
			else if (Data->ResultCode == EOS_EResult::EOS_InvalidUser)
			{
				ensure(Auth->ConnectHandle);

				EOS_Connect_CreateUserOptions Options;
				Options.ApiVersion = EOS_CONNECT_CREATEUSER_API_LATEST;

				if (Data->ContinuanceToken != NULL)
				{
					Options.ContinuanceToken = Data->ContinuanceToken;
				}
				EOS_Connect_CreateUser(Auth->ConnectHandle, &Options, Data->ClientData, ConnectCreateUserCompleteCallback);
			}
			else
			{
				delete context;
				UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Epic Game Services Login failed with %s"), UTF8_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));
			}
		}

		void EOS_CALL FAuthentication::ConnectCreateUserCompleteCallback(EOS_Connect_CreateUserCallbackInfo const *Data)
		{
			ensure(Data);
			ConnectLoginContext *context = static_cast<ConnectLoginContext*>(Data->ClientData);
			ensure(context);

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Connect Create User Complete - ProductUserId: %s, Result: %s")
				, ProductUserIDToString(Data->LocalUserId), UTF8_TO_TCHAR(EOS_EResult_ToString(Data->ResultCode)));

			context->Auth->OnConnectLoginComplete(Data->ResultCode, Data->LocalUserId, context);
		}

		void FAuthentication::OnConnectLoginComplete(EOS_EResult Result, EOS_ProductUserId ProductUserId, ConnectLoginContext *context)
		{
			PrintEOSAuthUsers();
			ensure(context);

			if (Result == EOS_EResult::EOS_Success) 
			{
				PlayerManager.SetProductAccount(context->player->GetUserID(), ProductUserId);
			}
			else 
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] SignIntoEpicGameServices Failed - Error Code: %s")
					, UTF8_TO_TCHAR(EOS_EResult_ToString(Result)));
			}
			delete context;
		}

		void FAuthentication::PrintAuthToken(EOS_Auth_Token* InAuthToken)
		{
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] AuthToken:"));
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - App: %s"), UTF8_TO_TCHAR(InAuthToken->App));
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - ClientId: %s"), UTF8_TO_TCHAR(InAuthToken->ClientId));
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - AccountId: %s"), EpicAccountIDToString(InAuthToken->AccountId));
			
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - AccessToken: %s"), UTF8_TO_TCHAR(InAuthToken->AccessToken));
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - ExpiresIn: %0.2f"), InAuthToken->ExpiresIn);
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] - ExpiresAt: %s"), UTF8_TO_TCHAR(InAuthToken->ExpiresAt));
		}

		void FAuthentication::PrintEOSAuthUsers()
		{
			ensure(ConnectHandle);

			uint32_t NumAccounts = EOS_Connect_GetLoggedInUsersCount(ConnectHandle);
			for (uint32_t i = 0; i < NumAccounts; ++i)
			{
				EOS_ProductUserId ProductUserId = EOS_Connect_GetLoggedInUserByIndex(ConnectHandle, i);

				EOS_ELoginStatus LoginStatus = EOS_Connect_GetLoginStatus(ConnectHandle, ProductUserId);
				ensure(LoginStatus == EOS_ELoginStatus::EOS_LS_LoggedIn);

				UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Product User Id: %s, Status: %d"), ProductUserIDToString(ProductUserId), LoginStatus);
			}
		}

		void FAuthentication::Shutdown()
		{
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK][Authentication] Shutting Down ..."));
			RemoveNotifyLoginStatusChanged();
			RemoveConnectAuthExpirationNotification();
		}

		void FAuthentication::AddNotifyLoginStatusChanged()
		{
			if (LoginStatusChangedId == EOS_INVALID_NOTIFICATIONID)
			{
				EOS_Auth_AddNotifyLoginStatusChangedOptions LoginStatusChangedOptions{};
				LoginStatusChangedOptions.ApiVersion = EOS_AUTH_ADDNOTIFYLOGINSTATUSCHANGED_API_LATEST;

				LoginStatusChangedId = EOS_Auth_AddNotifyLoginStatusChanged(AuthHandle, &LoginStatusChangedOptions, this, LoginStatusChangedCallback);
			}
		}

		void FAuthentication::RemoveNotifyLoginStatusChanged()
		{
			if (LoginStatusChangedId != EOS_INVALID_NOTIFICATIONID) 
			{
				EOS_Auth_RemoveNotifyLoginStatusChanged(AuthHandle, LoginStatusChangedId);
				LoginStatusChangedId = 0;
			}
		}		

		void EOS_CALL FAuthentication::LoginStatusChangedCallback(EOS_Auth_LoginStatusChangedCallbackInfo const *Data)
		{
			ensure(Data);

			auto PrevLoginStatus = LoginStatusStrings[static_cast<int32>(Data->PrevStatus)];
			auto CurrentLoginStatus = LoginStatusStrings[static_cast<int32>(Data->CurrentStatus)];

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Auth Login Status Changed - UserId: %s, Prev: %s, Current: %s"),
				EpicAccountIDToString(Data->LocalUserId), PrevLoginStatus, CurrentLoginStatus);

			if (Data->PrevStatus == EOS_ELoginStatus::EOS_LS_LoggedIn && Data->CurrentStatus == EOS_ELoginStatus::EOS_LS_NotLoggedIn)
			{
				FAuthentication* Auth = static_cast<FAuthentication*>(Data->ClientData);
				ensure(Auth);
			}
		}

	}
}
