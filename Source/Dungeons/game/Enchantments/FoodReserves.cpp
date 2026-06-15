// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "FoodReserves.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemUtil.h"
#include <AbilitySystemComponent.h>
#include "util/Algo.h"

UFoodReserves::UFoodReserves() {
	using namespace game::item::util;

	TypeId = EEnchantmentTypeID::FoodReserves;

	LevelMultiplier = [this](int level) -> float {
		return level;
	};
	MultiplierFormatter = valueformat::asConstant;

	PossibleItemsToDrop = algo::copy_if_map_tarray(GetItemRegistry().GetValues(), RETLAMBDA(it->hasTag(ItemTag::Food)), RETLAMBDA(FSpecifiedStoreCountItemDrop(it->getId())));
}
