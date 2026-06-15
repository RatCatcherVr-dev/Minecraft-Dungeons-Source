#pragma once

#include "CoreMinimal.h"

#include "OnlineAsyncTaskManagerDungeons.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionInterface.h"
#include "OnlineIdentityInterface.h"
#include "OnlineExternalUIInterfaceDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "P2P/Peer2PeerManager.h"
#include "utils.h"

class GenericAsyncTask : public FOnlineAsyncTaskDungeons {
public:
	explicit GenericAsyncTask(std::function<void()> onTriggerDelegate)
		: OnTriggerDelegate(std::move(onTriggerDelegate))
	{
		check(OnTriggerDelegate && "Dont create task with unset function")
		bIsComplete = true;
		bWasSuccessful = true;
	}

	virtual void TriggerDelegates() override {
		if (OnTriggerDelegate) {
			OnTriggerDelegate();
		}
	}

private:
	std::function<void()> OnTriggerDelegate;
};

/* Dummy task to trigger OnFindSessionComplete from game thread */
class OnlineAsyncTaskDungeonsTriggerOnSessionInviteReceived : public FOnlineAsyncTaskDungeons
{
	IOnlineSessionPtr SessionPtr;
	FOnlineSessionSearchResult SearchResult;
	FString SenderName;
public:

	OnlineAsyncTaskDungeonsTriggerOnSessionInviteReceived(IOnlineSessionPtr InSessionPtr, const FOnlineSessionSearchResult& InSearchResult, const FString& InSenderName)
		: SessionPtr(InSessionPtr), SearchResult(InSearchResult), SenderName(InSenderName)
	{}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("OnlineAsyncTaskDungeonsTriggerOnSessionInviteReceived bWasSuccessful: %d"), WasSuccessful());
	}

	virtual void Tick() override 
	{
		bIsComplete = true;
		bWasSuccessful = true;
	}

	virtual void Finalize() override {}
	virtual void TriggerDelegates() override;
};

/* Task to trigger JoinSessionComplete when logged in to XMPP*/
class OnlineAsyncTaskDungeonsTriggerOnJoinSessionComplete : public FOnlineAsyncTaskDungeons
{
	FOnlineIdentityDungeons* IdentityPtr;
	std::function<void(bool Success, FName SessionName)> OnCompleteCallback;
	FName SessionName;

	unsigned long long TaskStart;

public:

	OnlineAsyncTaskDungeonsTriggerOnJoinSessionComplete(
		FOnlineIdentityDungeons* InIdentityPtr, 
		std::function<void(bool Success, FName SessionName)> OnCompleteCallback,
		FName InSessionName)
		: IdentityPtr(InIdentityPtr), OnCompleteCallback(OnCompleteCallback), SessionName(InSessionName)
	{
		TaskStart = TimerUtils::GetCurrentTime();
	}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("OnlineAsyncTaskDungeonsTriggerOnLoginComplete bWasSuccessful: %d"), WasSuccessful());
	}

	virtual void Tick() override;
	virtual void Finalize() override {}
	virtual void TriggerDelegates() override;
};
