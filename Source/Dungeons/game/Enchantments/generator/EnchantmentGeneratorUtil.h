#pragma once

#include "EnchantmentGeneratorTypes.h"
#include "game/component/EnchantmentComponent.h"
#include "CoreMinimal.h"
#include "EnchantmentGenerator.h"

class Random;

namespace game { namespace enchantment { namespace generator {

struct PredState {
	const Enchantments& elements;
	FIntPoint pt;

	Enchantments sameRow() const;
	Enchantments row(int i) const;
};

using Pred = std::function<bool(PredState, const EnchantmentType&)>;

const EnchantmentType* getRandomEnchantment(const Enchantments&, PredState, const std::vector<Pred>&, Random* = nullptr);

}}}
