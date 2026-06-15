#pragma once

#include "ItemGeneratorTypes.h"
#include "util/FloatRange.h"
#include "game/item/ItemType.h"
#include "world/entity/EntityTypes.h"

enum class ESlotType : uint8;

namespace game { namespace item { namespace generator {

namespace predicates {

const Pred& Gear();
const Pred& Item();

const Pred& GearVendor();
const Pred& ItemVendor();

const Pred& Consumable();
const Pred& ConsumableNoWaterBreathing();
      Pred  Only(FItemId);
      Pred  OneOf(const TArray<FItemId>&);
      Pred  SlotType(ESlotType);
      Pred  VendorBoxSlotType(const TSet<ESlotType>&);

	  Pred  Tag(ItemTag);

const Pred& AllowedOnMissionDifficulty();
const Pred& AllowedOnAnyUnlockedMissionDifficulty();
const Pred& AllowedOnDifficulty();

const Pred& None();

}

namespace powerrangecalcs {

      PowerRangeCalc  Exactly(float);
const PowerRangeCalc& Default();
const PowerRangeCalc& DefaultRare();
const PowerRangeCalc& DefaultUnique();
const PowerRangeCalc& MissionReward();
const PowerRangeCalc& MissionRewardRare();
const PowerRangeCalc& MissionRewardUnique();
const PowerRangeCalc& RaidCaptainReward();
const PowerRangeCalc& RaidCaptainRewardRare();
const PowerRangeCalc& FromSource(ItemSource);
const PowerRangeCalc& FromSourceRare(ItemSource);
const PowerRangeCalc& FromSourceUnique(ItemSource);
const PowerRangeCalc& FromVendorRarity(EItemRarity);

const FloatRange GetDropItemPowerRange(const EnvState& state);
const FloatRange GetRewardItemPowerRange(const EnvState& state);
const FloatRange GetRaidCaptainChestItemPowerRange(const EnvState& state);
const FloatRange GetVendorDifficultyItemPowerRange(const EnvState& state);

}

namespace powercalcs {
const PowerCalc& Default();
}

namespace enchantmentcalcs {

const EnchantmentCalc& None();
const EnchantmentCalc& Default();
const EnchantmentCalc& MissionReward();
const EnchantmentCalc& DefaultDroppedByMob(EntityType SourceType);
const EnchantmentCalc& Netherite();
const EnchantmentCalc& NetheriteMerchant();
const EnchantmentCalc& NetheriteAncient();
const EnchantmentCalc& NetheriteDroppedByMob(EntityType SourceType);
      EnchantmentCalc  FromItemPower(float itemPower, bool isBoss = false, bool isUnique = false);
      EnchantmentCalc  Counts(const enchantment::generator::Counts&);
}

namespace probabilitycalcs {
	const ProbabilityCalc& Default();
	const ProbabilityCalc& AllowAll();
	const ProbabilityCalc& Unique();
	const ProbabilityCalc& NonUnique();
}

namespace enchantmentlevelcalcs {
    int GenerateNetheriteEnchantmentLevel(const FDifficulty&);
}

namespace configs {
	
Config DefaultItem();
Config DefaultGear();

Config MissionInspectorItem();
Config MissionInspectorGear();

Config LobbyItem(FItemId);

Config Item(FItemId, float power);
Config Item(FItemId);

Config ConsoleItem(FItemId, float power, bool boss, bool unique, bool netherite);
}
}}}
