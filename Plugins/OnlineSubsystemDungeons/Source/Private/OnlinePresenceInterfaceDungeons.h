#pragma once

#include "CoreMinimal.h"
#include "OnlinePresenceInterface.h"
#include "OnlineSubsystemDungeons.h"

class FOnlineUserPresenceDungeons : public FOnlineUserPresence
{
public:
	FOnlineUserPresenceDungeons()
	{
		Reset();
	}
};

class FOnlinePresenceDungeons : public IOnlinePresence
{
	TMap<FUniqueNetIdDungeons, TSharedRef<class FOnlineUserPresenceDungeons>> CachedPresence;
	FOnlineSubsystemDungeons* DungeonsSubsystem;
	FString CurrentPresence;

public:
	FOnlinePresenceDungeons(FOnlineSubsystemDungeons* InDungeonsSubsystem);

	void SetPresence(const FUniqueNetId& User, const FOnlineUserPresenceStatus& Status, const FOnPresenceTaskCompleteDelegate& Delegate = FOnPresenceTaskCompleteDelegate()) override;
	void QueryPresence(const FUniqueNetId& User, const FOnPresenceTaskCompleteDelegate& Delegate = FOnPresenceTaskCompleteDelegate()) override;
	EOnlineCachedResult::Type GetCachedPresence(const FUniqueNetId& User, TSharedPtr<FOnlineUserPresence>& OutPresence) override;
	EOnlineCachedResult::Type GetCachedPresenceForApp(const FUniqueNetId& LocalUserId, const FUniqueNetId& User, const FString& AppId, TSharedPtr<FOnlineUserPresence>& OutPresence) override;

};

struct PresenceTaskCompleteBlock
{
	PresenceTaskCompleteBlock(const FUniqueNetId& User, const IOnlinePresence::FOnPresenceTaskCompleteDelegate& Delegate) :
		mUser(User),
		mDelegate(Delegate)
	{}

	const FUniqueNetId& mUser;
	const IOnlinePresence::FOnPresenceTaskCompleteDelegate& mDelegate;
};

typedef TSharedPtr<FOnlinePresenceDungeons, ESPMode::ThreadSafe> FOnlinePresenceDungeonsPtr;
