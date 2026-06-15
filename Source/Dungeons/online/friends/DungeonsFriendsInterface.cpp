#include "Dungeons.h"
#include "util/Algo.h"
#include "DungeonsFriendsInterface.h"
#include "online/ui/OnlineTextLabels.h"
#include "online/sessions/SessionSettings.h"
#include "online/sessions/OnlineUtil.h"
#include "online/sessions/JoinDungeonsSessionCallbackProxy.h"
#include <OnlinePresenceInterface.h>
#include "Engine/LocalPlayer.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "util/EnumUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "online/crossplay/ExternalUI.h"
#include "online/crossplay/Friends.h"
#include "online/crossplay/Presence.h"

namespace friendsInterface {
	void SetOnlineUserPresence(const FOnlineUserPresence& userPresence, FBlueprintFriend& bpFriend) {
		bpFriend.IsOnline = userPresence.bIsOnline;
		bpFriend.IsPlayingThisGame = userPresence.bIsPlayingThisGame;
		bpFriend.InGameStatus = userPresence.Status.StatusStr;
	}

	bool EqualsUserAttribute(const FOnlineFriend& onlineFriend, const FString& attrName, const FString& attrCompareValue) {
		FString outValue;
		if (onlineFriend.GetUserAttribute(attrName, outValue)) {
			return outValue.Equals(attrCompareValue);
		}
		return false;
	}

	bool EqualsUserAttributeBool(const FOnlineFriend& onlineFriend, const FString& attrName, bool attrCompareValue) {
		return EqualsUserAttribute(onlineFriend, attrName, attrCompareValue ? FString("true") : FString("false"));
	}

	FString GetUserAttribute(const FOnlineFriend& onlineFriend, const FString& attrName) {
		FString outValue;
		if (onlineFriend.GetUserAttribute(attrName, outValue)) {
			return outValue;
		}
		return "";
	}

	FBlueprintFriend GetFriendByName(const TArray<FBlueprintFriend>& candidates, const FString& DisplayName) {
		for (auto buddy : candidates) {
			if (buddy.DisplayName.Equals(DisplayName)) {
				return buddy;
			}
		}
		return { "" };
	}

	FBlueprintFriend makeBpFriend(const FOnlineFriend& onlineFriend) {
		FBlueprintFriend bpFriend;
		bpFriend.DisplayName = onlineFriend.GetDisplayName();
		bpFriend.RealName = onlineFriend.GetRealName();
		bpFriend.InvitableStatus = EnumValueFromString(EFriendInvitableStatus, GetUserAttribute(onlineFriend, TEXT("invitablestatus"))).Get(EFriendInvitableStatus::BlockedUnknownReason);
		bpFriend.OnlineService = EnumValueFromString(EFriendOnlineService, GetUserAttribute(onlineFriend, TEXT("OnlineService"))).Get(EFriendOnlineService::PSN);
		bpFriend.IsInJoinableSession = false;
		bpFriend.isPlayingOnPs4 = bpFriend.OnlineService == EFriendOnlineService::PSN || onlineFriend.GetPresence().GetPlatform().Equals("PS4");
		bpFriend.UniqueNetId.SetUniqueNetId(onlineFriend.GetUserId());
		SetOnlineUserPresence(onlineFriend.GetPresence(), bpFriend);
		return bpFriend;
	}
}

UDungeonsFriendsInterface::UDungeonsFriendsInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, OnPresenceReceivedDelegate(FOnPresenceReceivedDelegate::CreateUObject(this, &UDungeonsFriendsInterface::OnPresenceReceived))
	, OnFriendsChangedDelegate(FOnFriendsChangeDelegate::CreateUObject(this, &UDungeonsFriendsInterface::OnFriendsChanged))
	, OnReadFriendsListComplete(FOnReadFriendsListComplete::CreateUObject(this, &UDungeonsFriendsInterface::OnReadFriendsCompleted))
	, OnFindFriendSessionCompleteDelegate(FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &UDungeonsFriendsInterface::OnFindSessionCompleted))
	, OnFriendAddedComplete(FOnSendInviteComplete::CreateUObject(this, &UDungeonsFriendsInterface::OnAddFriendCompleted))
{
}

FBlueprintPlayerInfoList UDungeonsFriendsInterface::GetAllFriends() {
	return AllFriends;
}

FBlueprintPlayerInfoList UDungeonsFriendsInterface::GetOnlineFriends() {
	return algo::copy_if(AllFriends.Players, RETLAMBDA(it.IsOnline));
}

FBlueprintPlayerInfoList UDungeonsFriendsInterface::GetFriendsInDungeons() {
	return algo::copy_if(AllFriends.Players, RETLAMBDA(it.IsPlayingThisGame));
}

bool UDungeonsFriendsInterface::JoinSessionOfFriend(const FBlueprintFriend& player) {
	if (auto sessioninterface = online::getSessionInterface()) {
		auto JoinSessionProxy = UJoinDungeonsSessionCallbackProxy::JoinDungeonsSession(GetWorld(), GetWorld()->GetFirstPlayerController(), player.SessionSearchResult);
		JoinSessionProxy->Activate();
	}
	return false;
}

bool UDungeonsFriendsInterface::ShouldDisplayFriend(const FBlueprintFriend& player) {
	if (auto sessionIface = online::getSessionInterface()) {
		return player.UniqueNetId.IsValid() && !sessionIface->IsPlayerInSession(DungeonsGameSessionName, *player.UniqueNetId);
	}
	return true;
}

bool UDungeonsFriendsInterface::CanDisplayProfileOfName(const FString& DisplayName) {
	FBlueprintFriend buddy = friendsInterface::GetFriendByName(AllFriends.Players, DisplayName);
	if (buddy.DisplayName.IsEmpty()) {
		return false;
	}

	if (PLATFORM_PS4) {
		return buddy.isPlayingOnPs4 && buddy.OnlineService == EFriendOnlineService::PSN;
	}
	else {
		return !buddy.isPlayingOnPs4;
	}
}

bool UDungeonsFriendsInterface::ShowProfileOfName(const FString& DisplayName) {
	if (!CanDisplayProfileOfName(DisplayName)) {
		return false;
	}
	FBlueprintFriend buddy = friendsInterface::GetFriendByName(AllFriends.Players, DisplayName);
	if (buddy.DisplayName.IsEmpty() || !buddy.UniqueNetId.IsValid()) {
		return false;
	}
	if (const auto externalUI = online::getExternalUIInterface()) {
		auto localUserId = GetLocalPlayerId();
		if (localUserId.IsValid()) {
			return externalUI->ShowProfileUI(*localUserId, *buddy.UniqueNetId);
		}
	}
	return false;
}

EFriendInvitableStatus UDungeonsFriendsInterface::GetFriendInvitableStatus(const FBlueprintFriend& player){
	if (!IsSessionOpenForInvites()) {
		return EFriendInvitableStatus::BlockedSessionClosed;
	}
	if (!ShouldDisplayFriend(player)) {
		return EFriendInvitableStatus::BlockedInvalidPlayerId;
	}
	return player.InvitableStatus;	
}

bool UDungeonsFriendsInterface::IsSessionOpenForInvites() {
	if (const auto* session = online::getCurrentSession()) {
		const FLevelSettings& levelSettings = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())->GetLevelSettingsLastStarted();
		if (UMissionDefs::IsTutorial(levelSettings.getLevelName())) {
			return false;
		}

		bool openSlots = 4 - session->RegisteredPlayers.Num() > 0;
		return openSlots && session->SessionSettings.bAllowInvites;
	}
	return false;
}

bool UDungeonsFriendsInterface::IsPlayingMultiplayer() {
	if (const auto* session = online::getCurrentSession()) {
		return session->RegisteredPlayers.Num() > 1;
	}
	return false;
}

bool UDungeonsFriendsInterface::UpdateFriendsList() {

	if (!online::isLoggedInOnline(GetWorld())) {
		UE_LOG(LogOnline, Error, TEXT("No logged in user when trying to read friends"));
		return false;
	}

	if (!online::getFriendsInterface()->ReadFriendsList(GetInitialControllerID(), EFriendsLists::ToString(EFriendsLists::Default), OnReadFriendsListComplete)) {
		UE_LOG(LogOnline, Error, TEXT("Failed to read friends list"));
		return false;
	}
	return true;
}

void UDungeonsFriendsInterface::SetPresenceStatus(const FString& levelName) {
	auto sessions = online::getSessionInterface();
	if (!sessions) {
		return;
	}

	const auto presence = online::getCrossplayOss()->GetPresenceIF();
	if (!presence) {
		UE_LOG(LogOnline, Error, TEXT("Found no presence interface"));
		return;
	}

	const auto localPlayer = GetLocalPlayerId();
	if (!localPlayer) {
		UE_LOG(LogOnline, Error, TEXT("Unable to get local player when setting presence"));
		return;
	}

	FOnlineUserPresenceStatus status;
	FNamedOnlineSession* session = sessions->GetNamedSession(DungeonsGameSessionName);
	if (session) {
		FString levelString = levelName.IsEmpty() ? online::ui::lobbyName().ToString() : levelName;
		SessionSettings ss(session->SessionSettings);

		if (IsPrivateGame() || levelName.Equals(online::EmptyPresence)) {
			status.StatusStr = "";
		}
		else {
			int players = session->RegisteredPlayers.Num();
			FString playersString = FString(" (") + FString::FromInt(players) + FString("/") + FString::FromInt(session->SessionSettings.NumPublicConnections) + FString(")");
			status.StatusStr = levelString + playersString;
		}
	}
	else {
		status.StatusStr = levelName;
	}

	if (!status.StatusStr.Equals(PresenceStatusString)) {
		presence->SetPresence(*localPlayer, status);
		PresenceStatusString = status.StatusStr;
	}
}

bool UDungeonsFriendsInterface::ShowSessionPlayerProfile(int localPlayerIndex, const ABasePlayerState* targetPlayer) {
	if (!targetPlayer) {
		return false;
	}

	if (const auto externalUI = online::getExternalUIInterface()) {
		const auto localPlayer = GetNetIDFromPlayerIndex(localPlayerIndex);
		if (localPlayer && localPlayer->IsValid() && targetPlayer->UniqueId.IsValid()) {
			if (!online::getCrossplayOss()->IsPS4Active() && targetPlayer->SecondaryUniqueId.IsValid()) {
				return externalUI->ShowProfileUI(*localPlayer.Get(), *targetPlayer->SecondaryUniqueId.GetUniqueNetId());
			}
			return externalUI->ShowProfileUI(*localPlayer.Get(), *targetPlayer->UniqueId.GetUniqueNetId());
		}
	}

	return false;
}

bool UDungeonsFriendsInterface::ShowPlayerProfile(int localPlayerIndex, const FBlueprintFriend& targetPlayer) {
	if (const auto externalUI = online::getExternalUIInterface()) {
		const auto localPlayer = GetNetIDFromPlayerIndex(localPlayerIndex);
		if (localPlayer && localPlayer->IsValid() && targetPlayer.UniqueNetId->IsValid()) {
			return externalUI->ShowProfileUI(*localPlayer.Get(), *targetPlayer.UniqueNetId.GetUniqueNetId());
		}
	}

	return false;
}

bool UDungeonsFriendsInterface::ShowLocalPlayerProfile(int localPlayerIndex, int targetPlayerIndex) {
	if (const auto externalUI = online::getExternalUIInterface()) {
		const auto localPlayer = GetNetIDFromPlayerIndex(localPlayerIndex);
		const auto targetPlayer = GetNetIDFromPlayerIndex(targetPlayerIndex);

		if (localPlayer && localPlayer->IsValid() && targetPlayer && targetPlayer->IsValid()) {
			return externalUI->ShowProfileUI(*localPlayer.Get(), *targetPlayer.Get());
		}
	}

	return false;
}

TSharedPtr<const FUniqueNetId> UDungeonsFriendsInterface::GetNetIDFromPlayerIndex(int localPlayerIndex) {
	if (const auto idIF = online::getIdentityInterface()) {
		return idIF->GetUniquePlayerId(localPlayerIndex);
	}
	return nullptr;
}

EFriendsAddResponse UDungeonsFriendsInterface::AddPlayerAsFriend(const FBlueprintFriend& player) {
	if (!player.UniqueNetId->IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("Cant add player as friend when friend has no valid UniqueNetId"));
		return EFriendsAddResponse::MissingParameter;
	}

	if (algo::any_of(AllFriends.Players, RETLAMBDA(it.UniqueNetId == player.UniqueNetId))) {
		UE_LOG(LogOnline, Log, TEXT("Player is already in friends list"));
		return EFriendsAddResponse::FriendAlreadyAdded;
	}

	const bool sent = online::getFriendsInterface()->SendInvite(GetInitialControllerID(), *player.UniqueNetId->AsShared(), "", OnFriendAddedComplete);
	return sent ? EFriendsAddResponse::Searching : EFriendsAddResponse::GeneralError;
}

EFriendsAddResponse UDungeonsFriendsInterface::AddFriendByListName(const FString& listName) {
	if (listName.IsEmpty()) {
		UE_LOG(LogOnline, Log, TEXT("Cant add friend by list name with empty listname"));
		return EFriendsAddResponse::MissingParameter;
	}

	if (algo::any_of(AllFriends.Players, RETLAMBDA(it.DisplayName == listName))) {
		UE_LOG(LogOnline, Log, TEXT("List name is already in friends list"));
		return EFriendsAddResponse::FriendAlreadyAdded;
	}

	const bool sent = online::getFriendsInterface()->SendInvite(GetInitialControllerID(), FUniqueNetIdString(""), listName, OnFriendAddedComplete);
	return sent ? EFriendsAddResponse::Searching : EFriendsAddResponse::GeneralError;
}

bool UDungeonsFriendsInterface::SupportsAddFriend() const {
	return !PLATFORM_XBOXONE && online::getCrossplayOss()->IsDungeonsActive();
}

void UDungeonsFriendsInterface::RemoveFriend(const FBlueprintFriend& player) {
	if (!algo::any_of(AllFriends.Players, RETLAMBDA(it.UniqueNetId == player.UniqueNetId))) {
		UE_LOG(LogOnline, Log, TEXT("Player is not in friendslist"));
		return;
	}

	if (player.UniqueNetId->AsShared()->IsValid()) {
		online::getFriendsInterface()->DeleteFriend(0, *player.UniqueNetId->AsShared(), "");
	}
}

//TODO -> Add LocalUserNum and trigger in a proper state
bool UDungeonsFriendsInterface::ConfigureDelegates() {
	auto presence = online::getCrossplayOss()->GetPresenceIF();
	if (!presence) {
		UE_LOG(LogOnline, Error, TEXT("Found no presence interface"));
		return false;
	}

	OnPresenceReceivedDelegateHandle = presence->AddOnPresenceReceivedDelegate_Handle(OnPresenceReceivedDelegate);
	OnFriendsChangedDelegateHandle = online::getFriendsInterface()->AddOnFriendsChangeDelegate_Handle(0, OnFriendsChangedDelegate);
	return true;
}
//TODO -> Add LocalUserNum and trigger in a proper state
bool UDungeonsFriendsInterface::DeleteDelegates() {
	auto presence = online::getCrossplayOss()->GetPresenceIF();
	if (!presence) {
		UE_LOG(LogOnline, Error, TEXT("Found no presence interface"));
		return false;
	}
	presence->ClearOnPresenceReceivedDelegate_Handle(OnPresenceReceivedDelegateHandle);
	online::getFriendsInterface()->ClearOnFriendsChangeDelegate_Handle(0, OnFriendsChangedDelegateHandle);
	return true;
}

void UDungeonsFriendsInterface::OnReadFriendsCompleted(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString) {
	if (!bWasSuccessful) {
		UE_LOG(LogOnline, Error, TEXT("Friends Interface: failed to read friends (%s)"), *ErrorString);
		FriendsUpdateFailed.Broadcast();
		return;
	}

	auto presence = online::getCrossplayOss()->GetPresenceIF();
	if (!presence) {
		UE_LOG(LogOnline, Error, TEXT("(OSS) Presence interface unavailable"));
		FriendsUpdateFailed.Broadcast();
		return;
	}

	presence->ClearOnPresenceReceivedDelegate_Handle(OnPresenceReceivedDelegateHandle);
	OnPresenceReceivedDelegateHandle = presence->AddOnPresenceReceivedDelegate_Handle(OnPresenceReceivedDelegate);
	if (!OnPresenceReceivedDelegateHandle.IsValid()) {
		UE_LOG(LogOnline, Error, TEXT("Unable to bind presence received delegate"));
		FriendsUpdateFailed.Broadcast();
		return;
	}

	online::getFriendsInterface()->ClearOnFriendsChangeDelegate_Handle(GetInitialControllerID(), OnFriendsChangedDelegateHandle);
	OnFriendsChangedDelegateHandle = online::getFriendsInterface()->AddOnFriendsChangeDelegate_Handle(GetInitialControllerID(), OnFriendsChangedDelegate);
	if (!OnFriendsChangedDelegateHandle.IsValid()) {
		UE_LOG(LogOnline, Error, TEXT("Unable to bind friends change delegate"));
		FriendsUpdateFailed.Broadcast();
		return;
	}

	TArray<TSharedRef<FOnlineFriend>> FriendsList;
	if (!online::getFriendsInterface()->GetFriendsList(GetInitialControllerID(), EFriendsLists::ToString(EFriendsLists::Default), FriendsList)) {
		UE_LOG(LogOnline, Error, TEXT("FriendsInterface: failed to get friends"));
		FriendsUpdateFailed.Broadcast();
		return;
	}
	
	AllFriends.Players.Empty();

	for (auto& onlineFriend : FriendsList) {
		FBlueprintFriend bpFriend = friendsInterface::makeBpFriend(*onlineFriend);
		AllFriends.Players.Add(bpFriend);
		if (bpFriend.IsPlayingThisGame) {
			AddRequest(*bpFriend.UniqueNetId);
		}
	}
	AllFriends.Players.Sort();
	FriendsUpdateCompleted.Broadcast();
}

void UDungeonsFriendsInterface::OnAddFriendCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& FriendId, const FString& ListName, const FString& ErrorString) {
	UE_LOG(LogOnline, Log, TEXT("Friends Interface: Friend %s added, Error string: %s"), *FriendId.ToString(), *ErrorString);
	AddFriendCompleted.Broadcast(bWasSuccessful ? EFriendsAddResponse::FriendAdded : EFriendsAddResponse::NoMatch, ListName, ErrorString);
}

void UDungeonsFriendsInterface::AddRequest(const FUniqueNetId& UserId) {
	if (FriendSessionRequests.Contains(UserId)) {
		return;
	}
	UE_LOG(LogOnline, Log, TEXT("FriendsInterface: Adding request for user: %s. Request queue size: %d"), *UserId.ToString(), FriendSessionRequests.Num());
	FriendSessionRequests.Add(UserId);
	if (FriendSessionRequests.Num() == 1) { // trigger request if this is the only one in the queue
		NextRequest();
	}
}

void UDungeonsFriendsInterface::NextRequest() {
	if (auto sessionIface = online::getSessionInterface()) {
		sessionIface->ClearOnFindFriendSessionCompleteDelegate_Handle(GetInitialControllerID(), OnFindFriendSessionCompleteDelegateHandle);
		OnFindFriendSessionCompleteDelegateHandle = sessionIface->AddOnFindFriendSessionCompleteDelegate_Handle(GetInitialControllerID(), OnFindFriendSessionCompleteDelegate);
		sessionIface->FindFriendSession(GetInitialControllerID(), *FriendSessionRequests[0]);
	}
}

void UDungeonsFriendsInterface::OnFindSessionCompleted(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& results) {
	if (FriendSessionRequests.Num() == 0) {
		return;
	}

	UE_LOG(LogOnline, Log, TEXT("FriendsInterface: Request queue size: %d, was successful: %d, result size: %d"), FriendSessionRequests.Num(), bWasSuccessful, results.Num());
	if (results.Num() > 0) {
		int32 friendIndex = GetFriendIndex(*FriendSessionRequests[0]);
		if (friendIndex != INDEX_NONE) {
			AllFriends.Players[friendIndex].IsInJoinableSession = true;
			AllFriends.Players[friendIndex].SessionSearchResult = results[0];
		}
	}

	FriendSessionRequests.RemoveAt(0);

	if (FriendSessionRequests.Num() > 0) {
		UE_LOG(LogOnline, Log, TEXT("FriendsInterface: Sending another request, Request queue size: %d"), FriendSessionRequests.Num());
		NextRequest();
	}
	else {
		AllFriends.Players.Sort();
		FriendsUpdateCompleted.Broadcast();
	}
}

int32 UDungeonsFriendsInterface::GetFriendIndex(const FUniqueNetId& UserId) {
	if (!UserId.IsValid()) {
		return INDEX_NONE;
	}

	const auto predicate = [&](const FBlueprintFriend& bpFriend) {
		return *bpFriend.UniqueNetId == UserId;
	};

	return AllFriends.Players.IndexOfByPredicate(predicate);
}


void UDungeonsFriendsInterface::OnFriendsChanged() {
	UpdateFriendsList();
}

void UDungeonsFriendsInterface::OnPresenceReceived(const FUniqueNetId& UserId, const TSharedRef<FOnlineUserPresence>& PresenceData) {
	int32 friendIndex = GetFriendIndex(UserId);
	if (friendIndex == INDEX_NONE) {
		return;
	}
	if (const auto updatedFriend = online::getFriendsInterface()->GetFriend(GetInitialControllerID(), UserId, EFriendsLists::ToString(EFriendsLists::Default))) {
		auto bpFriend = friendsInterface::makeBpFriend(*updatedFriend);
		friendsInterface::SetOnlineUserPresence(*PresenceData, bpFriend);
		AllFriends.Players[friendIndex] = bpFriend;
		if (bpFriend.IsPlayingThisGame) {
			AddRequest(*bpFriend.UniqueNetId);
		}
	}
}

TSharedPtr<const FUniqueNetId> UDungeonsFriendsInterface::GetLocalPlayerId() {

	TSharedPtr<online::Crossplay::Identity> identity = online::getIdentityInterface();
	if (!identity.IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("Invalid Identity Interface"));
		return nullptr;
	}

	TSharedPtr<const FUniqueNetId> LocalPlayerId = identity->GetUniquePlayerId(GetInitialControllerID());
	if (identity->GetLoginStatus(GetInitialControllerID()) != ELoginStatus::LoggedIn) {
		UE_LOG(LogOnline, Log, TEXT("Local player is not logged in"));
		return nullptr;
	}
	return LocalPlayerId;
}

bool UDungeonsFriendsInterface::IsValidFriend(FBlueprintFriend player) {
	// D11.SSN
	if (player.UniqueNetId.IsValid()) {
		return player.UniqueNetId->IsValid();
	}
	return false;
}

int32 UDungeonsFriendsInterface::GetInitialControllerID() const
{
	if (GetWorld() && GetWorld()->GetGameInstance() && GetWorld()->GetGameInstance()->GetLocalPlayerByIndex(0))
		return GetWorld()->GetGameInstance()->GetLocalPlayerByIndex(0)->GetControllerId();

	return 0;
}

bool UDungeonsFriendsInterface::SetPrivateGame(bool privateGame) {
	if (auto sessionIface = online::getSessionInterface()) {
		if (auto session = online::getCurrentSession()){
			SessionSettings ss(session->SessionSettings);
			ss.SetPrivate(privateGame);
			if(sessionIface->UpdateSession(DungeonsGameSessionName, ss.Get()))
			{
				PrivateGame = privateGame;
				return true;
			}
		}
		else
		{
			PrivateGame = privateGame;
			return true;
		}
	}
	return false;
}

bool UDungeonsFriendsInterface::IsPrivateGame() {
	return PrivateGame;
}