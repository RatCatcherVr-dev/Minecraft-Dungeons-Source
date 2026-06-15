#include "Dungeons.h"
#include "MobGroupUtil.h"
#include "game/GameTypes.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/Enchantments/mobs/MobEnchantmentConfigs.h"
#include "game/Enchantments/mobs/MobEnchantmentTypes.h"
#include "game/Enchantments/mobs/MobEnchantmentGenerator.h"
#include "lovika/io/IoMobTypes.h"
#include "util/parse/eval/Boolean.h"
#include "util/parse/ParserFactory.h"
#include "util/Algo.hpp"
#include "util/FloatWeighedRandom.h"
#include "util/RandomUtil.h"
#include "util/SharedRandom.h"
#include "world/entity/MobTags.h"
#include <algorithm>
#include <Array.h>

namespace game { namespace mobspawn {

//
// MobGroup parsing, expressions, etc
//
::Pred<EntityType> mobPredicateFactory(const parse::token::Token& token) {
	if (parse::token::Type::Value == token.type) {
		return[tag = Util::toLower(token.data)](EntityType mob) { return hasMobTag_alreadyLowerCase(mob, tag); };
	}
	return {};
}

parse::eval::boolean::PredicateNode<EntityType> createExcludePredicate(const std::vector<std::string>& excludes) {
	parse::eval::boolean::PredicateNode<EntityType> pred{ parse::token::Token(parse::token::Type::Or, "or") };
	for (auto&& exclude : excludes) {
		if (auto sub = parse::predicateDefault<EntityType>(excludes[0], mobPredicateFactory)) {
			pred.children.push_back(sub.GetValue());
		}
	}
	return pred;
}

void buildFilteredTypes(std::set<EntityType>& types, const std::string& expr, const std::function<bool(EntityType)>& exclude) {
	if (auto pred = parse::predicateDefault<EntityType>(expr, mobPredicateFactory)) {
		for (auto&& type : game::TypeMap::singleton().mobTypes()) {
			if (pred->eval(type) && !exclude(type)) {
				types.insert(type);
			}
		}
	}
}


::Pred<EntityType> NoExclusion = [](auto type) { return false; };

std::vector<EntityType> evaluateMobExpr(const std::string& expr, const ::Pred<EntityType>& exclude) {
	std::set<EntityType> types;
	buildFilteredTypes(types, expr, exclude);
	return std::vector<EntityType>(types.begin(), types.end());
}

std::vector<EntityType> evaluateMobExpr(const std::string& expr) {
	return evaluateMobExpr(expr, NoExclusion);
}

void prepareMobGroup(const io::MobGroup& mobGroup, const ::Pred<EntityType>& exclude) {
	for (auto&& mobType : mobGroup.types) {
		mobType.types = evaluateMobExpr(mobType.expr, exclude);
	}
}

void prepareMobGroup(io::MobGroup& mobGroup) {
	prepareMobGroup(mobGroup, NoExclusion);
}

void buildPredicates(io::StretchMobs& mobs) {
	const auto excludeNode = createExcludePredicate(mobs.excludePredicates);
	const auto excludePred = [&excludeNode](auto type) { return excludeNode.eval(type); };

	for (auto&& mobGroup : mobs.groups) {
		prepareMobGroup(mobGroup, excludePred);
	}
}

//
// MobGroup selection
//

struct FilteredMobType {
	FilteredMobType(const io::MobType& type, const FDifficulty& difficulty)
		: type(type)
	{
		ensureMsgf(type.types, TEXT("For now, you have to run prepareMobGroup(io::MobGroup) before!"));
		types = type.types.GetValue(); // @note: we used to do difficulty thresholding for mobs here
	}

	bool hasMobs() const {
		return !types.empty();
	}
	
	unsigned int maxFractionClamped(int c) const {
		const auto upperLimit = Math::max(1, Math::round(type.maxFractionOfTotal * c));
		return Math::clamp(c, 0, upperLimit);
	}

	unsigned int clamp(unsigned int count, unsigned int totalCount) const {
		return Math::clamp(count, type.min, Math::min(type.max, maxFractionClamped(totalCount)));
	}

	const io::MobType& type;
	std::vector<EntityType> types;
};

struct WeightedMobGroup {
	WeightedMobGroup(const io::MobGroup& group, const FDifficulty& difficulty)
		: group(group) 
		//D11.PS - PS4 - Had to move this to the initialisation list because of the error: cannot be assigned because its copy assignment operator is implicitly deleted with type
		, mobTypes(algo::map_if_vector(group.types,
			RETLAMBDA(FilteredMobType(it, difficulty)),
			RETLAMBDA(it.hasMobs() && it.type.allowedOnDifficulties.has(difficulty.chosen()))))
	{
	}
	bool hasMobs() const { return !mobTypes.empty(); }
	float getWeight() const { return group.weight; }

	const io::MobGroup& group;
	std::vector<FilteredMobType> mobTypes;
};

static std::vector<WeightedMobGroup> buildAndFilterGroups(const std::vector<io::MobGroup>& mobGroups, const FDifficulty& difficulty) {
	return algo::map_if_vector(mobGroups,
		RETLAMBDA(WeightedMobGroup(it, difficulty)),
		RETLAMBDA(it.hasMobs() && it.group.allowedOnDifficulties.has(difficulty.chosen()))
	);
}

std::vector<const FilteredMobType*> pickMobTypes(const WeightedMobGroup& group, Random& rnd) {
	if (auto pick = group.group.typesCountInterval) {
		// @attn: or fail validation if pickCount > len(types) (and pickCount == 0?)?
		const int total = static_cast<int>(group.mobTypes.size());
		const int take = std::min(total, rnd.nextInt(pick->min, pick->max + 1));
		return algo::map_vector(Util::randomIndices(total, take, &rnd), RETLAMBDA(&group.mobTypes[it]));
	}
	return algo::map_vector(group.mobTypes, RETLAMBDA(&it));
}

std::unordered_map<int, unsigned> calculateLimitedSpawnCountsPerIndex(std::vector<const FilteredMobType*> types, Random& rnd, const int count) {
	// @note: We only do the max-fraction/max-percent here. We could as well have done the min/max
	//        clamping as well, but this is 1) for historical reasons, and that 2) we throw away
	//        some randomness when we add more clamping here; especially when using few MobTypes.
	//        This might change later.
	struct WeightedMobType {
		WeightedMobType(const FilteredMobType* type, int count) : maxGen(type->maxFractionClamped(count)), weight(type->type.weight) {}
		unsigned int maxGen;
		float weight;
		float getWeight() const { return weight; }
	};
	auto weightedItems = algo::map_vector(types, RETLAMBDA(WeightedMobType(it, count)));

	std::unordered_map<int, unsigned> countPerIndex;
	for (int i = 0; i < count; ++i) {
		const int index = FloatWeighedRandom::getRandomItemIndex(&rnd, weightedItems);
		if (index < 0 || weightedItems[index].getWeight() <= 0) { // the latter is actually a bug in FloatWeighedRandom that might show up once in a million or so
			continue;
		}
		if (++countPerIndex[index] >= weightedItems[index].maxGen) {
			weightedItems[index].weight = 0;
		}
	}
	for (size_t i = 0; i < types.size(); ++i) {
		countPerIndex[i] = types[i]->clamp(countPerIndex[i], count);
	}
	return countPerIndex;
}

TArray<TMap<EntityType, size_t>> pickMobsFromGroup(const WeightedMobGroup& group, Random& rnd, const int count) {
	const auto pickedMobTypes = pickMobTypes(group, rnd); // @todo @attn empty?
	const auto mobTypeIndexSpawnCounts = calculateLimitedSpawnCountsPerIndex(pickedMobTypes, rnd, count);

	TArray<TMap<EntityType, size_t>> mobCounts;
	for (auto&& kv : mobTypeIndexSpawnCounts) {
		auto& entityCountsPerMobType = mobCounts.AddDefaulted_GetRef();
		const auto& mobTypeEntityTypes = pickedMobTypes[kv.first]->types;
		for (auto i = 0U; i < kv.second; ++i) {
			const auto entityType = *Util::randomChoice(mobTypeEntityTypes, &rnd);
			entityCountsPerMobType.FindOrAdd(entityType)++;
		}
	}
	return mobCounts;
}

TArray<EntityType> flatten(const TArray<TMap<EntityType, size_t>>& types) {
	TArray<EntityType> out;
	for (const auto& type : types) {
		for (const auto& kv : type) {
			for (size_t i = 0; i < kv.Value; i++) {
				out.Add(kv.Key);
			}
		}
	}
	return out;
}

TArray<SpawnGroup> flatmap(const TArray<TMap<EntityType, size_t>>& types) {
	TArray<SpawnGroup> mobs;

	for (auto& entry : types) {
		for (auto it = entry.CreateConstIterator(); it; ++it) {
			mobs.Emplace(it->Key, it->Value);
		}
	}
	return mobs;
}

TArray<EntityType> calculateMobsWithoutDifficultyLimitation(const std::vector<io::MobGroup>& groups, int count, Random* rnd /*= nullptr*/) {
	return calculateMobs(groups, count, FDifficulty::HIGHEST, rnd);
}

TArray<EntityType> calculateMobs(const std::vector<io::MobGroup>& groups, int count, const FDifficulty& difficulty, Random* rnd /*= nullptr*/) {
	if (count > 0 && !groups.empty()) {
		rnd = Util::thisOrSharedRandom(rnd);

		const auto filteredGroups = buildAndFilterGroups(groups, difficulty);
		if (const auto mobGroup = FloatWeighedRandom::getRandomItem(rnd, filteredGroups)) {
			return flatten(pickMobsFromGroup(*mobGroup, *rnd, count));
		}
	}
	return {};
}

TArray<SpawnGroup> calculateMobsWithEnchantments_Internal(const std::vector<io::MobGroup>& groups, int count, const DifficultyStats& difficultyStats, const game::enchantment::mob::MobEnchantmentSpawnData& data, Random* rnd /*= nullptr*/) {
	if (count > 0 && !groups.empty()) {
		rnd = Util::thisOrSharedRandom(rnd);

		const auto filteredGroups = buildAndFilterGroups(groups, difficultyStats.GetDifficulty());
		if (const auto mobGroup = FloatWeighedRandom::getRandomItem(rnd, filteredGroups)) {
			const auto pickedMobs = pickMobsFromGroup(*mobGroup, *rnd, count);
			return enchantment::mob::EnchantPickedMobs(flatmap(pickedMobs), difficultyStats.GetDifficulty(), data, enchantment::mob::configs::Settings(), rnd);
		}
	}
	return {};
}

TArray<SpawnGroup> calculateMobsWithEnchantments(const std::vector<io::MobGroup>& groups, int count, const DifficultyStats& difficultyStats, Random* rnd /*= nullptr*/) {
	using game::enchantment::mob::MobEnchantmentSpawnData;
	return calculateMobsWithEnchantments_Internal(groups, count, difficultyStats, MobEnchantmentSpawnData { difficultyStats.GetEnchantedMobProbability(), difficultyStats.GetEnchantedMobPowerFraction(), difficultyStats.GetEnchantedMobGroupDropChance() }, rnd);
}

TArray<SpawnGroup> calculateObjectiveWaveMobsWithEnchantments(const std::vector<io::MobGroup>& groups, int count, const DifficultyStats& difficultyStats, Random* rnd /*= nullptr*/) {
	using game::enchantment::mob::MobEnchantmentSpawnData;
	return calculateMobsWithEnchantments_Internal(groups, count, difficultyStats, MobEnchantmentSpawnData { difficultyStats.GetEnchantedMobObjectiveWaveProbability(), difficultyStats.GetEnchantedMobPowerFraction(), difficultyStats.GetEnchantedMobGroupDropChance()}, rnd);
}

io::MobGroup sampleWithProbabilities(const std::vector<EntityType>& types, std::initializer_list<float> probabilities, float restProbability/* = 0*/) {
	const size_t typesWithProbabilityCount = std::min(probabilities.size(), types.size());
	std::vector<io::MobType> pickedTypes;

	for (size_t i = 0; i < typesWithProbabilityCount; ++i) {
		pickedTypes.push_back({ types[i], probabilities.begin()[i] });
	}
	if (restProbability > 0) {
		for (size_t i = typesWithProbabilityCount; i < types.size(); ++i) {
			pickedTypes.push_back({ types[i], restProbability });
		}
	}
	return { pickedTypes };
}

}}
