#include "Dungeons.h"
#include "ItemGeneratorConfigs.h"
#include "ItemGeneratorLooterState.h"
#include "game/GameSettings.h"
#include "game/item/ItemType.h"
#include "game/mission/MissionDefs.h"
#include "game/Enchantments/generator/EnchantmentGeneratorConfigs.h"
#include "world/entity/MobTags.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "Engine/Engine.h"
#include "game/affector/Affectors.h"
#include "game/reward/RewardData.h"

namespace game { namespace item { namespace generator {

//
// Predicates
//
namespace predicates {

bool isAllowedOnAnyUnlockedMissionDifficulty(const PredState& state) {
	for (auto unlockedLevel : state.env.looter.unlockedLevels) {
		if (state.item.isAllowedOnMissionDifficulty(unlockedLevel, state.env.settings.difficulty)) {
			return true;
		}
	}
	return false;
}

bool isAllowedOnMissionDifficulty(const PredState& state) {
	if (state.env.settings.levelName != ELevelNames::Invalid) {
		return state.item.isAllowedOnMissionDifficulty(state.env.settings.levelName, state.env.settings.difficulty);
	} else {
		return isAllowedOnAnyUnlockedMissionDifficulty(state);
	}
}

bool isValidRandomType(const PredState& state) {
	static const auto allowedOnDifficulty = AllowedOnDifficulty();
	static const TArray<FItemId> DeniedRandomItemTypes{
		game::item::type::HealthPotion.getId(),
		game::item::type::Arrow.getId(),
		game::item::type::FireworksArrow.getId(),
		game::item::type::DiamondDust.getId()
	};
	return allowedOnDifficulty(state) && isAllowedOnMissionDifficulty(state) && !DeniedRandomItemTypes.Contains(state.item.getId()) && !state.item.isEventItem();
}

const Pred& AllowedOnMissionDifficulty() {
	static const Pred pred = [](const PredState& state) { return isAllowedOnMissionDifficulty(state); };
	return pred;
}

const Pred& AllowedOnAnyUnlockedMissionDifficulty() {
	static const Pred pred = [](const PredState& state) { return isAllowedOnAnyUnlockedMissionDifficulty(state); };
	return pred;
}

const Pred& Gear() {
	static const Pred pred = [](const PredState& state) { return state.item.isGear() && isValidRandomType(state); };
	return pred;
}

const Pred& Item() {
	static const Pred pred = [](const PredState& state) { return state.item.isPermanent() && isValidRandomType(state); };
	return pred;
}

const Pred& GearVendor() {
	static const Pred pred = [](const PredState& state) { return state.item.isGear() && !state.item.isVendorBlocked() && isValidRandomType(state); };
	return pred;
}

const Pred& ItemVendor() {
	static const Pred pred = [](const PredState& state) { return state.item.isPermanent() && !state.item.isVendorBlocked() && isValidRandomType(state); };
	return pred;
}

const Pred& Consumable() {
	static const Pred pred = [](const PredState& state) { return state.item.isConsumable() && isValidRandomType(state); };
	return pred;
}

const Pred& ConsumableNoWaterBreathing() {
	static const Pred pred = [](const PredState& state) { return state.item.isConsumable() && isValidRandomType(state) && state.item.getId() != game::item::type::WaterBreathingPotion.getId(); };
	return pred;
}

const Pred& None() {
	static const Pred pred = [](const PredState& state) { return false; };
	return pred;
}

const Pred& AllowedOnDifficulty() {
	static const Pred pred = [](const PredState& state) {
		return state.item.isAllowedOnDifficulty(state.env.settings.difficulty);
	};
	return pred;
}

Pred Only(FItemId type) {
	return [type](const PredState& state) { return state.item.getId() == type; };
}

Pred OneOf(const TArray<FItemId>& types) {
	// Build an array for allowance instead of using a quadratic Contains
	TArray<bool> allowed = Util::createZeroedTArrayOfSize<bool>(GetItemRegistry().GetValues().Num());
	for (const auto& type : types) {
		allowed[GetItemRegistry().IndexOf(type)] = true;
	}
	return [allowed](const PredState& state) { return allowed[GetItemRegistry().IndexOf(state.item.getId())]; };
}

Pred SlotType(ESlotType slotType) {
	return [slotType](const PredState& state) { return state.item.slotType() == slotType && isValidRandomType(state); };
}

Pred VendorBoxSlotType(const TSet<ESlotType>& slotTypes) {
	return [slotTypes](const PredState& state) { return !state.item.isVendorBlocked() && slotTypes.Contains(state.item.slotType()) && isValidRandomType(state); };
}

Pred Tag(ItemTag tag) {
	return [tag](const PredState& state) { return state.item.hasTag(tag) && isValidRandomType(state); };
}

}

//
// Power calculations
//
namespace powerrangecalcs {

PowerRangeCalc Exactly(float power) {
	return [power](const EnvState& state) { return PowerRange(power, power); };
}

PowerRange DefaultGearRarityPowerRange(const PowerRange& powerRange, EItemRarity rarity) {
	switch (rarity) {
		case EItemRarity::Unique:
			return PowerRange(powerRange.lerp(0.5), powerRange.max());
		case EItemRarity::Rare:
			return PowerRange(powerRange.lerp(0.2), powerRange.max());
		case EItemRarity::Common:
		default:
			return powerRange;
	}
}

const game::DifficultyStats& GetRecommendedOrCurrentDifficultyStats(const EnvState& state) {
	if (auto recommendation = state.looter.difficultyRecommendation) {
		return recommendation->getDifficultyStats(missions::get(state.settings.levelName).getExtraChallenge());
	}
	return state.settings.difficultyStats;
}

const PowerRange GetDropItemPowerRange(const EnvState& state) {
	const auto missionRange = state.settings.difficultyStats.GetDropItemPowerRange();
	const auto recommendedRange = GetRecommendedOrCurrentDifficultyStats(state).GetDropItemPowerRange();
	const auto min = FMath::Min(missionRange.min(), recommendedRange.min());
	const float recommendedToMissionLerpAlpha = state.settings.difficulty.anyEndlessStruggle() ? 0.1f : 0.3f;
	const auto max = (missionRange.max() > recommendedRange.max()) ? FMath::Lerp(recommendedRange.max(), missionRange.max(), recommendedToMissionLerpAlpha) : missionRange.max();
	return { min, max };
}

const PowerRange GetRewardItemPowerRange(const EnvState& state) {
	const auto missionRange = state.settings.difficultyStats.GetRewardItemPowerRange();
	const auto recommendedRange = GetRecommendedOrCurrentDifficultyStats(state).GetRewardItemPowerRange();
	const float recommendedToMissionLerpAlpha = state.settings.difficulty.anyEndlessStruggle() ? 0.2f : 0.5f;
	const auto min = FMath::Lerp(recommendedRange.min(), missionRange.min(), recommendedToMissionLerpAlpha);
	const auto max = FMath::Lerp(recommendedRange.max(), missionRange.max(), recommendedToMissionLerpAlpha);
	return { min, max };
}

const PowerRange GetRaidCaptainChestItemPowerRange(const EnvState& state) {
	const auto missionRange = state.settings.difficultyStats.GetRaidCaptainRewardItemPowerRange();
	const auto recommendedRange = GetRecommendedOrCurrentDifficultyStats(state).GetRaidCaptainRewardItemPowerRange();
	const float recommendedToMissionLerpAlpha = state.settings.difficulty.anyEndlessStruggle() ? 0.2f : 0.5f;
	const auto min = FMath::Lerp(recommendedRange.min(), missionRange.min(), recommendedToMissionLerpAlpha);
	const auto max = FMath::Lerp(recommendedRange.max(), missionRange.max(), recommendedToMissionLerpAlpha);
	return { min, max };
}

const PowerRange GetVendorDifficultyItemPowerRange(const EnvState& state) {
	const auto& stats = state.looter.highestCompletedDifficultyStats.Get(game::FDifficulty::LOWEST);
	const FloatRange missionRange = stats.GetDropItemPowerRange();
	const FloatRange recommendedRange = GetRecommendedOrCurrentDifficultyStats(state).GetDropItemPowerRange();
	const float min = FMath::Min(missionRange.min(), recommendedRange.min());
	const float recommendedToMissionLerpAlpha = stats.GetDifficulty().anyEndlessStruggle() ? 0.1f : 0.3f;
	const float max = (missionRange.max() > recommendedRange.max()) ? FMath::Lerp(recommendedRange.max(), missionRange.max(), recommendedToMissionLerpAlpha) : missionRange.max();
	auto range = FloatRange( min, max );
	return FloatRange(range.lerp(0.25f), range.max()); //Vendor power roll fudge
}

const PowerRangeCalc& AsDefaultVendor() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		return DefaultGearRarityPowerRange(GetVendorDifficultyItemPowerRange(state), EItemRarity::Common);
	};
	return power;
}

const PowerRangeCalc& AsRareVendor() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		return DefaultGearRarityPowerRange(GetVendorDifficultyItemPowerRange(state), EItemRarity::Rare) + state.settings.difficultyStats.GetRareItemPowerBoost();
	};
	return power;
}

const PowerRangeCalc& AsUniqueVendor() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		return DefaultGearRarityPowerRange(GetVendorDifficultyItemPowerRange(state), EItemRarity::Unique) + state.settings.difficultyStats.GetRareItemPowerBoost();
	};
	return power;
}

const PowerRangeCalc& FromVendorRarity(EItemRarity rarity) {
	switch (rarity) {
	case EItemRarity::Unique:
		return AsUniqueVendor();
	case EItemRarity::Rare:
		return AsRareVendor();
	case EItemRarity::Common:
	default:
		return AsDefaultVendor();
	}
}

const PowerRangeCalc& Default() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = GetDropItemPowerRange(state);
		return DefaultGearRarityPowerRange(range, EItemRarity::Common);
	};
	return power;
}

const PowerRangeCalc& DefaultRare() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = GetDropItemPowerRange(state);
		return DefaultGearRarityPowerRange(range, EItemRarity::Rare) + state.settings.difficultyStats.GetRareItemPowerBoost();
	};
	return power;
}

const PowerRangeCalc& DefaultUnique() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = GetDropItemPowerRange(state);
		return DefaultGearRarityPowerRange(range, EItemRarity::Unique) + state.settings.difficultyStats.GetRareItemPowerBoost();
	};
	return power;
}


const PowerRangeCalc& MissionReward() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = GetRewardItemPowerRange(state);
		const auto FudgedRewardRange = PowerRange(range.lerp(0.5), range.max());
		return FudgedRewardRange;
	};
	return power;
}

static PowerRange RarifyPowerRange(const PowerRange& powerRange, const EnvState& state) {
	const float min = powerRange.min() + state.settings.difficultyStats.GetRareItemPowerBoost();
	const auto BoostedFudgedRewardRange = PowerRange(FMath::Min(min, powerRange.max()), powerRange.max());
	return BoostedFudgedRewardRange;
}

const PowerRangeCalc& MissionRewardRare() {
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = MissionReward()(state);
		const auto BoostedFudgedRewardRange = RarifyPowerRange(range, state);
		return BoostedFudgedRewardRange;
	};
	return power;
}

const game::item::generator::PowerRangeCalc& MissionRewardUnique() {
	return MissionRewardRare();
}

const game::item::generator::PowerRangeCalc& RaidCaptainReward()
{
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = GetRaidCaptainChestItemPowerRange(state);
		const auto FudgedRewardRange = PowerRange(range.lerp(0.5), range.max());
		return FudgedRewardRange;
	};
	return power;
}

const game::item::generator::PowerRangeCalc& RaidCaptainRewardRare()
{
	static const PowerRangeCalc power = [](const EnvState& state) {
		const auto range = RaidCaptainReward()(state);
		const auto BoostedFudgedRewardRange = RarifyPowerRange(range, state);
		return BoostedFudgedRewardRange;
	};
	return power;
}

const PowerRangeCalc& FromSource(ItemSource itemSource)
{
	switch (itemSource) {
	case ItemSource::Merchant:
	case ItemSource::MysteryBox:
	case ItemSource::NetheriteMerchant:
		return AsDefaultVendor();
	case ItemSource::MissionReward:
		return MissionReward();
	case ItemSource::RaidCaptainMissionReward:
		return RaidCaptainReward();
	case ItemSource::Drop:
	case ItemSource::LootUnlocker:
	default:		
		return Default();
	}
}

const PowerRangeCalc& FromSourceRare(ItemSource itemSource)
{
	switch (itemSource) {
	case ItemSource::Merchant:
	case ItemSource::MysteryBox:
	case ItemSource::NetheriteMerchant:
		return AsRareVendor();
	case ItemSource::MissionReward:
		return MissionRewardRare();
	case ItemSource::RaidCaptainMissionReward:
		return RaidCaptainRewardRare();
	case ItemSource::Drop:
	case ItemSource::LootUnlocker:
	default:
		return DefaultRare();
	}
}

const PowerRangeCalc& FromSourceUnique(ItemSource itemSource)
{
	switch (itemSource) {
	case ItemSource::Merchant:
	case ItemSource::MysteryBox:
	case ItemSource::NetheriteMerchant:
		return AsUniqueVendor();
	case ItemSource::MissionReward:
		return MissionRewardUnique();
	case ItemSource::RaidCaptainMissionReward:
		return RaidCaptainRewardRare();
	case ItemSource::Drop:
	case ItemSource::LootUnlocker:
	default:
		return DefaultUnique();
	}
}

}


namespace powercalcs {

const PowerCalc& Default() {
	static const PowerCalc power = [](PowerRange powerRange) {
		const float randomRoll = FMath::Pow(FMath::FRand(), 3.5f);
		return powerRange.min() + (powerRange.max() - powerRange.min()) * randomRoll;
	};
	return power;
}

}

//
// Enchantment calculations
//
namespace enchantmentcalcs {

const EnchantmentCalc& None() {
	static const EnchantmentCalc ench = [](const EnchantmentState& state) {
		return enchgen::Counts(0, 0, 0, 0, 0);
	};
	return ench;
}

const EnchantmentCalc& Default() {
	static const EnchantmentCalc ench = [](const EnchantmentState& state) {
		return enchgen::configs::FromDifficulty(state.env.settings.difficulty, false, state.item.isUnique());
	};
	return ench;
}


const game::item::generator::EnchantmentCalc& MissionReward()
{
	return Default();
}

game::item::generator::EnchantmentCalc FromItemPower(float itemPower, bool isBoss, bool isUnique) {
	return [itemPower, isBoss, isUnique](const EnchantmentState& state) {
		return enchgen::configs::FromItemPower(itemPower, isBoss, isUnique);
	};	
}

const EnchantmentCalc& DefaultDroppedByMob(EntityType SourceType) {
	static const EnchantmentCalc boss = [](const EnchantmentState& state) {
		return enchgen::configs::FromDifficulty(state.env.settings.difficulty, true, state.item.isUnique());
	};
	return hasMobTag(SourceType, MobTags::HashTag_Miniboss) ? boss : Default();
}

const EnchantmentCalc& Netherite() {
	static const EnchantmentCalc ench = [](const EnchantmentState& state) {
		return enchgen::configs::NetheriteFromDifficulty(state.env.settings.difficulty, false, state.item.isUnique());
	};
	return ench;
}

const EnchantmentCalc& NetheriteMerchant() {
	static const EnchantmentCalc ench = [](const EnchantmentState& state) {
		return enchgen::configs::NetheriteFromDifficulty(state.env.settings.difficulty, false, state.item.isUnique());
	};
	return ench;
}

const EnchantmentCalc& NetheriteAncient() {
	static const EnchantmentCalc boss = [](const EnchantmentState& state) {
		return enchgen::configs::NetheriteFromDifficulty(state.env.settings.difficulty, true, state.item.isUnique());
	};
	return boss;
}

const EnchantmentCalc& NetheriteDroppedByMob(EntityType SourceType) {
	if (hasMobTag(SourceType, MobTags::HashTag_Ancient)) {
		return NetheriteAncient();
	}
	return None();
}

EnchantmentCalc Counts(const enchantment::generator::Counts& counts) {
	return [counts](const EnchantmentState& state) { return counts; };
}

}


namespace probabilitycalcs {

	float GetImplementedAndAvailableBaseProbability(const ItemType& item) {
		return (item.isWorkInProgress() ? 0.0f : 1.0f) * item.getDropChance();
	}

	const ProbabilityCalc& Default() {
		static const ProbabilityCalc calc = [](const ProbabilityState& state) {
			return GetImplementedAndAvailableBaseProbability(state.item);
		};
		return calc;
	}

	const ProbabilityCalc& AllowAll() {
		static const ProbabilityCalc calc = [](const ProbabilityState& state) {
			return 1.0f;
		};
		return calc;
	}

	const ProbabilityCalc& Unique() {
		static const ProbabilityCalc calc = [](const ProbabilityState& state) {
			if (state.item.isUnique()) {
				return GetImplementedAndAvailableBaseProbability(state.item);
			}
			return 0.0f;
		};

		return calc;
	}

	const ProbabilityCalc& NonUnique() {
		static const ProbabilityCalc calc = [](const ProbabilityState& state) {
			if (!state.item.isUnique()) {
				return GetImplementedAndAvailableBaseProbability(state.item);
			}
			return 0.0f;
		};

		return calc;
	}
}

//
// Configurations (Predicate + Power calculation)
//
namespace configs {

Config DefaultItem() {
	return Config(predicates::Item());
}

Config DefaultGear() {
	return Config(predicates::Gear());
}

Config MissionInspectorItem() {
	return Config(predicates::Item(), powerrangecalcs::Default(), enchantmentcalcs::Default(), probabilitycalcs::NonUnique(), powercalcs::Default());
}

Config MissionInspectorGear() {
	return Config(predicates::Gear(), powerrangecalcs::Default(), enchantmentcalcs::Default(), probabilitycalcs::NonUnique(), powercalcs::Default());
}

Config LobbyItem() {
	return DefaultItem();
}

Config LobbyItem(FItemId type) {
	return Item(type);
}

Config LobbyGear() {
	return DefaultGear();
}

Config Item(FItemId type, float power) {
	return Config(predicates::Only(type), powerrangecalcs::Exactly(power));
}

Config Item(FItemId type) {
	return Config(predicates::Only(type));
}

Config ConsoleItem(FItemId type, float power, bool boss, bool unique, bool netherite) {
	if (netherite) {
		return Config(predicates::Only(type), powerrangecalcs::Exactly(power), enchantmentcalcs::FromItemPower(power, boss, unique), probabilitycalcs::AllowAll(), powercalcs::Default(), enchantmentcalcs::FromItemPower(power, boss, unique));
	}
	return Config(predicates::Only(type), powerrangecalcs::Exactly(power), enchantmentcalcs::FromItemPower(power, boss, unique), probabilitycalcs::AllowAll(), powercalcs::Default());
}

}

namespace enchantmentlevelcalcs {

int GenerateNetheriteEnchantmentLevel(const FDifficulty& difficulty) {
	const auto endless = difficulty.endlessStruggle();
	static constexpr auto baseEnchantmentLevel = 0.2f;
	static constexpr auto endlessStruggleLevelsPerLevelIncrease = 10.f;
	static constexpr auto levelRandomRange = 1.5f;
	const float prg = baseEnchantmentLevel + endless.Value / endlessStruggleLevelsPerLevelIncrease;
	return FMath::Clamp(FMath::FloorToInt(FMath::FRandRange(prg, prg + levelRandomRange)), 1, 3);
}

}


}}}
