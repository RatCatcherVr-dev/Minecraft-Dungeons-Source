#pragma once

#include "game/item/enchantment/EnchantmentData.h"
#include "util/Math.h"
#include <Array.h>

enum class EntityType : uint32;

namespace game { namespace enchantment { namespace mob {

constexpr int maxMobEchantments = 3;

FORCEINLINE int scoreForPowerful(int num) { return 2 * num; }

struct EnchantmentCounts {
private:
	int enchantments;
	int powerfulEnchantments;
public:
	EnchantmentCounts(int enchantments, int powerfulEnchantments)
		: enchantments(Math::clamp(enchantments, 0, 3))
		, powerfulEnchantments(Math::clamp(powerfulEnchantments, 0, 3)) {
	}
	void addEnchantment() {
		if (!canAdd()) return;
		++enchantments;
	}

	void addPowerful() {
		if (!canAdd()) return;
		++powerfulEnchantments;
	}

	int getEnchantments() const {
		return enchantments;
	}

	int getPowerful() const {
		return powerfulEnchantments;
	}

	int getScore() const { 
		return enchantments + scoreForPowerful(powerfulEnchantments);
	}

	bool canAdd() const {
		return enchantments + powerfulEnchantments < maxMobEchantments;
	}
};

struct EnchantedMobTypeDef {
	EnchantedMobTypeDef(EnchantmentCounts counts, EntityType type, int amount)
		: counts(counts)
		, type(type)
		, amount(amount) {
	}

	EnchantmentCounts counts;
	EntityType type;
	int amount;
};

struct EnchantedMob {
	EnchantedMob(const TArray<FEnchantmentData>& enchantmentData, EntityType type)
		: enchantmentData(enchantmentData)
		, type(type) {
	}

	TArray<FEnchantmentData> enchantmentData;
	EntityType type;
};

using EnchantedMobTypes = TArray<EnchantedMobTypeDef>;
}}}

namespace mobench = game::enchantment::mob;
