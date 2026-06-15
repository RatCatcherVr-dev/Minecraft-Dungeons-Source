// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsAwardGenerator.h"
#include "util/telemetry/AnalyticsDataTypes.h"
#include "util/telemetry/AnalyticsReflection.h"

#include <Containers/Map.h>
#include <algorithm>
#include <numeric>
#include "StringUtil.h"

namespace awards {
namespace {
	using Scorer = std::function<double(const DungeonsPlayerStatTracker&)>;
	using Thresholder = std::function<bool(EAwardType, double)>;
	using AwardGenerator = std::function<void(AwardResult&, const TMap<int, DungeonsPlayerStatTracker>&, Thresholder)>;

	template<typename T> unsigned Accumulate(const TMap<T, unsigned>& container, std::function<bool(T)> predicate) {
		unsigned sum = 0;
		if (predicate) {
			for (auto it = container.CreateConstIterator(); it; ++it) {
				if (predicate(it->Key)) sum += it->Value;
			}
		} else {
			for (auto it = container.CreateConstIterator(); it; ++it) {
				sum += it->Value;
			}
		}

		return sum;
	}

	double BlockScorer(const DungeonsPlayerStatTracker& tracker, std::function<bool(EMaterialTypeEnum)> pred) {
		return static_cast<double>(Accumulate(tracker.GetBlockTypesWalkedOn(), pred));
	}
	double MobScorer(const DungeonsPlayerStatTracker& tracker, std::function<bool(EntityType)> pred) {
		return static_cast<double>(Accumulate(tracker.GetKilledMobs(), pred));
	}
	double ItemScorer(const DungeonsPlayerStatTracker& tracker, std::function<bool(FItemId)> pred) {
		return static_cast<double>(Accumulate(tracker.GetUsedItems(), pred));
	}
	double ScalarScorer(const DungeonsPlayerStatTracker& tracker, DungeonsPlayerStatTracker::ScalarTrackingTypes type) {
		return tracker.GetScalarValues()[static_cast<size_t>(type)];
	}

	void Generator(AwardResult& out, const TMap<int, DungeonsPlayerStatTracker>& playerStats, Thresholder thresholder, Scorer scorer, EAwardType awardType, const std::function<bool(double, double)>& comparator) {
		std::vector<AwardData> scores;

		for (auto it = playerStats.CreateConstIterator(); it; ++it) {
			const double score = scorer(it->Value);
			if (thresholder(awardType, score)) {
				scores.emplace_back(it->Key, awardType, score);
			}
		}
		if (comparator) {
			if (!scores.empty()) {
				out.best.push_back(*algo::min_element(scores, [&](const AwardData& a, const AwardData& b) { return comparator(a.score, b.score); }));
			}
		} else {
			algo::append_all(out.best, scores);
		}
		algo::append_all(out.all, scores);
	}

	AwardGenerator Awarder(EAwardType type, DungeonsPlayerStatTracker::ScalarTrackingTypes trackingType, std::function<bool(double, double)> comparator = {}) {
		Scorer scorer = std::bind(ScalarScorer, std::placeholders::_1, trackingType);
		return [type, scorer, comparator](AwardResult& out, const TMap<int, DungeonsPlayerStatTracker>& data, Thresholder threshold) {
			Generator(out, data, threshold, scorer, type, comparator);
		};
	}

	template<typename T> AwardGenerator Awarder(EAwardType type, double(*enumScorer)(const DungeonsPlayerStatTracker& tracker, std::function<bool(T)>), std::function<bool(double, double)> comparator = {}, std::function<bool(T)> predicate = {}) {
		Scorer scorer = std::bind(enumScorer, std::placeholders::_1, predicate);
		return [type, scorer, comparator](AwardResult& out, const TMap<int, DungeonsPlayerStatTracker>& data, Thresholder threshold) {
			Generator(out, data, threshold, scorer, type, comparator);
		};
	}

	static const std::vector<AwardGenerator> Generators({
		Awarder(EAwardType::MostDamageDoneSingleBlow, DungeonsPlayerStatTracker::ScalarTrackingTypes::MostDamageDeltInSingleBlow, std::greater<double>()),
		Awarder(EAwardType::MostEnemiesKilled, MobScorer, std::greater<double>()),
		Awarder(EAwardType::MostZombiesKilled, MobScorer, std::greater<double>(), std::function<bool(EntityType)>([](EntityType t) { return t == EntityType::ZombieVariant0 || t == EntityType::ZombieVariant1 || t == EntityType::ZombieVariant2; })),
		Awarder(EAwardType::MostSkeletonsKilled, MobScorer, std::greater<double>(),std::function<bool(EntityType)>([](EntityType t) { return t == EntityType::SkeletonVariant0 || t == EntityType::SkeletonVariant1 || t == EntityType::SkeletonVariant2; })),
		Awarder(EAwardType::MostSpidersKilled, MobScorer, std::greater<double>(),std::function<bool(EntityType)>([](EntityType t) { return t == EntityType::Spider || t == EntityType::CaveSpider; })),
		Awarder(EAwardType::MostDamageDone, DungeonsPlayerStatTracker::ScalarTrackingTypes::DamageDealt, std::greater<double>()),
		Awarder(EAwardType::BlocksWalkedOn, BlockScorer, nullptr),
		Awarder(EAwardType::MostArrowsFired, DungeonsPlayerStatTracker::ScalarTrackingTypes::ProjectileFired, std::greater<double>()),
		Awarder(EAwardType::MostArrowsHit, DungeonsPlayerStatTracker::ScalarTrackingTypes::ProjectilesHit, std::greater<double>()),
		Awarder(EAwardType::StrengthPotionsUsed, ItemScorer, nullptr, std::function<bool(FItemId)>([](FItemId id) { return id == game::item::type::StrengthPotion.getId(); })),
		Awarder(EAwardType::SwiftnessPotionsUsed, ItemScorer, nullptr, std::function<bool(FItemId)>([](FItemId id) { return id == game::item::type::SwiftnessPotion.getId(); })),
		Awarder(EAwardType::TNTUsed, ItemScorer, nullptr, std::function<bool(FItemId)>([](FItemId id) { return id == game::item::type::TNTBox.getId(); })),
		Awarder(EAwardType::HealthPotionsUsed, ItemScorer, nullptr, std::function<bool(FItemId)>([](FItemId id) { return id == game::item::type::HealthPotion.getId(); })),
		Awarder(EAwardType::ItemsUsed, ItemScorer),
		Awarder(EAwardType::LeastDamageTaken, DungeonsPlayerStatTracker::ScalarTrackingTypes::DamageTaken, std::less<double>()),
		
	});
}

awards::AwardResult GenerateAwards(const TMap<int, DungeonsPlayerStatTracker>& playerStats, const TMap<EAwardType, float>& threshhold, ELevelNames level, EGameDifficulty difficulty) {
	awards::AwardResult result;
	const std::vector<AwardGenerator> AwardGenerators = Generators;
	for (auto& generator : AwardGenerators) {
		generator(result, playerStats, [&](EAwardType type, double value) {
			if (type == EAwardType::TNTUsed && !game::item::type::TNTBox.isAllowedOnMissionDifficulty(level, difficulty)) {
				return false;
			}
			return threshhold.Contains(type) ? value > threshhold[type] : true;
		});
	}
	return result;
}

// todo: move into separate interface?
void GeneratePlayerStats(const DungeonsPlayerStatTracker& statTracker, analytics::PlayerStats& outStats) {
	//stats.ScalarStats.reserve(DungeonsPlayerStatTracker::ScalarTrackingTypes::ScalarTrackingTypesSize);
	// todo: string representations of counters
	for (auto mob = statTracker.GetKilledMobs().CreateConstIterator(); mob; ++mob) {
		outStats.MobKills.Add(UTF8_TO_TCHAR(EntityTypeToString(mob->Key).c_str()), (*mob).Value);
	}

	for (auto item = statTracker.GetUsedItems().CreateConstIterator(); item; ++item) {
		outStats.ItemUses.Add(item->Key.GetBackingType().ToString(), (*item).Value);
		// todo: map ItemType to string representation
		//stats.ItemUses.emplace(std::make_pair(EntityTypeToString(item->Key), (*item).Value));
	}

	const std::vector<double>& scalarValues = statTracker.GetScalarValues();
	for (int i = 0; i < (int)DungeonsPlayerStatTracker::ScalarTrackingTypes::ScalarTrackingTypesSize; ++i) {
		std::string statName = DungeonsPlayerStatTracker::toString(static_cast<DungeonsPlayerStatTracker::ScalarTrackingTypes>(i));
		outStats.ScalarStats.Add(UTF8_TO_TCHAR(statName.c_str()), scalarValues.at(i));
	}
}
 
}
