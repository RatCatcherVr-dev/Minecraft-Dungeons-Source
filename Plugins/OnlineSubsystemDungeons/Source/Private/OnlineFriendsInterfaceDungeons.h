#pragma once

#include "CoreMinimal.h"
#include "OnlineFriendsInterface.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlinePresenceInterface.h"
#include "xsapiServicesInclude.h"

class FOnlineFriendDungeons:
	public FOnlineFriend
{
public:

	FOnlineFriendDungeons(uint64 UniqueNetId, FOnlineUserPresence InPresence);
	virtual ~FOnlineFriendDungeons();

	virtual TSharedRef<const FUniqueNetId> GetUserId() const override;
	virtual FString GetRealName() const override;
	virtual FString GetDisplayName(const FString& Platform = FString()) const override;
	virtual bool GetUserAttribute(const FString& AttrName, FString& OutAttrValue) const override;
	virtual EInviteStatus::Type GetInviteStatus() const override;
	virtual const FOnlineUserPresence& GetPresence() const override;

	FString DisplayName;
	FString RealName;
	TSharedRef<const FUniqueNetId> UserId;
	FOnlineUserPresence Presence;
	TMap<FString, FString> Attributes;	

	virtual bool SetUserLocalAttribute(const FString& AttrName, const FString& AttrValue) override;
	void SetUserAttributeBool(const FString& AttrName, bool);
};

class FOnlineFriendsDungeons : public IOnlineFriends
{
	class FOnlineSubsystemDungeons* DungeonsSubsystem;
	TMap<FUniqueNetIdDungeons, TSharedRef<FOnlineFriendDungeons>> Friends;

	friend class OnlineAsyncTaskDungeonsUpdateFriendsList;

	XblHttpCallHandle MakeUpdateFriendCall(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, FString Method) const;
	XblUserHandle GetXBLUserHandle(int32 LocalUserNum) const;
	XblContextHandle GetXBLContextHandle(int32 LocalUserNum) const;
public:

	FOnlineFriendsDungeons(class FOnlineSubsystemDungeons* InDungeonsSubsystem);
	virtual ~FOnlineFriendsDungeons();	

	virtual bool ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete& Delegate = FOnReadFriendsListComplete()) override;
	virtual bool DeleteFriendsList(int32 LocalUserNum, const FString& ListName, const FOnDeleteFriendsListComplete& Delegate = FOnDeleteFriendsListComplete()) override;
	virtual bool SendInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnSendInviteComplete& Delegate = FOnSendInviteComplete()) override;
	virtual bool AcceptInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnAcceptInviteComplete& Delegate = FOnAcceptInviteComplete()) override;
	virtual bool RejectInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray< TSharedRef<FOnlineFriend> >& OutFriends) override;
	virtual TSharedPtr<FOnlineFriend> GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool IsFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) override;
	virtual bool QueryRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace) override;
	virtual bool GetRecentPlayers(const FUniqueNetId& UserId, const FString& Namespace, TArray< TSharedRef<FOnlineRecentPlayer> >& OutRecentPlayers) override;
	virtual bool BlockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) override;
	virtual bool UnblockPlayer(int32 LocalUserNum, const FUniqueNetId& PlayerId) override;
	virtual bool QueryBlockedPlayers(const FUniqueNetId& UserId) override;
	virtual bool GetBlockedPlayers(const FUniqueNetId& UserId, TArray< TSharedRef<FOnlineBlockedPlayer> >& OutBlockedPlayers) override;
	virtual void DumpBlockedPlayers() const override;
	virtual void DumpRecentPlayers() const override;

	FOnSendInviteComplete SendInviteComplete;
};

typedef TSharedPtr<FOnlineFriendsDungeons, ESPMode::ThreadSafe> FOnlineFriendsDungeonsPtr;
