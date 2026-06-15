#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "InviteDungeonsSessionCallbackProxy.h"
#include "Engine.h"
#include "DungeonsGameSession.h"
#include "OnlineUtil.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/crossplay/CrossplayOSS.h"

UInviteDungeonsSessionCallbackProxy::UInviteDungeonsSessionCallbackProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {

}

UInviteDungeonsSessionCallbackProxy* UInviteDungeonsSessionCallbackProxy::InviteDungeonsFriend(const UObject* WorldContextObject, ABasePlayerController* playerController, const FBlueprintFriend& Friend) {
	UInviteDungeonsSessionCallbackProxy* node = NewObject<UInviteDungeonsSessionCallbackProxy>();
	node->WorldContextObject = WorldContextObject;
	node->PlayerControllerWeakPtr = playerController;	
	node->FriendUniqueId.SetUniqueNetId(Friend.UniqueNetId.GetUniqueNetId());
	return node;
}

void UInviteDungeonsSessionCallbackProxy::Activate() {
	if (!PlayerControllerWeakPtr.IsValid()) {
		InviteSessionFailed.Broadcast(ESessionFailureReason::InvalidPlayerController);
		return;
	}

	auto UserId = online::getCrossplayOss()->GetIdentityIF()->GetUniquePlayerIdForSessionSubsystem(Cast<UDungeonsLocalPlayer>(PlayerControllerWeakPtr->GetLocalPlayer())->GetSystemUserId());
	if (!UserId.IsValid()) {
		InviteSessionFailed.Broadcast(ESessionFailureReason::InvalidUniqueNetworkId);
		return;
	}

	if (!FriendUniqueId.IsValid()) {
		InviteSessionFailed.Broadcast(ESessionFailureReason::InvalidFriendNetId);
		return;
	}

	auto Sessions = online::getSessionInterface();
	if (!Sessions.IsValid()) {
		InviteSessionFailed.Broadcast(ESessionFailureReason::SessionInterfaceUnavailable);
		return;
	}

	if (Sessions->IsPlayerInSession(DungeonsGameSessionName, *FriendUniqueId)) {
		InviteSessionFailed.Broadcast(ESessionFailureReason::Unknown);
		UE_LOG(LogOnline, Log, TEXT("UInviteDungeonsSessionCallbackProxy: Cannot invite a player that is already registered in the session"));
		return;
	}

	FNamedOnlineSession* namedSession = Sessions->GetNamedSession(DungeonsGameSessionName);
	if (namedSession) {
		auto players = namedSession->RegisteredPlayers;
		if (players.Num() >= 4) {
			InviteSessionFailed.Broadcast(ESessionFailureReason::Unknown);
			UE_LOG(LogOnline, Log, TEXT("UInviteDungeonsSessionCallbackProxy: Cannot invite friends to a full session"));
			return;
		}
	}

	if (auto gameMode = WorldContextObject->GetWorld()->GetAuthGameMode()) {
		ADungeonsGameSession* gameSession = Cast<ADungeonsGameSession>(gameMode->GameSession);
		if (gameSession && !gameSession->InvitePlayer(FriendUniqueId)) {
			UE_LOG(LogOnline, Log, TEXT("UInviteDungeonsSessionCallbackProxy: must wait 5 seconds between invites to the same account"));
			return;
		}
	}

	ClearKicked(FriendUniqueId);
	if (Sessions->SendSessionInviteToFriend(*UserId, DungeonsGameSessionName, *FriendUniqueId)) {
		InviteSessionSucceeded.Broadcast();
	} else {
		UE_LOG(LogOnline, Log, TEXT("SendSessionInviteToFriend invite failed."));
		InviteSessionFailed.Broadcast(ESessionFailureReason::SessionNotFound);
	}
}

void UInviteDungeonsSessionCallbackProxy::ClearKicked(FUniqueNetIdWrapper friendId) {
	if (auto gameMode = WorldContextObject->GetWorld()->GetAuthGameMode()) {
		auto* gameSession = Cast<ADungeonsGameSession>(gameMode->GameSession);
		if (gameSession) {
			gameSession->RemoveKick(friendId);
		}
	}
}
