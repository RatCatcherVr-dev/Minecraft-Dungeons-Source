// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SurpriseGift.h"
#include "game/item/ItemUtil.h"
#include "game/item/ItemType.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemTypeDefs.h"
#include <AbilitySystemComponent.h>
#include "game/actor/item/StorableItem.h"


USurpriseGift::USurpriseGift() {
	TypeId = EEnchantmentTypeID::SurpriseGift;

	LevelMultiplier = [this](int level) -> float {
		return 0.5f * (float)level;
	};
	MultiplierFormatter = valueformat::asPercentage;

	PossibleItemsToDrop = {
		{ game::item::type::StrengthPotion.getId() },
		{ game::item::type::SwiftnessPotion.getId() },
		{ game::item::type::BackstabbersBrew.getId() },
		{ game::item::type::DefensePotion.getId() },
		{ game::item::type::IcePotion.getId() },
		{ game::item::type::DenseBrewPotion.getId() },
		{ game::item::type::WaterBreathingPotion.getId() },
		{ game::item::type::Arrow.getId(), 10 }
	};
}


int USurpriseGift::GetDropAmount() const {
	const float combinedSpawnChance = LevelMultiplier(Level);
	const int maxPotionsToSpawn = FMath::CeilToInt(combinedSpawnChance);
	const float deductAmount = 1.0f;
	float deductableSpawnChance = combinedSpawnChance;

	int dropCount = 0;
	for (int i = 0; i < maxPotionsToSpawn; ++i) {
		if (FMath::RandRange(0.0f, 1.0f) < deductableSpawnChance) {
			dropCount++;
			deductableSpawnChance -= deductAmount;
		}
	}
	return dropCount;
}
