#pragma once

#include "game/Enchantments/generator/EnchantmentGeneratorTypes.h"
#include "ItemGeneratorLooterState.h"
#include "game/item/ItemType.h"

class ItemType;

namespace game {
struct Settings;
struct FDifficulty;

namespace item { namespace generator {
struct Looter;


struct EnvState {
	const Settings& settings;
	Looter looter;
};

struct PredState {
	const ItemType& item;
	const EnvState env;
};

struct EnchantmentState {
	const ItemType& item;
	const EnvState env;
};

struct ProbabilityState {
	const ItemType& item;
	const EnvState env;
};

using Pred = std::function<bool(const PredState&)>;
using PowerRange = FloatRange;
using PowerRangeCalc = std::function<PowerRange(const EnvState&)>;
using PowerCalc = std::function<float(const PowerRange&)>;
using EnchantmentCalc = std::function<enchgen::Counts(const EnchantmentState&)>;
using ProbabilityCalc = std::function<float(const ProbabilityState&)>;

struct Config {
	explicit Config(Pred);
	Config(Pred, PowerRangeCalc);
	Config(Pred, PowerRangeCalc, EnchantmentCalc);
	Config(Pred, PowerRangeCalc, EnchantmentCalc, ProbabilityCalc);
	Config(Pred, PowerRangeCalc, EnchantmentCalc, ProbabilityCalc, PowerCalc);
	Config(Pred, PowerRangeCalc, EnchantmentCalc, ProbabilityCalc, PowerCalc, EnchantmentCalc);

	Pred pred;
	PowerRangeCalc powerrange;
	EnchantmentCalc enchantment;
	ProbabilityCalc probability;
	PowerCalc power;
	EnchantmentCalc netheriteEnchantment;
};

enum class Category {
	Gear,
	NonGear
};

enum class ItemSource {
	Drop,
	LootUnlocker,
	Merchant,
	NetheriteMerchant,
	MysteryBox,	
	MissionReward,
	RaidCaptainMissionReward,
};

}}}

namespace itemgen = game::item::generator;

#include "util/BooleanOperatorMacros.h"
GENERATE_BOOLEAN_PRED_OPERATORS(const game::item::generator::PredState&);
