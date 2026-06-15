#include "Dungeons.h"
#include "MobEnchantmentConfigs.h"
#include "MobEnchantmentGenerator.h"
#include "MobEnchantmentTypes.h"
#include "game/difficulty/Difficulty.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/event/EventMobSpawner.h"
#include "world/entity/MobTags.h"
#include "util/Algo.h"
#include "util/BooleanOperatorMacros.h"
#include "util/FloatRange.h"
#include "util/FloatWeighedRandom.h"
#include "util/Math.h"
#include "util/SharedRandom.h"
#include <numeric>
#include <type_traits>

namespace game { namespace enchantment { namespace mob {

struct EnchantOption {
	EnchantOption(float weight, std::function<float()> action) : weight(weight), Execute(std::move(action)) {}
	float getWeight() const { return weight; }
	float weight;
	std::function<float()> Execute;
};

struct Config {
	const FDifficulty& difficulty;
	const configs::Settings& settings;
	Random& rnd;
	float normalizedDifficulty;
	float pool;
};

using OptionGenerator = std::function<std::vector<EnchantOption>(TArray<mobspawn::SpawnGroup>&, EnchantedMobTypes&, Config)> ;

Pred<const mobspawn::SpawnGroup&> groupSizeAndType(EntityType type, int size) {
	return [=](const mobspawn::SpawnGroup& group) { return group.type == type && group.count >= size; };
}

void AdjustWeights(std::vector<EnchantOption>& options, const float desiredWeight) {
	const auto totalWeight = algo::sum(options, RETLAMBDA(it.getWeight()));
	if (totalWeight > 0) {
		//Adjust weights so total weight of all options match desired weight for this option.
		const float adjustment = desiredWeight / totalWeight;
		algo::for_each(options, [adjustment](EnchantOption& v) { v.weight *= adjustment; });
	}
}

std::vector<EnchantOption> AddMobsToGroupGenerator(TArray<mobspawn::SpawnGroup>& mobs, EnchantedMobTypes& enchantedMobs, Config config) {
	std::vector<EnchantOption> options;
	
	for (auto& enchantedGroup : enchantedMobs) {
		const float cost = enchantedGroup.counts.getScore();
		if (cost <= config.pool) {
			if (auto existing = mobs.FindByPredicate(groupSizeAndType(enchantedGroup.type, 1))) {
				auto& entry = *existing;
				options.emplace_back( 1.f, [&entry, &enchantedGroup, cost] {
					entry.count--;
					enchantedGroup.amount++;
					return cost;
				} );
			}
		}
	}
	AdjustWeights(options, config.settings.newMobWeights.totalWeight.lerp(config.normalizedDifficulty));
	return options;
}

std::vector<EnchantOption> AddEnchantToGroupGenerator(TArray<mobspawn::SpawnGroup>& mobs, EnchantedMobTypes& enchantedMobs, Config config) {
	std::vector<EnchantOption> options;

	for (auto& enchantedGroup : enchantedMobs) {
		if (enchantedGroup.counts.canAdd()) {
			const float costPowerful = enchantedGroup.amount * scoreForPowerful(1);
			if (costPowerful <= config.pool) {
				options.emplace_back(config.settings.newEnchantmentWeights.normalWeight.lerp(config.normalizedDifficulty), [&enchantedGroup, costPowerful] {
					enchantedGroup.counts.addPowerful();
					return costPowerful;
				});
			}

			const float costNormal = enchantedGroup.amount;
			if (costNormal <= config.pool) {
				options.emplace_back(config.settings.newEnchantmentWeights.powerfulWeight.lerp(config.normalizedDifficulty), [&enchantedGroup, costNormal] {
					enchantedGroup.counts.addEnchantment();
					return costNormal;
				});
			}
		}
	}
	AdjustWeights(options, config.settings.newEnchantmentWeights.totalWeight.lerp(config.normalizedDifficulty));
	return options;
}


std::vector<EnchantOption> MakeNewGroupGenerator(TArray<mobspawn::SpawnGroup>& mobs, EnchantedMobTypes& enchantedMobs, Config config) {
	std::vector<EnchantOption> options;

	for (auto& spawnGroup : mobs) {
		const int groupSize = config.settings.GroupSize(spawnGroup.type);
		if (spawnGroup.count > 0
			&& !hasMobTag(spawnGroup.type, MobTags::HashTag_Miniboss)
			&& algo::none_of(enchantedMobs, RETLAMBDA(it.type == spawnGroup.type)))
		{
			float newGroupCost = (enchantedMobs.Num() > 0 ? config.settings.NEW_GROUP_COST : 0.0f);
			if (newGroupCost <= config.pool) {
				options.emplace_back(config.settings.newGroupWeights.normalWeight.lerp(config.normalizedDifficulty), [&spawnGroup, &enchantedMobs, cost = newGroupCost, groupSize] {
					enchantedMobs.Emplace(EnchantmentCounts(1, 0), spawnGroup.type, groupSize);
					spawnGroup.count -= FMath::Min(static_cast<int>(spawnGroup.count), groupSize);
					return cost;
				});
			}

			const float costPowerful = scoreForPowerful(newGroupCost);
			if (costPowerful <= config.pool) {
				options.emplace_back(config.settings.newGroupWeights.powerfulWeight.lerp(config.normalizedDifficulty), [&spawnGroup, &enchantedMobs, costPowerful, groupSize] {
					enchantedMobs.Emplace(EnchantmentCounts(0, 1), spawnGroup.type, groupSize);
					spawnGroup.count -= FMath::Min(static_cast<int>(spawnGroup.count), groupSize);
					return costPowerful;
				});
			}
		}
	}
	AdjustWeights(options, config.settings.newGroupWeights.totalWeight.lerp(config.normalizedDifficulty));
	return options;
}

const std::array<OptionGenerator, 3> optionGenerators = { AddMobsToGroupGenerator, AddEnchantToGroupGenerator, MakeNewGroupGenerator };

std::vector<EnchantOption> GenerateOptions(TArray<mobspawn::SpawnGroup>& mobs, EnchantedMobTypes& enchantedMobs, Config state) {
	std::vector<EnchantOption> options;

	for (const auto& generator : optionGenerators) {
		auto newOptions = generator(mobs, enchantedMobs, state);
		options.insert(options.end(), newOptions.begin(), newOptions.end()); 
	};
	return options;
}

TArray<mobspawn::SpawnGroup> flatten(const TArray<mobspawn::SpawnGroup>& inMobs) {
	TArray<mobspawn::SpawnGroup> outmobs;

	for (const auto& entry : inMobs) {
		if (entry.enchantmentData.Num()) {
			outmobs.Emplace(entry);
		}
		else {
			for (int i = 0; i < entry.count; ++i) {
				outmobs.Emplace(entry.type, 1);
			}
		}
	}

	return outmobs;
}

TArray<mobspawn::SpawnGroup> EnchantPickedMobs(TArray<mobspawn::SpawnGroup>&& mutableMobs, const FDifficulty& difficulty, const MobEnchantmentSpawnData& data, const configs::Settings& settings, Random* rand /*= nullptr*/) {
	rand = Util::thisOrSharedRandom(rand);

	if (data.probability > 0.0f && data.probability > rand->nextFloat()) {
		EnchantedMobTypes enchantedMobs;
		Config config {
			difficulty,
			settings,
			*rand,
			data.powerFraction,
			rand->nextFloat(settings.enchantmentCostPoolRanges.lowerBound.lerp(data.powerFraction), settings.enchantmentCostPoolRanges.upperBound.lerp(data.powerFraction))
		};

		auto options = GenerateOptions(mutableMobs, enchantedMobs, config);
		while (options.size()) {
			config.pool -= FMath::Min(config.pool, FloatWeighedRandom::getRandomItem(rand, options)->Execute());
			options = GenerateOptions(mutableMobs, enchantedMobs, config);
		}
		TArray<mobspawn::SpawnGroup> outMobs;
		for (const auto& enchanted : enchantedMobs) {
			if (!hasMobTag(enchanted.type, MobTags::HashTag_Unenchantable))
			{
				outMobs.Emplace(enchanted.type, enchanted.amount, generator::generate(enchanted.type, enchanted.counts, config.difficulty), data.dropChance);
			}
		}
		outMobs.Append(flatten(mutableMobs));
		return outMobs;
	} else {
		return flatten(mutableMobs);
	}
}

}}}
