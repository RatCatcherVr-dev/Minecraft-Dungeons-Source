#pragma once

#include "MobEnchantmentTypes.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/ai/behavior/BehaviorFactoryWip.h"
#include "game/difficulty/DifficultyStats.h"
#include "world/entity/EntityTypes.h"

enum class EItemType : uint8;
class Random;

namespace game {
struct FDifficulty;

namespace enchantment { namespace mob { 

namespace configs {

	struct Settings {
		Settings() {} //D11.PS - Added missing default constructor

		const float NEW_GROUP_COST = 5.0f;

		const struct NewGroupWeights {
			FloatRange totalWeight = FloatRange(0.1f, 0.1f);

			FloatRange normalWeight = FloatRange(1.f, 1.f);
			FloatRange powerfulWeight = FloatRange(0.5f, 1.f);
		} newGroupWeights;

		const struct NewEnchantmentWeights {
			FloatRange totalWeight = FloatRange(2.f, 2.f);

			FloatRange normalWeight = FloatRange(1.f, 1.f);
			FloatRange powerfulWeight = FloatRange(0.5f, 1.f);
		} newEnchantmentWeights;

		const struct NewMobWeights {
			FloatRange totalWeight = FloatRange(0.5f, 0.5f);
		} newMobWeights;

		/** Cost is 1 per enchantment per mob for normal enchantments and 2 for powerful.
			At the highest difficulty we want the minimum to be three fully enchanted mobs. */
		const struct EnchantmentCostPoolRanges {
			FloatRange lowerBound = FloatRange(3.f, 9.f);
			FloatRange upperBound = FloatRange(3.f, 18.f);
		} enchantmentCostPoolRanges;

		const int minNewGroupSize = 3;
		const TMap<EntityType, int> minGroupSize = {
			{EntityType::Necromancer, 1},
			{EntityType::Enchanter, 2},
			{EntityType::Geomancer, 1},
			{EntityType::Evoker, 1 },
			{EntityType::Wraith, 2},
			{EntityType::Pillager, 2},
			{EntityType::RedstoneGolem, 1},
			{EntityType::Witch, 2}
		};

		FORCEINLINE int GroupSize(EntityType type) const { return minGroupSize.Contains(type) ? minGroupSize[type] : minNewGroupSize; };
	};

}
	
	struct MobEnchantmentSpawnData {
		float probability; 
		float powerFraction;
		float dropChance;
	};

	TArray<mobspawn::SpawnGroup> EnchantPickedMobs(TArray<mobspawn::SpawnGroup>&&, const FDifficulty& difficulty, const MobEnchantmentSpawnData&, const configs::Settings&, Random* = nullptr);
}}}
