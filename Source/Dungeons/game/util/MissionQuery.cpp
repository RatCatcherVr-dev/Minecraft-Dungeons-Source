#include "Dungeons.h"
#include "MissionQuery.h"
#include "DungeonsGameInstance.h"
#include "game/GameBP.h"

namespace missionquery_internal {

UDungeonsGameInstance* DungeonsGameInstance(UObject* WorldContextObject) {
	return WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
}

}

ELevelNames UMissionQuery::GetLevelName(UObject* WorldContextObject) {
	if (auto* di = missionquery_internal::DungeonsGameInstance(WorldContextObject)) {
		return di->GetLevelSettingsLastStarted().getLevelName();
	}
	return ELevelNames::Invalid;
}

void UMissionQuery::GetPropNamesForTile(UObject* WorldContextObject, const FString& TileName, TArray<FString>& PropNames) {
	auto gameBP = actorquery::getFirstActor<AGameBP>(WorldContextObject->GetWorld());
	if (!gameBP) {
		return;
	}

	auto levelDef = gameBP->GetLevelDef();
	if (!levelDef) {
		return;
	}

	std::string tileName(TCHAR_TO_UTF8(*TileName));
	for (auto& tile : levelDef->levelDef.tiles) {
		//generator::Tile tile;
		if (tile.metaTile.id == tileName) {
			auto original = tile.placeResult.tilePlacement.originalPlacement();
			if (original.IsSet()) {
				for (auto& child : original.GetValue().children()) {
					PropNames.Add(FString(child.metaIdHACK().c_str()));
				}
			}
		}
	}
}

FText UMissionQuery::GetMissionNightName(UObject* WorldContextObject)
{
	auto gameBP = actorquery::getFirstActor<AGameBP>(WorldContextObject->GetWorld());
	if (!gameBP) {
		return FText::GetEmpty();
	}

	return gameBP->GetCurrentMissionNightName();
}

FText UMissionQuery::GetMissionNightAttackName(UObject* WorldContextObject)
{
	auto gameBP = actorquery::getFirstActor<AGameBP>(WorldContextObject->GetWorld());
	if (!gameBP) {
		return FText::GetEmpty();
	}

	return gameBP->GetCurrentMissionNightAttackName();
}
