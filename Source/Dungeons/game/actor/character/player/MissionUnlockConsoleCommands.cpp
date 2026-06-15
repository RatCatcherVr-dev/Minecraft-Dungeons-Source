#include "Dungeons.h"
#include "game/component/MissionProgressComponent.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"
#include "DungeonsGameState.h"
#include "DungeonsGameInstance.h"
#include "game/component/CharacterSerializeComponent.h"

namespace missionconsolehandlers {

UMissionProgressComponent* GetProgressComponent(UWorld* world, const TArray<FString>& args) {
	if (APlayerCharacter* character = GetPlayerCharacter(world, args)) {
		if (UMissionProgressComponent* validProgressComponent = character->FindComponentByClass<UMissionProgressComponent>()) {
			return validProgressComponent;
		}
	}
	return nullptr;
}

void unlockMissionsUpTo(UCharacterSaveData::MissionProgressMap& progress, ELevelNames level, FMissionDifficulty unlockDifficulty) {
	const auto unlockMission = missions::get(level);
	for (const auto& mission : unlockMission.getRequiredMissions()) {
		unlockMissionsUpTo(progress, mission, unlockDifficulty);
	}

	auto threatLevelToSet = Math::max(unlockDifficulty.threatLevel, unlockMission.getMinimalThreatLevel().Get(threatquery::Default));
	if (progress.find(level) != progress.end()) {
		progress[level].completedDifficulty = Math::max(progress[level].completedDifficulty.Get(unlockDifficulty.difficulty), unlockDifficulty.difficulty);
		progress[level].completedThreatLevel = Math::max(progress[level].completedThreatLevel.Get(threatLevelToSet), threatLevelToSet);
		progress[level].completedEndlessStruggle.Value = Math::max(progress[level].completedEndlessStruggle.Value, unlockDifficulty.endlessStruggle.Value);
	} else {
		progress[level] = { unlockDifficulty.difficulty, threatLevelToSet, unlockDifficulty.endlessStruggle };
	}
}

void Unlock(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto missionDifficulty = parseMissionDifficulty(args);
	if (!missionDifficulty) {
		out.Log(TEXT("Three arguments required, e.g: ") MISSION_DIFFICULTY_ARG_EXAMPLE);
		return;
	}
	auto progressComponent = GetProgressComponent(world, args);
	if (!progressComponent) {
		return;
	}

	UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	if (!gameInstance) {
		return;
	}

	APlayerCharacter* character = GetPlayerCharacter(world, args);
	UCharacterSerializeComponent* serializeComponent = character->GetCharacterSerializeComponent();
	if (!serializeComponent) {
		return;
	}

	//Unlock difficulty
	serializeComponent->SetDifficulties({ missionDifficulty->difficulty, missionDifficulty->difficulty });

	auto& progress = serializeComponent->GetProgress();
	progress.clear();

	// add squidcoast (because its not a requirement for any map, and its will keep being played for a starting player.)
	progress[ELevelNames::squidcoast] = { EGameDifficulty::Difficulty_1, missions::get(ELevelNames::squidcoast).getMinimalThreatLevel().Get(EThreatLevel::Threat_1) };

	TOptional<EThreatLevel> threatLevelToUnlock = {};

	//As a baseline, we assume that we have played the previous threat level.
	if (auto previousThreatLevel = threatquery::getPreviousThreatLevel(missionDifficulty->threatLevel)) {
		serializeComponent->SetThreatLevelProgress({ previousThreatLevel.GetValue(), previousThreatLevel.GetValue() });
	}

	game::FDifficulty unlockUpToDifficulty = difficultyquery::getPreviousDifficulty(missionDifficulty.GetValue()).Get(missionDifficulty.GetValue());
	auto previousGameDifficulty = difficultyquery::getPreviousDifficulty(missionDifficulty->difficulty);

	auto unlockMissions = [&threatLevelToUnlock, &unlockUpToDifficulty, &progress, &missionDifficulty](ELevelNames level, EGameDifficulty gamedifficulty) {
		auto requiredThreatLevel = Math::max(unlockUpToDifficulty.threatLevel(), missions::get(level).getMinimalThreatLevel().Get(threatquery::First));
		threatLevelToUnlock = Math::max(threatLevelToUnlock.Get(threatquery::First), requiredThreatLevel);
		unlockMissionsUpTo(progress, level, { level, gamedifficulty, requiredThreatLevel, unlockUpToDifficulty.endlessStruggle() });
	};


	//If we are trying to unlock a difficulty above normal, assume that we have played all maps to allow vendors to accurately represent all loot.
	if (previousGameDifficulty.IsSet()) {
		for (const auto* mission : missions::getAllMatching(RETLAMBDA(it.isValidDifficulty(previousGameDifficulty.GetValue())))) {
			unlockMissions(mission->level(), previousGameDifficulty.GetValue());			
		}
	}

	//Also unlock more completed missions to simulate playing through higher difficulties
	while (previousGameDifficulty.IsSet()) {
		//complete the threat level required to unlock this difficulty at least.
		unlockMissions(ELevelNames::obsidianpinnacle, previousGameDifficulty.GetValue());		
		previousGameDifficulty = difficultyquery::getPreviousDifficulty(previousGameDifficulty.GetValue());
	}

	//Unlock only required missions at lowest possible threat - don't complete the actual mission requested.
	const auto unlockMission = missions::get(missionDifficulty->mission);
	for (const auto& mission : unlockMission.getRequiredMissions()) {
		unlockMissions(mission, unlockUpToDifficulty.chosen());
	}

	//Claim threatLevel progress if any was set.
	if (threatLevelToUnlock.IsSet()) {
		auto unlocked = threatquery::getUnlockedForCompleted(threatLevelToUnlock.GetValue());
		serializeComponent->SetThreatLevelProgress({ unlocked, unlocked });
	}

	progressComponent->DeserializeSaveState();

	//Generate struggle tier unlocks after deserialize since it operates on the progress component directly.
	if (unlockUpToDifficulty.chosen() >= difficultyquery::RequiredForEndlessStuggle && unlockUpToDifficulty.threatLevel() >= threatquery::RequiredForEndlessStuggle) {
		if (auto endlessStruggletier = endlesstruggle::tier::getUnlockFor(missionDifficulty->endlessStruggle)) {
			endlessStruggletier->GenerateRequiredProgress(progressComponent);
		}
	}
}

void UnlockAll(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
#if !UE_BUILD_SHIPPING
	if (auto progress = GetProgressComponent(world, args)) {
		progress->UnlockAllMissions(true);
	}
#endif
}

void SetLives(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	//if (world->HasAuthority())... out.Log(TEXT("Only server can set number of lives left!");
	if (args.Num() != 1 || !args[0].IsNumeric()) {
		out.Log(TEXT("Need number of lives left (Numeric) as argument!"));
		return;
	}

	int lives = FCString::Atoi(*args[0]);
	if(ADungeonsGameState* gamestate = world->GetGameState<ADungeonsGameState>())
		gamestate->SetLives(lives);

}

}

static const FAutoConsoleCommand MissionUnlock(TEXT("Dungeons.Mission.Unlock")
	, TEXT("Unlock the given mission, difficulty (1-5), and threat level, e.g: " MISSION_DIFFICULTY_ARG_EXAMPLE)
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&missionconsolehandlers::Unlock)
	, ECVF_Cheat);

static const FAutoConsoleCommand SetLives(TEXT("Dungeons.Mission.SetLives")
	, TEXT("Set number of lives left (0+) ")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&missionconsolehandlers::SetLives)
	, ECVF_Cheat);

// Note: I know this already exists in Dungeons.Player.UnlockAllMissions, but it was basically faster to rewrite it here
static const FAutoConsoleCommand MissionUnlockAll(TEXT("Dungeons.Mission.UnlockAll")
	, TEXT("Unlock all missions")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&missionconsolehandlers::UnlockAll)
	, ECVF_Cheat);
