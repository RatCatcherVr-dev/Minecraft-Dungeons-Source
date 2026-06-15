#pragma once

#include "CommonTypes.h"
#include "GameSettings.h"
#include "levels.h"
#include "difficulty/ThreatLevel.h"
#include "difficulty/Difficulty.h"
#include "mission/MissionDefs.h"
#include "mission/state/MissionState.h"
#include "mission/MissionDef.h"
#include <UnrealString.h>
#include "difficulty/ExtraChallenge.h"
#include "difficulty/EndlessStruggle.h"
#include "online/ui/OnlineTextLabels.h"
#include "LevelSettings.generated.h"

class APlayerControllerBase;

USTRUCT(BlueprintType)
struct DUNGEONS_API FLevelSettings {
	static const FString UNREAL_LOBBY_MAP_NAME;
	static const FString UNREAL_INGAME_MAP_NAME;

	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FMissionState missionState;

	UPROPERTY(BlueprintReadOnly)
	FEmergentDifficulty emergentDifficulty;

	UPROPERTY(BlueprintReadOnly)
	FString unrealMapName = "";

	UPROPERTY(BlueprintReadOnly)
	FString levelFilename = "";

	FLevelSettings() = default;
	FLevelSettings(const FMissionState& missionState, FEmergentDifficulty, FString unrealMapName, FString levelFilename, const TArray<FString>& progressionKeys);

	EGameDifficulty getDifficulty() const {
		return missionState.getDifficulty();
	}

	EThreatLevel getThreatLevel() const {
		return missionState.getThreatLevel();
	}

	FEndlessStruggle getEndlessStruggle() const {
		return missionState.getEndlessStruggle();
	}

	FEmergentDifficulty getEmergentDifficulty() const {
		return emergentDifficulty;
	}

	ELevelNames getLevelName() const {
		return missionState.getLevelName();
	}

	EExtraChallenge getExtraChallenge() const {
		return missions::get(getLevelName()).getExtraChallenge();
	}

	bool isLobby() const {
		return unrealMapName == UNREAL_LOBBY_MAP_NAME;
	}

	RandomSeed getSeed(bool isSession = false) const {
		return missionState.getSeed();
	}

	FText getLevelDisplayName() const {
		if (isLobby()) {
			return online::ui::lobbyName();
		}

		return missions::get(getLevelName()).getNameText();
	}

	FString getLevelFilename() const {
		return levelFilename;
	}

	FString getUnrealMapName() const {
		return unrealMapName;
	}

	const TArray<FString>& getProgressionKeys() const {
		return progressionKeys;
	}

	bool IsSameLevel(const FLevelSettings& other) const {
		return other.getUnrealMapName() == getUnrealMapName() &&
			other.getLevelFilename() == getLevelFilename() &&
			other.getLevelName() == getLevelName() &&
			other.getSeed() == getSeed() &&
			other.getDifficulty() == getDifficulty() &&
			other.getThreatLevel() == getThreatLevel();
	}

	game::FDifficulty makeGameDifficulty() const {
		return game::FDifficulty(getDifficulty(), getThreatLevel(), getExtraChallenge(), getEndlessStruggle());
	}

	game::Settings makeGameSettings() const {
		return game::Settings(makeGameDifficulty(), getSeed(), getLevelName());
	}

	void setMissionDifficulty(const FMissionDifficulty& missionDifficulty) {
		missionState.missionDifficulty = missionDifficulty;
	}

	void setSeed(RandomSeed seed) {
		missionState.seed = static_cast<int32>(seed);
	}

private:
	UPROPERTY()
	TArray<FString> progressionKeys;
};

namespace levelsettingsutil {
	FLevelSettings generateLobbySettings(UWorld*);
	FLevelSettings generateTutorialSettings(UWorld*);
	FLevelSettings generateMissionSettings(UWorld*, const FMissionState&);
	FLevelSettings generateDevelopmentMissionSettings(UWorld*, ELevelNames, FString levelFilename, EGameDifficulty, EThreatLevel, FEndlessStruggle, FEmergentDifficulty, RandomSeed);
	FLevelSettings generateDummyMissionSettings();

	TArray<FString> getProgressionKeys(UWorld*);
	TArray<FString> getProgressionKeysForPlayerController(const APlayerControllerBase*);
	TArray<FString> getDefaultProgressionKeys();
};

UCLASS(BlueprintType)
class ULevelSettingsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsLobby(const FLevelSettings& LevelSettings);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetLevelDisplayName(const FLevelSettings& LevelSettings);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString CreateUnlockKeyForLevel(ELevelNames levelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EGameDifficulty getDifficulty(const FLevelSettings& LevelSettings);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EThreatLevel getThreatLevel(const FLevelSettings& LevelSettings);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FEndlessStruggle getEndlessStruggle(const FLevelSettings& LevelSettings);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ELevelNames getLevelName(const FLevelSettings& LevelSettings);
};
