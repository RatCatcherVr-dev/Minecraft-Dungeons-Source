#include "Dungeons.h"
#include "TrialsConsoleCommands.h"
#include "game/GameBP.h"
#include "game/LobbyBP.h"
#include "TrialsProvider.h"
#include "DungeonsGameInstance.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/mission/trial/TrialUtil.h"
#include "game/mission/trial/MockTrials.h"

namespace trialcommands {

UTrialsProvider* GetTrialsProvider(UWorld* world) {
	auto* dungeonsGameInstance = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	return dungeonsGameInstance->GetMinecraftAPI()->GetTrialsProvider();
}

void LoadTrials(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!world) {
		return;
	}
	auto* lobbyBp = actorquery::getFirstActor<ALobbyBP>(world);
	if (!lobbyBp) {
		out.Log(ELogVerbosity::Error, TEXT("Dungeons.Trials.Load can only be run from the Lobby!"));
		return;
	}

	GetTrialsProvider(world)->LoadTrials();
	lobbyBp->InitTrials();
}
	
std::vector<EGameDifficulty> ArgAsDifficulties(const TArray<FString>& commands, int index) {
	if (ArgAsFString(commands, index) == FString("*")) {
		return difficultyquery::AllDifficulties;
	}
	if (auto difficulty = ArgAsEnum<EGameDifficulty>(commands, index)) {
		return { difficulty.GetValue() };
	}
	return {};
}

void CompleteTrialLocally(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto trialId = ArgAsFString(commands, 0);
	const auto difficulties = ArgAsDifficulties(commands, 1);

	if (!trialId) {
		out.Log(TEXT("No trial id provided"));
		return;
	}
	if (difficulties.empty()) {
		out.Log(TEXT("Can't parse game difficulty"));
		return;
	}
	if (auto* player = GetPlayerCharacter(world, commands)) {
		for (auto& difficulty : difficulties) {
			player->GetCharacterSerializeComponent()->AddTrialCompleted(trialId.GetValue(), difficulty);
		}
	}
}

void ClearTrialLocally(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto trialId = ArgAsFString(commands, 0);
	const auto difficulties = ArgAsDifficulties(commands, 1);

	if (!trialId) {
		out.Log(TEXT("No trial id provided"));
		return;
	}
	if (difficulties.empty()) {
		out.Log(TEXT("Can't parse game difficulty"));
		return;
	}
	if (auto* player = GetPlayerCharacter(world, commands)) {
		for (auto& difficulty : difficulties) {
			player->GetCharacterSerializeComponent()->ClearTrial_DEBUG(trialId.GetValue(), difficulty);
		}
	}
}

void ClearAllLocally(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* player = GetPlayerCharacter(world, commands)) {
		player->GetCharacterSerializeComponent()->ClearTrials_DEBUG();
	}
}


void CreateMockTrialsLocally(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	TOptional<int> count;
	TOptional<int> seed;
	if (commands.Num() > 0) {
		count = ArgAsInt(commands, 0);
	}
	if (commands.Num() > 1) {
		seed = ArgAsInt(commands, 1);
	}
	trial::prepareMissionDefs(trial::mock::generateRandom(seed.Get(0), count));
}


FAutoConsoleCommand LoadTrialsCommand(
	TEXT("Dungeons.Trials.Load"),
	TEXT("Loads all trials for the authenticated user") TEXT("\nUsage: Dungeons.Trials.Load"), 
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&LoadTrials), 
	ECVF_Cheat);

FAutoConsoleCommand CompleteTrialLocallyCommand(
	TEXT("Dungeons.Trials.Complete"),
	TEXT("Complete the specified trial locally") TEXT("\nUsage: Dungeons.Trials.Complete <trial-id> <difficulty>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CompleteTrialLocally),
	ECVF_Cheat);

FAutoConsoleCommand ClearTrialLocallyCommand(
	TEXT("Dungeons.Trials.Clear"),
	TEXT("Clear the specified trial locally") TEXT("\nUsage: Dungeons.Trials.Clear <trial-id> <difficulty or '*' for all difficulties>"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ClearTrialLocally),
	ECVF_Cheat);

FAutoConsoleCommand ClearAllTrialsLocallyCommand(
	TEXT("Dungeons.Trials.ClearAll"),
	TEXT("Clear all trials locally") TEXT("\nUsage: Dungeons.Trials.ClearAll"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ClearAllLocally),
	ECVF_Cheat);

FAutoConsoleCommand CreateMockTrialsTrialsLocallyCommand(
	TEXT("Dungeons.Trials.CreateMockTrials"),
	TEXT("Show all trials locally") TEXT("\nUsage: Dungeons.Trials.CreateMockTrials [count] [randomseed]"),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CreateMockTrialsLocally),
	ECVF_Cheat);

}
