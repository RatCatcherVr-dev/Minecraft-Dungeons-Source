#include "Dungeons.h"
#include "actor/character/player/InventoryConsoleCommands.h"
#include "lovika/LovikaLevelActor.h"
#include "util/Algo.hpp"
#include "util/CollectionUtils.h"
#include "util/ConsoleCommandHelpers.h"
#include <HAL/ConsoleManager.h>

static void DoRunIniCommands(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 0) {
		out.Log(TEXT("Error: Need section name passed as 1st parameter!"));
		return;
	}

	TArray<FString> commands;
	if (!GConfig->GetArray(*args[0], TEXT("Commands"), commands, GGameIni)) {
		GLog->Log(FString::Printf(TEXT("No commands found for setup: %s"), *args[0]));
		return;
	}

	for (const auto& cmd : commands) {
		FConsoleManager::Get().ProcessUserConsoleInput(*cmd, out, world);
	}
}

static const FAutoConsoleCommand RunIniCommandsCommand(TEXT("Dungeons.Command.RunIni")
	, TEXT("Run commands from the DefaultGame ini file")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoRunIniCommands)
	, ECVF_Cheat);


static void ProcessDelimiterSeparatedCommands(const FString& s, const FString& delimiter, UWorld* world, FOutputDevice& out) {
	TArray<FString> commands;
	s.ParseIntoArray(commands, *delimiter, true);

	for (const auto& cmd : commands) {
		FConsoleManager::Get().ProcessUserConsoleInput(*cmd.TrimStartAndEnd(), out, world);
	}
}

static void DoRunCsvCommands(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 0) {
		out.Log(TEXT("Not enough arguments passed."));
		return;
	}
	ProcessDelimiterSeparatedCommands(FString::Join(args, TEXT(" ")), ",", world, out);
}

static const FAutoConsoleCommand RunCsvCommandsCommand(TEXT("Dungeons.Command.RunCsv")
	, TEXT("Run commands from a comma separated list of console commands")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoRunCsvCommands)
	, ECVF_Cheat);


static void DoRunDelimiterSeparatedCommands(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 1) {
		out.Log(TEXT("Not enough arguments passed."));
		return;
	}
	const auto delimiter = args[0];
	const auto joinedCommands = FString::Join(Util::subsequence(args, 1), TEXT(" "));
	ProcessDelimiterSeparatedCommands(joinedCommands, delimiter, world, out);
}

static const FAutoConsoleCommand RunSvCommandsCommand(TEXT("Dungeons.Command.RunSv")
	, TEXT("Run commands from a delimiter separated list of console commands: Dungeons.Command.RunSv delimiter commands+")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoRunDelimiterSeparatedCommands)
	, ECVF_Cheat);


FString GenerateRunCommandsString(const TArray<FString>& commands) {
	if (algo::none_of(commands, RETLAMBDA(it.Contains(",")))) {
		return "Dungeons.Command.RunCsv " + FString::Join(algo::copy_if(commands, RETLAMBDA(!it.IsEmpty())), TEXT(", "));
	}
	const FString separator(" && "); // @todo?: while (containsDelimiter) updateDelimiter;
	return "Dungeons.Command.RunSv" + separator + FString::Join(algo::copy_if(commands, RETLAMBDA(!it.IsEmpty())), *separator);
}

FAutoConsoleCommand MakeRedirectCommand(const FString& cmd, const FString& help, const FString& redirectedToCmd, EConsoleVariableFlags flags, bool appendCmdToHelp) {
	const auto helpText = appendCmdToHelp ?
		help + "\nThis is identical to calling:\n" + redirectedToCmd:
		help;

	const auto callRedirectCommand = [trimmedCmd = redirectedToCmd.TrimStartAndEnd()] (const TArray<FString>&, UWorld* world, FOutputDevice& out) {
		FConsoleManager::Get().ProcessUserConsoleInput(*trimmedCmd, out, world);
	};

	return FAutoConsoleCommand(
		*cmd,
		*helpText,
		FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateLambda(callRedirectCommand),
		flags
	);
}

TOptional<FString> GenerateStartLevelCommand(UWorld* world, FOutputDevice& errorOut) {
	const auto* level = actorquery::getFirstActor<ALovikaLevelActor>(world);
	if (!level) {
		errorOut.Log(TEXT("No level actor found. Are you really playing a mission?"));
		return {};
	}
	const auto levelSettings = level->getGeneratedLevelSettings();
	if (!levelSettings) {
		errorOut.Log(TEXT("No 'generated FLevelSettings' found. Who knows what's wrong."));
		return {};
	}
	return "Dungeons.Level.Start " +
		GetEnumValueToStringStripped(levelSettings->getLevelName()) + " " +
		FString::FromInt(static_cast<int>(levelSettings->getDifficulty())) + " " +
		FString::FromInt(static_cast<int>(levelSettings->getThreatLevel())) + " " +
		FString::FromInt(static_cast<int>(levelSettings->getSeed()));
}

static void DoEmitMission(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (auto command = GenerateStartLevelCommand(world, out)) {
		out.Log(command.GetValue());
	}
}

static void DoEmitMissionSetup(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto* inventory = GetPlayerComponent<UItemStashComponent>(world, args);
	if (!inventory) {
		out.Log("Can't find player controller or an UItemStashComponent on it");
		return;
	}
	auto startLevelCommand = GenerateStartLevelCommand(world, out);
	if (!startLevelCommand) {
		return;
	}
	TArray<FString> commands;
	commands.Append(EmitInventoryCommands(*inventory));
	commands.Add(startLevelCommand.GetValue());
	out.Log(GenerateRunCommandsString(commands));
}

// @todo: Possibly move this to a fitting file after the "new character save"
//        refactor is completed to not get lots of conflict resolving.
static const FAutoConsoleCommand EmitMissionCommand(TEXT("Dungeons.Command.EmitMission")
	, TEXT("Emits a command re-generating this mission")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoEmitMission)
	, ECVF_Cheat);

static const FAutoConsoleCommand EmitMissionSetupCommand(TEXT("Dungeons.Command.EmitMissionSetup")
	, TEXT("Emits a command re-generating this mission and equipment")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoEmitMissionSetup)
	, ECVF_Cheat);
