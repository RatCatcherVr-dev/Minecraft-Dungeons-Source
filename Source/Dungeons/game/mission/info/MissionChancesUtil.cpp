#include "Dungeons.h"
#include <GameFramework/Actor.h>
#include "util/Algo.hpp"
#include "MissionChancesUtil.h"
#include "DungeonsGameInstance.h"
#include "data/MobIconLibrary.h"
#include "data/MobIconRow.h"
#include "game/mission/hyper/HyperMissions.h"
#include "game/mob/MobTypeDefs.h"
#include "game/dlc/DLCDefs.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/component/ItemStashComponent.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "MissionChances.h"

FText UMissionChancesUtil::GetMissionChancesTypeText(EMissionChancesType chancesType) {
	switch (chancesType) {
	case EMissionChancesType::Gear:
		return NSLOCTEXT("", "Gear_Drops_Label", "GEAR DROPS");
	case EMissionChancesType::Artifacts:
		return NSLOCTEXT("", "Artifacts_LAbel", "ARTIFACTS");
	case EMissionChancesType::Rewards:
		return NSLOCTEXT("Trials", "rewards", "Rewards");
	case EMissionChancesType::Ancients:
		return NSLOCTEXT("MissionChances", "Ancients", "Ancients");
	default:
		checkNoEntry();
		return FText::GetEmpty();
	}
}

FText UMissionChancesUtil::GetMissionChancesText(const FMissionChances& chances) {
	return GetMissionChancesTypeText(chances.missionChancesType);
}

TArray<EMissionChancesType> UMissionChancesUtil::GetMissionChancesTypes(ELevelNames LevelName) {
	auto& def = missions::get(LevelName);
	if (def.isHyperMission()) {
		return { EMissionChancesType::Ancients };
	}
	else if (def.isTrial()) {
		return { EMissionChancesType::Gear, EMissionChancesType::Rewards };
	}
	else {
		return { EMissionChancesType::Gear, EMissionChancesType::Artifacts };
	}
}

TArray<EMissionChancesType> UMissionChancesUtil::GetMissionChancesTypesFiltered(ELevelNames LevelName, TArray<EMissionChancesType> AllowedTypes) {
	return algo::copy_if(GetMissionChancesTypes(LevelName), RETLAMBDA(AllowedTypes.Contains(it)));
}

FMissionChances UMissionChancesUtil::GetMissionChances(const FMissionState& missionState, EMissionChancesType chancesType) {
	FMissionChances chances = { chancesType };
	switch (chancesType) {
	case EMissionChancesType::Gear:
		chances.typeChances = GetMapLootItemTypeChances(missionState.missionDifficulty);
		break;
	case EMissionChancesType::Artifacts:
		chances.typeChances = GetMapArtifactItemTypeChances(missionState.missionDifficulty);
		break;
	case EMissionChancesType::Rewards:
		chances.typeChances = GetMapRewardItemTypeChances(missionState.missionDifficulty);
		chances.tagChances = GetMapRewardItemTagChances(missionState.getLevelName());
		chances.rarityChances = GetMapRewardItemRarityChances(missionState.getLevelName());
		break;
	case EMissionChancesType::Ancients:		
		chances.mobChances = GetMapUIMobChances(missionState.getLevelName(), missionState.getEligibleDLCs(), missionState.getItemArchetypeCounts());		
		break;
	default:
		checkNoEntry();
		break;
	}
	return chances;
}

TArray<FMissionProbability> UMissionChancesUtil::GetMissionProbabilities(const FMissionState& missionState) {
	return GetMapProbabilities(missionState);
}

TSubclassOf<UChanceInfoIconWidget> UMissionChancesUtil::GetMobChanceIconClass(UObject* WorldContextObject, const FMissionMobChance& mobChance) {
	const auto& mobdef = mob::type::get(mobChance.mob);
	if (auto gi = WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>()){
		UMobIconLibrary* library = gi->GetMobIconLibrary();
		if (auto row = library->findRow(mobdef.GetIconRowName())) {
			if (!row->MobChanceInfoIcon.IsNull()) {
				auto loadedClass = row->MobChanceInfoIcon.Get();
				return TSubclassOf<UChanceInfoIconWidget>(loadedClass ? loadedClass : row->MobChanceInfoIcon.LoadSynchronous());
			}
		}
	}
	return nullptr;
}

const FText& UMissionChancesUtil::GetMobChanceText(const FMissionMobChance& mobChance) {
	return mob::type::getMobDisplayName(mobChance.mob);
}

FText UMissionChancesUtil::GetMobChanceDLCText(const FMissionMobChance& mobChance) {
	return FText::Format(NSLOCTEXT("", "dlc_mobchancetext", "Part of the {DLCName} DLC"), UDLCDefs::GetDLCNameText(mobChance.missingDLC.GetValue()));
}

bool UMissionChancesUtil::GetCanShowItemType(const FSerializableItemId& itemId, AActor* progressOwner) {
	if (progressOwner) {
		if (const auto* itemStash = progressOwner->FindComponentByClass<UItemStashComponent>()) {
			if (itemStash->HasPreviouslyFoundItem(itemId) || itemStash->HasPreviouslyFoundCurrency(itemId)) {
				return true;
			}
		}
	}
	return false;
}

EMobStatus UMissionChancesUtil::GetMobStatus(const FMissionMobChance& mobChance, AActor* progressOwner) {
	if (mobChance.missingDLC) {
		return EMobStatus::NotOwningDLC;
	}
	if (progressOwner) {
		if (const auto* characterserialize = progressOwner->FindComponentByClass<UCharacterSerializeComponent>()) {
			if (characterserialize->ReadMobKillsForType(mobChance.mob) > 0) {
				return EMobStatus::Defeated;
			}
		}
	}
	return EMobStatus::NotDefeated;
}

namespace internal {
	TArray<FMissionItemTypeChance> GetPossibleItemTypesChances(const FMissionDifficulty& missionDifficulty, const itemgen::Config& config) {
		//Some assumptions here about how loot generated in a mission... Why are we showing this config?
		//Get configs from level settings data?
		const auto settings = game::Settings(missionDifficulty, 0, missionDifficulty.mission);
		const auto possibleTypes = game::item::generator::getPossibleItemTypeIds(settings, itemgen::looters::Unknown(), config);
		return algo::map_tarray(possibleTypes, RETLAMBDA((FMissionItemTypeChance(it, EItemRarity::Common))));
	}
}

TArray<FMissionItemTypeChance> UMissionChancesUtil::GetMapLootItemTypeChances(const FMissionDifficulty& missionDifficulty) {
	if (missions::get(missionDifficulty.mission).areDropsEnabled()) {
		return internal::GetPossibleItemTypesChances(missionDifficulty, itemgen::configs::MissionInspectorGear());
	}
	return {};
}

TArray<FMissionItemTypeChance> UMissionChancesUtil::GetMapArtifactItemTypeChances(const FMissionDifficulty& missionDifficulty) {
	return internal::GetPossibleItemTypesChances(missionDifficulty, itemgen::configs::MissionInspectorItem());
}

TArray<FMissionItemTypeChance> UMissionChancesUtil::GetMapRewardItemTypeChances(const FMissionDifficulty& missionDifficulty) {
	return missions::get(missionDifficulty.mission).rewards().getPossibleItemRarityTypes(missionDifficulty);
}

TArray<FMissionItemTagChance> UMissionChancesUtil::GetMapRewardItemTagChances(ELevelNames levelName) {
	return algo::copy_if_map_tarray(missions::get(levelName).rewards().itemChances(),
		RETLAMBDA(it.tagChance),
		RETLAMBDA(it.tagChance.GetValue())
	);
}

TArray<FMissionItemRarityChance> UMissionChancesUtil::GetMapRewardItemRarityChances(ELevelNames levelName) {
	return algo::copy_if_map_tarray(missions::get(levelName).rewards().itemChances(),
		RETLAMBDA(it.rarityChance),
		RETLAMBDA(it.rarityChance.GetValue())
	);
}

TArray<FMissionMobChance> UMissionChancesUtil::GetMapUIMobChances(ELevelNames levelName, const FEligibleDLC& eligibleDLCs, const ItemArchetypeCounts& archetypeCounts) {
	auto&& missiondef = missions::get(levelName);
	return missiondef.getAncientMobChancesFor(eligibleDLCs, archetypeCounts, missions::hajper::shouldIncludeInUI);
}

TArray<FMissionProbability> UMissionChancesUtil::GetMapProbabilities(const FMissionState& missionState) {
	TArray<FMissionProbability> probabilities;
	missions::hajper::populateMissionProbabilities(missionState, probabilities);
	return probabilities;
}

float UMissionChancesUtil::GetMapMobChance(const FMissionState& missionState) {
	return FMath::Max(0.0f, missions::hajper::getApproximateChanceForAnyAncient(missionState));
}
