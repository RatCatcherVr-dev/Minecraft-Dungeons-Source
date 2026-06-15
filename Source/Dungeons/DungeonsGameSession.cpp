#include "Dungeons.h"
#include "DungeonsGameSession.h"
#include "game/mission/MissionDefs.h"
#include "game/util/ActorQuery.h"
#include "lovika/LovikaLevelActor.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "game/actor/character/player/BasePlayerController.h"
#include "online/reconnect/ReconnectUtil.h"
#include "game/component/ReconnectComponent.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"

ADungeonsGameSession::InviteInfo::InviteInfo(const FUniqueNetIdWrapper& netId, int64 timestamp) :
	NetId(netId), InviteTime(timestamp) {
}

bool ADungeonsGameSession::IsKicked(const FString& guid) {
	return GetGameInstance<UDungeonsGameInstance>()->GetReconnectStates().IsKicked(guid);
}

FString ADungeonsGameSession::ApproveLogin(const FString& Options) {
	const auto& guid = UGameplayStatics::ParseOption(Options, "guid");
	const FString& inviteId = UGameplayStatics::ParseOption(Options, "inviteId");
	if (IsKicked(guid)) {
		return "ClientKicked";
	}

	if (GetWorld()->WorldType == EWorldType::PIE) {
		return Super::ApproveLogin(Options);
	}

	if (!Options.Contains("listen")) {
		const auto levelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
		if (levelName != "Lobby") {
			if (const auto levelActor = actorquery::getFirstActor<ALovikaLevelActor>(GetWorld())) {
				auto levelSettings = levelActor->getGeneratedLevelSettings();

				//D11.KS - Networked players inform of us of the splitscreen count, this is a good way of determining whether a local player is trying to join on the hosts end or not.
				const int splitscreenCount = UGameplayStatics::GetIntOption(Options, "SplitscreenCount", -1);

				if (splitscreenCount > 0 && levelSettings.IsSet() && UMissionDefs::IsTutorial(levelSettings.GetValue().getLevelName()))
				{
					const FString errorMsg = "Server is in tutorial, unable to join";
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, errorMsg);
					return errorMsg;
				}
			}
		}
	}

	return Super::ApproveLogin(Options);
}

bool ADungeonsGameSession::KickPlayer(APlayerController* KickedPlayer, const FText& KickReason) {
	if (!Super::KickPlayer(KickedPlayer, KickReason) || !KickedPlayer->PlayerState) {
		return false;
	}

	GetGameInstance<UDungeonsGameInstance>()->GetReconnectStates().KickPlayer(KickedPlayer->PlayerState->UniqueId->ToString());
	return true;
}

void ADungeonsGameSession::RemoveKick(const FUniqueNetIdWrapper& netId) {
	GetGameInstance<UDungeonsGameInstance>()->GetReconnectStates().RemoveKick(netId);
}

bool ADungeonsGameSession::InvitePlayer(const FUniqueNetIdWrapper& netId) {
	InviteInfo* invite = Invites.FindByPredicate([netId](const InviteInfo& invite)->bool {return invite.NetId == netId; });

	if (invite) {
		if (FApp::GetCurrentTime() - invite->InviteTime < 5) {
			return false;
		}
		else {
			invite->InviteTime = FApp::GetCurrentTime();
		}
	}
	else {
		Invites.Add(InviteInfo(netId, FApp::GetCurrentTime()));
	}
	return true;
}

void ADungeonsGameSession::RegisterPlayer(APlayerController* NewPlayer, const TSharedPtr<const FUniqueNetId>& UniqueId, bool bWasFromInvite) {
	if (NewPlayer != nullptr) {
		// Set the player's ID.
		check(NewPlayer->PlayerState);
		NewPlayer->PlayerState->PlayerId = GetNextPlayerID();
		NewPlayer->PlayerState->SetUniqueId(UniqueId);
	}

	if(online::getCrossplayOss()->GetSessionIF()->GetNamedSession(DungeonsGameSessionName) && UniqueId) {
		online::getCrossplayOss()->GetSessionIF()->RegisterPlayer(DungeonsGameSessionName, *UniqueId, bWasFromInvite);
	}
}

void ADungeonsGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) {
	if (online::getCrossplayOss()->GetSessionIF()->GetNamedSession(DungeonsGameSessionName)) {
		online::getCrossplayOss()->GetSessionIF()->UnregisterPlayer(InSessionName, *UniqueId);
	}
}
