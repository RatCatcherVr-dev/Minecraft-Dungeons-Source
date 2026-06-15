// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "AnimaConduitRanged.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/actor/item/Soul.h"


UAnimaConduitRanged::UAnimaConduitRanged() : UAnimaConduitMelee() {
	TypeId = EEnchantmentTypeID::AnimaConduitRanged;
}
