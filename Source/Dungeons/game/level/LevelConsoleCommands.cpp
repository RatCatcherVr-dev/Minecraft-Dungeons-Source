#include "Dungeons.h"
#include "game/actor/character/player/InventoryConsoleCommands.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "DungeonsDefsMinimal.h"
#include "DungeonsGameInstance.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "game/mission/state/MissionState.h"
#include "game/LevelSettings.h"
#include "game/dlc/DLCDefs.h"

namespace levelcommands {

void startLevel(UWorld& world, EMapLoadType mapLoadType, const FLevelSettings& levelSettings) {
	if (auto gameInstance = world.GetGameInstance<UDungeonsGameInstance>()) {
		gameInstance->BeginLoadingScreenWithTravel(levelSettings, mapLoadType, UDungeonsGameInstance::fDefaultFadeOutTime, UDungeonsGameInstance::fDefaultFadeInTime);
	}
}

TOptional<TArray<FItemId>> parseItemIds(const TArray<FString>& args, int startIndex, bool invalidIfUnparsable, FOutputDevice* log = nullptr) {
	TArray<FItemId> out;

	for (int index = startIndex;; index++) {
		if (auto item = ArgAsItemId(args, index)) {
			out.Add(item.GetValue());
		}
		else if (args.IsValidIndex(index) && invalidIfUnparsable) {
			if (log) { log->Logf(ELogVerbosity::Error, TEXT("Incorrect item type: %s"), *args[index]); }
			return {};
		}
		else {
			break;
		}
	}
	return out;
}

void DoStartHyperLevel(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	int index = 0;

	const auto difficulty  = ArgAsEnum<EGameDifficulty>(args, index++);
	const auto threatLevel = ArgAsEnum<EThreatLevel>(args, index++);
	const auto seedAndPowerLevel = ArgAsInt(args, index++);
	const auto sacrificedTotalEnchantmentPoints = ArgAsInt(args, index++);
	const auto itemIds = parseItemIds(args, index, true, &out);

	if (!(difficulty && threatLevel && seedAndPowerLevel && sacrificedTotalEnchantmentPoints && itemIds)) {
		out.Logf(ELogVerbosity::Error, TEXT("Usage: difficulty threatlevel seed-powerlevel enchantment-points [item]{0,4}"));
		return;
	}

	const int32 seed = seedAndPowerLevel.GetValue();
	const int points = sacrificedTotalEnchantmentPoints.GetValue();
	const TArray<FInventoryItemData> offeredItems = algo::map_cast<TArray<FInventoryItemData>>(itemIds.GetValue());

	FLevelSettings levelSettings = levelsettingsutil::generateMissionSettings(world,
		FMissionState(
			FMissionDifficulty{
				ELevelNames::netherhypermission,
				difficulty.GetValue(),
				threatLevel.GetValue(),
				{0}
			},
			seed,
			offeredItems,
			points,
			dlc::getAllEnabled()
		)
	);

	startLevel(*world, EMapLoadType::OpenIngame, levelSettings);
}

static const FAutoConsoleCommand StartHyperMissionCommand(TEXT("Dungeons.Level.StartHyperMission")
	, TEXT("Start [the] hypermission with a given difficulty threatlevel seed sacrificedenchantmentpoints [itemid]*")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoStartHyperLevel)
	, ECVF_Cheat);

}
