#pragma once

#include "OnlineUtil.h"
#include "online/OnlineCommon.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/difficulty/EndlessStruggle.h"
#include "game/LevelSettings.h"

struct FBlueprintSessionResult;

class SessionSettings {
public:
	SessionSettings();
	SessionSettings(const FOnlineSessionSettings&);
	SessionSettings(const FOnlineSessionSearchResult&);
	SessionSettings(const FBlueprintSessionResult&);
	SessionSettings(const FString& localPlayerName, const FLevelSettings&, bool isPrivate, bool isCrossplayEnabled);
	
	bool Update(const FLevelSettings& levelSettings, const TArray<FString>& reconnectableGuids);
	void SetPrivate(bool isPrivate);
	void SetLanMatch(bool isLan);

	FLevelSettings GetLevelSettings() const;
	FString GetGameVersion() const;
	FString GetPlayerName() const;

	bool IsPrivate() const;
	bool CanReconnect(const FString& guid) const;
	TArray<sessionSettings::FPlayerNames> GetPlayerNames() const;

	const FOnlineSessionSettings& Get() const;
	FOnlineSessionSettings& Get();

	ELevelNames GetLevelName() const;
	FString GetUnrealMapName() const;
	FText GetLevelDisplayName() const;
	
private:
	EGameDifficulty GetDifficulty() const;
	FString GetThreatLevelStr() const;
	EThreatLevel GetThreatLevel() const;
	FString GetEndlessStruggleStr() const;
	FEndlessStruggle GetEndlessStruggle() const;
	FString GetEmergentDifficultyRaidDifficultyStr() const;
	FString GetEmergentDifficultyAffectorsNumStr() const;
	FEmergentDifficulty GetEmergentDifficulty() const;
	FString GetLevelNameStr() const;
	
	FString GetLevelFilename() const;
	RandomSeed GetRandomSeed() const;

	TOptional<FString> Get(const FName& key) const;

	FOnlineSessionSettings Settings;
};


namespace online {
	SessionSettings createSessionSettings(UWorld*, const FLevelSettings&, bool isPrivate, bool isCrossplayEnabled);
}
