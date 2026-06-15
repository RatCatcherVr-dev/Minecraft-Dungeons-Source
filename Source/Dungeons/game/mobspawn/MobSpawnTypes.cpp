#include "Dungeons.h"
#include "MobSpawnTypes.h"

namespace game { namespace mobspawn {

SpawnGroup::SpawnGroup(EntityType type, size_t count)
	: type(type)
	, count(count)
	, groupLootDropChance(0.f) {
}

SpawnGroup::SpawnGroup(EntityType type, size_t count, TArray<FEnchantmentData> data, float dropChance)
	: enchantmentData(std::move(data))
	, type(type)
	, count(count)
	, groupLootDropChance(dropChance) {
}

void SpawnGroup::disenchant() {
	enchantmentData.SetNum(0);
}

bool SpawnGroup::hasGroupLoot() const {
	return groupLootDropChance > 0.f && enchantmentData.Num() > 0;
}

}}
