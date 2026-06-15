#include "Dungeons.h"
#include "EntityTypeMappers.h"
#include "game/Difficulty/Difficulty.h"
#include "world/entity/EntityTypes.h"
#include "util/Algo.h"
#include "util/RandomUtil.h"
#include "util/SharedRandom.h"

namespace game { namespace mobspawn {

const std::unordered_map<EntityType, std::vector<EntityType>> TypeToVariants {
	{EntityType::Pillager,   {EntityType::PillagerVariant0, EntityType::PillagerVariant1, EntityType::PillagerVariant2}},
	{EntityType::Skeleton,   {EntityType::SkeletonVariant0, EntityType::SkeletonVariant1, EntityType::SkeletonVariant2}},
	{EntityType::Vindicator, {EntityType::VindicatorVariant0, EntityType::VindicatorVariant1, EntityType::VindicatorVariant2}},
	{EntityType::Zombie,     {EntityType::ZombieVariant0, EntityType::ZombieVariant1, EntityType::ZombieVariant2}},
	{EntityType::Mountaineer,{EntityType::MountaineerVariant0, EntityType::MountaineerVariant1, EntityType::MountaineerVariant2}},
	{EntityType::PiglinMelee,{EntityType::PiglinMeleeVariant0, EntityType::PiglinMeleeVariant1, EntityType::PiglinMeleeVariant2}},
	{EntityType::PiglinRanged,{EntityType::PiglinRangedVariant0, EntityType::PiglinRangedVariant1, EntityType::PiglinRangedVariant2}},
	{EntityType::ZombifiedPiglinMelee,  {EntityType::ZombifiedPiglinMeleeVariant0, EntityType::ZombifiedPiglinMeleeVariant1, EntityType::ZombifiedPiglinMeleeVariant2}},
	{EntityType::ZombifiedPiglinRanged, {EntityType::ZombifiedPiglinRangedVariant0, EntityType::ZombifiedPiglinRangedVariant1, EntityType::ZombifiedPiglinRangedVariant2}},
	{EntityType::Panda, {EntityType::LazyPanda, EntityType::PlayfulPanda, EntityType::BrownPanda, EntityType::BabyPanda}},
	{EntityType::Drowned,{EntityType::DrownedVariant0, EntityType::DrownedVariant1, EntityType::DrownedVariant2}},
	{EntityType::TridentDrowned,{EntityType::TridentDrownedVariant0, EntityType::TridentDrownedVariant1, EntityType::TridentDrownedVariant2}},
	{EntityType::SunkenSkeleton,{EntityType::SunkenSkeletonVariant0, EntityType::SunkenSkeletonVariant1, EntityType::SunkenSkeletonVariant2}},
};

const EntityTypeMapper& NoVariants() {
	static const EntityTypeMapper mapper = [](EntityType type) {
		const auto it = TypeToVariants.find(type);
		return it != TypeToVariants.end() ? it->second[0] : type;
	};
	return mapper;
}

EntityTypeMapper WeightedVariants(std::vector<float> weights, Random* rnd /*= nullptr*/) {
	return [ws = std::move(weights), rnd = Util::thisOrSharedRandom(rnd)](EntityType type) {
		auto wt = algo::sum(ws); //D11.PS moved this into here as the initalisation order by the lambda capture is no guaranteed
		auto it = TypeToVariants.find(type);
		if (it == TypeToVariants.end()) {
			return type;
		}
		ensure(ws.size() && wt > 0 && ws.size() == it->second.size());
		return it->second[Util::randomWeightedItemIndex(ws, wt, rnd)];
	};
}

EntityTypeMapper Only(EntityType type) {
	return [type](EntityType) { return type; };
}

EntityTypeMapper FromPredicateAndMapper(::Pred<EntityType> predicate, EntityTypeMapper mapper) {
	return [pred = std::move(predicate), mapper = std::move(mapper)](EntityType type) {
		return pred(type) ? mapper(type) : type;
	};
}

EntityTypeMapper CascadedBreakWhenTypeChanged(std::vector<EntityTypeMapper> mappers) {
	return [mappers = std::move(mappers)](EntityType type) {
		for (auto& mapper : mappers) {
			const auto mappedType = mapper(type);
			if (mappedType != type) {
				return mappedType;
			}
		}
		return type;
	};
}

std::vector<float> lerp(const std::vector<float> a, const std::vector<float> b, float fraction) {
	ensure(a.size() == b.size());

	std::vector<float> res;
	for (size_t i = 0; i < a.size(); ++i) {
		res.push_back(FMath::Lerp(a[i], b[i], fraction));
	}
	return res;
}

std::vector<float> createDefaultVariantWeightsFor(const FDifficulty& difficulty) {
	switch (difficulty.chosen()) {
	case EGameDifficulty::Difficulty_1: 
		return { 85, 10, 5 };
	case EGameDifficulty::Difficulty_2: 
		return lerp({85, 10,  5}, {79, 14, 7}, difficulty.missionDifficultyFraction());
	case EGameDifficulty::Difficulty_3: 
		return lerp({ 75, 17, 8 }, {60, 25, 15}, difficulty.endlessStruggleFraction());
	default:
		checkNoEntry();
		return {};
	}
}

int getRandomIndex(std::vector<float> weights, Random* rnd) {
	int index = -1;
	float total = 0.0f;
	for (auto weight : weights) {
		total += weight;
	}
	float randomWeight = rnd->nextFloat(total);
	float tracker = 0.0f;
	for (auto weight : weights) {
		tracker += weight;
		index++;
		if (tracker > randomWeight) {
			break;
		}
	}
	return index;
}

TArray<EntityType> AllVariants(EntityType type) {
	const auto it = TypeToVariants.find(type);
	if(it != TypeToVariants.end()) {
		return TArray<EntityType>(it->second.data(), it->second.size());
	}

	return {type};
}

EntityTypeMapper DefaultVariants(const FDifficulty& difficulty, Random* rnd /*= nullptr*/) {
	const auto pandaMapper = WeightedVariants({ 0.4f, 0.45f, 0.05f, 0.1f }, rnd);
	
	return CascadedBreakWhenTypeChanged({
		[rnd = Util::thisOrSharedRandom(rnd), pandaMapper](EntityType type) { 
			switch (type) {
				case EntityType::ChickenJockey:
					return rnd->testProbability(0.01f) ? EntityType::ChickenJockeyTower : EntityType::ChickenJockey;
				case EntityType::Panda: // D11.DB
					return pandaMapper(type);
				default:
					return type;
			}
		},
		WeightedVariants(createDefaultVariantWeightsFor(difficulty), rnd)
	});
}

}}

