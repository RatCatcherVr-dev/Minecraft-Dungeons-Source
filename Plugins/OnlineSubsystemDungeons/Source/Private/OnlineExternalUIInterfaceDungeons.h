#pragma once

#include "CoreMinimal.h"

#include "OnlineExternalUIInterface.h"
#include "OnlineIdentityInterface.h"

#include "OnlineAsyncTaskManagerDungeons.h"

class FOnlineSubsystemDungeons;

class FOnlineExternalUIDungeons : public IOnlineExternalUI
{

public:
	FOnlineExternalUIDungeons(FOnlineSubsystemDungeons* InSubsystem);
	~FOnlineExternalUIDungeons();

	virtual bool ShowLoginUI(const int LocalPlayerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate = FOnLoginUIClosedDelegate()) override;
	virtual bool ShowAccountCreationUI(const int ControllerIndex, const FOnAccountCreationUIClosedDelegate& Delegate = FOnAccountCreationUIClosedDelegate()) override { return false; }
	virtual bool ShowFriendsUI(int32 LocalUserNum) override { return false; }
	virtual bool ShowInviteUI(int32 LocalUserNum, FName SessionName = NAME_GameSession) { return false; }
	virtual bool ShowAchievementsUI(int32 LocalUserNum) { return false; }
	virtual bool ShowLeaderboardUI(const FString& LeaderboardName) { return false; }
	virtual bool ShowWebURL(const FString& Url, const FShowWebUrlParams& ShowParams, const FOnShowWebUrlClosedDelegate& Delegate = FOnShowWebUrlClosedDelegate()) { return false; }
	virtual bool CloseWebURL() { return false; }
	virtual bool ShowProfileUI(const FUniqueNetId& Requestor, const FUniqueNetId& Requestee, const FOnProfileUIClosedDelegate& Delegate = FOnProfileUIClosedDelegate()) override;
	virtual bool ShowAccountUpgradeUI(const FUniqueNetId& UniqueId) { return false; }
	virtual bool ShowStoreUI(int32 LocalUserNum, const FShowStoreParams& ShowParams, const FOnShowStoreUIClosedDelegate& Delegate = FOnShowStoreUIClosedDelegate()) override;
	virtual bool ShowSendMessageUI(int32 LocalUserNum, const FShowSendMessageParams& ShowParams, const FOnShowSendMessageUIClosedDelegate& Delegate = FOnShowSendMessageUIClosedDelegate()) { return false; }

	void TriggerUIClosedDelegate(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	FOnLoginFlowUIRequiredDelegate OnRequireLogin;
	FOnLoginRedirectURL OnRedirectUrl;
private:
	FOnlineSubsystemDungeons* Subsystem;
	FOnLoginUIClosedDelegate ShowLoginDelegate;

	FOnLoginCompleteDelegate OnLoginCompleteDelegate;
	TArray<FDelegateHandle> OnLoginCompleteDelegateHandles; //D11.PC

	int	ShowLoginControllerIndex;
};
typedef TSharedPtr<FOnlineExternalUIDungeons, ESPMode::ThreadSafe> FOnlineExternalUIDungeonsPtr;

#if PLATFORM_SWITCH
class FOnlineAsyncTaskTrackStoreUIStatus : public FOnlineAsyncTaskDungeons
{
public:
	FOnlineAsyncTaskTrackStoreUIStatus(int32 InLocalUserNum, const FShowStoreParams& InShowParams, const FOnShowStoreUIClosedDelegate& OnShowStoreUIClosedDelegate);

	virtual void Tick() override;

private:
	FOnShowStoreUIClosedDelegate ShowStoreUIClosedDelegate;
	int32 LocalUserNum;
	FShowStoreParams ShowParams;
};
#endif
