// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "PlayfabServicesEvent.h"
#include "Containers/Queue.h"
#include "CoreOnline.h"
#include "PlayfabPlayer.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayfabServices, Display, All);


enum class PlayfabServicesInitState : uint8_t {
    NotInitialized = 0,
    Success,
    ConnectionFailure
};

enum class AccountLinkStatus: uint8_t {
	NotLinked,
	Linked,
	ThisPsnAlreadyLinkedToDifferentMSA,
	ThisMsaAlreadyLinkedToDifferentPSN,
	Error
};

struct AccountLinkConfig {
	using ResultCallback = TFunction<void(AccountLinkStatus)>;

	FString psnAuthToken;
	FString xblAuthToken;
	FString issuerId;
	FString xblAccount;
	ResultCallback callback;
};

struct AccountUnlinkConfig {
	using ResultCallback = TFunction<void(bool)>;

	FString psnAuthToken;
	FString issuerId;
	FString xblAccount;
	ResultCallback callback;
};

struct AuthenticationConfig {
	using ResultCallback = TFunction<void(bool)>;

	FString authToken;
	FString issuerId;
	TFunction<void(bool success)> callback;
};

class PLAYFABSERVICES_API PlayfabServices : public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;
    static PlayfabServicesInitState GetInitState();
	static void PlayfabCleanup();

	static bool RefreshAuthenticationCredentials(const AuthenticationConfig&);
	static bool RegisterOnAuthenticationCallback(const AuthenticationConfig&);

	using DequeueFunction = TFunction<void()>;
	static void SendTelemetry(const PlayfabServicesEvent& event);
	static void AddPlayerToCache(const FString&);

	static void SetAuthenticationTokenFunction(PlayFabPlayer::AuthTokenFunction func) { AuthoTokenFunc = func; }
	static void ClearAuthenticationTokenFunction() { AuthoTokenFunc = {}; }

	static AccountLinkStatus GetAccountLinkStatus(FString);
	static FString GetLinkedXblAccount();
	static bool LinkAccounts(AccountLinkConfig);
	static bool UnlinkXBLAccount(AccountUnlinkConfig);

	static bool SetGamertagHint(FString xblGamerTag);
	static bool GetGamertagHint(TFunction<void(FString gamerTag)> callback);
	static class PlayFabPlayer* GetPlayfabPlayer();
	static FString GetTitleId();

	static class IOnlineUserCloud* GetUserCloudInterface();
private:
	static bool Authenticate(const AuthenticationConfig&);

	static void AnalyticsTick();
	static void DequeueTelemetry();

	static class PlayFabClient* GetPlayFabClient();
	static PlayFabPlayer::AuthTokenFunction AuthoTokenFunc;
	static TQueue<PlayfabServicesEvent, EQueueMode::Mpsc> queuedAnalyticsEvents;
};

namespace LoginFlowConstants {
	static const FString NetworkIssueError ("NetworkIssueError");
	static const FString FailedMSALoginError ("failed_msa_login");
	static const FString FailedPlayfabLoginError ("failed_playfab_login");
	static const FString GenericLinkingError ("generic_link_error");
	static const FString MSALinkedToDifferentPSNError ("msa_already_linked_to_different_psn");
	static const FString PSNLinkedToDifferentMSAError ("psn_already_linked_to_different_msa");
	static const FString GuestAccountError ("guest_account");
	static const FString AlreadyLoggedInUserError("AlreadyLoggedInUserError");
	static const FString SignInCancelled("signin_cancelled");
	static const FString SpopIssue("spop_issue"); /* player already logged in on a different device */
}
