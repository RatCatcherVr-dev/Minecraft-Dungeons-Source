#include "Dungeons.h"
#include "ItemGeneratorTypes.h"
#include "ItemGeneratorConfigs.h"

namespace game { namespace item { namespace generator {

Config::Config(Pred pred)
	: Config(std::move(pred), powerrangecalcs::Default()) {
}

Config::Config(Pred pred, PowerRangeCalc powerrange)
	: Config (std::move(pred)
	, std::move(powerrange)
	, enchantmentcalcs::Default()) {
}

Config::Config(Pred pred, PowerRangeCalc powerrange, EnchantmentCalc enchantment)
	: Config (std::move(pred)
	, std::move(powerrange)
	, std::move(enchantment)
	, probabilitycalcs::Default())
	{
}

Config::Config(Pred pred, PowerRangeCalc powerrange, EnchantmentCalc enchantment, ProbabilityCalc probability)
	: Config ((std::move(pred))
	, (std::move(powerrange))
	, (std::move(enchantment))
	, (std::move(probability)) 
	, powercalcs::Default()) {
}

Config::Config(Pred pred, PowerRangeCalc powerrange, EnchantmentCalc enchantment, ProbabilityCalc probability, PowerCalc power)
	: Config((std::move(pred))
	, (std::move(powerrange))
	, (std::move(enchantment))
	, (std::move(probability))
	, (std::move(power))
	, enchantmentcalcs::None()) {
}

Config::Config(Pred pred, PowerRangeCalc powerrange, EnchantmentCalc enchantment, ProbabilityCalc probability, PowerCalc power, EnchantmentCalc netheriteEnchantment)
	: pred(std::move(pred))
	, powerrange(std::move(powerrange))
	, enchantment(std::move(enchantment))
	, probability(std::move(probability))
	, power(std::move(power))
	, netheriteEnchantment(std::move(netheriteEnchantment)) {
}

}}}
