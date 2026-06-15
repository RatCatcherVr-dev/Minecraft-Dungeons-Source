#include "Dungeons.h"
#include "SessionDisplayEntity.h"
#include "FindSessionsCallbackProxy.h"
#include "SessionSettings.h"
#include "online/ui/OnlineTextLabels.h"
#include "OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"

int32 UDungeonsSessionDisplayEntity::GetPingInMs(const FBlueprintSessionResult& Result) {
	return Result.OnlineResult.PingInMs;
}

int32 UDungeonsSessionDisplayEntity::GetCurrentPlayers(const FBlueprintSessionResult& Result) {
	SessionSettings ss(Result);
	if (!online::getCrossplayOss()->IsDungeonsActive()) {
		return Result.OnlineResult.Session.SessionSettings.NumPublicConnections - Result.OnlineResult.Session.NumOpenPublicConnections;
	}
	else
	{
		if (ss.IsPrivate()) {
			return Result.OnlineResult.Session.SessionSettings.NumPrivateConnections - Result.OnlineResult.Session.NumOpenPrivateConnections;
		}
		else {
			return Result.OnlineResult.Session.SessionSettings.NumPublicConnections - Result.OnlineResult.Session.NumOpenPublicConnections;
		}
	}
}

int32 UDungeonsSessionDisplayEntity::GetMaxPlayers(const FBlueprintSessionResult& Result) {
	return Result.OnlineResult.Session.SessionSettings.NumPublicConnections;
}

FString UDungeonsSessionDisplayEntity::GetServerName(const FBlueprintSessionResult& Result) {
	return Result.OnlineResult.Session.OwningUserName;
}

FString UDungeonsSessionDisplayEntity::GetPlayerName(const FBlueprintSessionResult& Result) {
	if (online::shouldShowSecondaryName(Result.OnlineResult.Session.OwningUserName)) {
		return Result.OnlineResult.Session.OwningUserName;
	}
	else {
		return SessionSettings(Result).GetPlayerName();
	}
}

FText UDungeonsSessionDisplayEntity::GetMapName(const FBlueprintSessionResult& Result) {
	FText levelName = SessionSettings(Result).GetLevelDisplayName();
	if (levelName.IsEmpty()) {
		return online::ui::lobbyName();
	}
	else {
		return levelName;
	}
}

EGameDifficulty UDungeonsSessionDisplayEntity::GetDifficulty(const FBlueprintSessionResult& Result) {
	return SessionSettings(Result).GetLevelSettings().getDifficulty();
}

EThreatLevel UDungeonsSessionDisplayEntity::GetThreatLevel(const FBlueprintSessionResult& Result) {
	return SessionSettings(Result).GetLevelSettings().getThreatLevel();
}
