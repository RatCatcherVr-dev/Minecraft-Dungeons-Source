#include "Dungeons.h"
#include "LevelSettings.h"
#include "DungeonsGameInstance.h"
#include "util/StringUtil.h"
#include "merchant/MerchantDefs.h"
#include "component/ItemStashComponent.h"
#include "util/Algo.h"
#include "EndGame/EndGameContentDefs.h"

const FString FLevelSettings::UNREAL_LOBBY_MAP_NAME = "lobby";
const FString FLevelSettings::UNREAL_INGAME_MAP_NAME = "ingame";

FLevelSettings::FLevelSettings(
	const FMissionState& missionState,
	FEmergentDifficulty emergentDifficulty,
	FString unrealMapName,
	FString levelFilename,
	const TArray<FString>& progressionKeys)
	: missionState(missionState)
	, emergentDifficulty(emergentDifficulty)
	, unrealMapName(unrealMapName)
	, levelFilename(levelFilename)
	, progressionKeys(progressionKeys)
{
}

namespace levelsettingsutil {

const TSet<FString> _defaultProgressionKeys = {
	"unlocked-rune-b", // Unlocks SquidCoast rune for rune puzzle as default
};

TArray<FString> getProgressionKeys(UWorld* world) {
	const auto* controller = world && world->GetGameInstance() ?
		world->GetGameInstance()->GetFirstLocalPlayerController() :
		nullptr;

	return getProgressionKeysForPlayerController(Cast<const APlayerControllerBase>(controller));
}

TArray<FString> getProgressionKeysForPlayerController(const APlayerControllerBase * controller) {
	TSet<FString> progressionKeys = _defaultProgressionKeys;

	const auto addKey = [&progressionKeys](FString key) {
		key.ToLowerInline();
		progressionKeys.Add(key);
	};

	if (controller) {
		const auto* saveData = controller->GetSaveData();
		const auto* characterSerialize = controller->GetCharacterSerializeComponent();

		// Add the stored progression keys
		algo::for_each(saveData->mRecordedData.progressionKeys, addKey);

		// Completed levels (progression keys)
		for (const auto& kv : saveData->mRecordedData.progress) {
			if (!kv.second.completedDifficulty) {
				continue;
			}
			const auto levelName = GetEnumValueToStringStripped(kv.first);
			addKey(levelName);

			if (const auto* def = missions::getChecked(kv.first)) {
				for (const auto& keyGetter : def->getMissionGrantUnlockKeys()) {
					const auto key = keyGetter();
					if (!progressionKeys.Contains(key)) {
						addKey(key);
					}
				}
			}

			for (auto completedDifficulty : difficultyquery::inclusiveBetween(difficultyquery::First, kv.second.completedDifficulty.GetValue())) {
				addKey(FString::Format(TEXT("progression-{0}-{1}"), { static_cast<int>(completedDifficulty), levelName }));
			}
		}
		
		for (auto key : endgamecontent::getAllUnlockedProgressKeys(characterSerialize)) {
			addKey(key);
		}

		// Unlocked bonus missions
		for (const auto& bonusMission : saveData->mRecordedData.unlockedSecretMissions) {
			addKey(ULevelSettingsUtil::CreateUnlockKeyForLevel(bonusMission));
		}
		// Unlocked lobby chests
		for (const auto& chest : saveData->mRecordedData.unlockedLobbyChests) {
			addKey(FString::Format(TEXT("lobbychest-{0}"), { chest.first }));
		}
		// (Finished) objective tags
		for (const auto& objectiveTag : saveData->mRecordedData.finishedObjectiveTags) {
			addKey(FString::Format(TEXT("#{0}={1}"), { stringutil::toFString(objectiveTag.first), objectiveTag.second }));
		}

		// Add inventory items
		for (const auto& inventoryItem : saveData->mRecordedData.items) {
			addKey("carried-" + inventoryItem.type.GetBackingType().ToString());
		}

		
		// registration lambda:
		auto addMerchantLevelKey = [&progressionKeys, controller](TSubclassOf<UMerchantDef> staticClass) {
			auto def = staticClass->GetDefaultObject<UMerchantDef>();
			auto unlockKey = def->GetUnlockProgressKey();
			if(progressionKeys.Contains(unlockKey)) {
				if(auto levelKey = def->GetMerchantLevelProgressKey(controller)) {
					progressionKeys.Add(levelKey.GetValue());
				}
			}
		};

		// all merchant types:
		for (auto merchantClass : merchantdefs::getAllEnabledClasses()) {
			addMerchantLevelKey(merchantClass);
		}		
	}
	else {
		ensure(false && "Getting progression keys when there's no player controller is likely not intended.");
	}

	return progressionKeys.Array();
}

TArray<FString> getDefaultProgressionKeys() {
	return _defaultProgressionKeys.Array();
}

FString filenameForLevel(ELevelNames level) {
	if (const auto* mission = missions::getChecked(level)) {
		return mission->levelFilename();
	}
	ensureMsgf(false, TEXT("We shouldn't come here! '%s' has no MissionDef instance!"), *GetEnumValueToStringStripped(level));
	return GetEnumValueToStringStripped(level);
}

FLevelSettings generateLobbySettings(UWorld* world) {
	return FLevelSettings(
		FMissionState(
			FMissionDifficulty{
				ELevelNames::Invalid,
				EGameDifficulty::Invalid,
				EThreatLevel::Invalid,
				{0}
			},
			0
		),
		FEmergentDifficulty(),
		FLevelSettings::UNREAL_LOBBY_MAP_NAME,
		"Lobby",
		getProgressionKeys(world));
}

FLevelSettings generateTutorialSettings(UWorld* world) {
	return FLevelSettings(
		FMissionState(
			FMissionDifficulty{
				ELevelNames::squidcoast,
				EGameDifficulty::Difficulty_1,
				EThreatLevel::Threat_1,
				{0}
			},
			0
		),
		FEmergentDifficulty(),
		FLevelSettings::UNREAL_INGAME_MAP_NAME,
		filenameForLevel(ELevelNames::squidcoast),
		getProgressionKeys(world));
}

FLevelSettings generateMissionSettings(UWorld* world, const FMissionState& missionState) {
	return FLevelSettings(
		missionState,
		FEmergentDifficulty(),
		FLevelSettings::UNREAL_INGAME_MAP_NAME,
		filenameForLevel(missionState.getLevelName()),
		getProgressionKeys(world));
}

FLevelSettings generateDevelopmentMissionSettings(UWorld* world, ELevelNames levelName, FString levelFilename, EGameDifficulty difficulty, EThreatLevel threatLevel, FEndlessStruggle endlessStruggle, FEmergentDifficulty emergentDifficulty, RandomSeed seed) {
	return FLevelSettings(
		FMissionState(
			FMissionDifficulty{
				levelName,
				difficulty,
				threatLevel,
				endlessStruggle,
			},
			seed
		),
		emergentDifficulty,
		FLevelSettings::UNREAL_INGAME_MAP_NAME,
		levelFilename.IsEmpty() ? filenameForLevel(levelName) : levelFilename,
		getProgressionKeys(world));
}

FLevelSettings generateDummyMissionSettings() {
	return FLevelSettings(
		FMissionState(
			FMissionDifficulty{
				ELevelNames::deserttemple,
				difficultyquery::Last,
				threatquery::Last,
				{0},
			},
			0
		),
		{0, 0},
		FLevelSettings::UNREAL_INGAME_MAP_NAME,
		filenameForLevel(ELevelNames::deserttemple),
		{});
}

}

bool ULevelSettingsUtil::IsLobby(const FLevelSettings& LevelSettings) {
	return LevelSettings.isLobby();
}

FText ULevelSettingsUtil::GetLevelDisplayName(const FLevelSettings& LevelSettings) {
	return LevelSettings.getLevelDisplayName();
}

FString ULevelSettingsUtil::CreateUnlockKeyForLevel(ELevelNames levelName) {
	return "unlocked-" + GetEnumValueToStringStripped(levelName);
}

EGameDifficulty ULevelSettingsUtil::getDifficulty(const FLevelSettings& LevelSettings) {
	return LevelSettings.getDifficulty();
}

EThreatLevel ULevelSettingsUtil::getThreatLevel(const FLevelSettings& LevelSettings) {
	return LevelSettings.getThreatLevel();
}

FEndlessStruggle ULevelSettingsUtil::getEndlessStruggle(const FLevelSettings& LevelSettings) {
	return LevelSettings.getEndlessStruggle();
}

ELevelNames ULevelSettingsUtil::getLevelName(const FLevelSettings& LevelSettings) {
	return LevelSettings.getLevelName();
}
