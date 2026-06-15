#include "Dungeons.h"
#include "game/item/ItemUtil.h"
#include "game/item/instance/AItemInstance.h"
#include "util/EnumUtil.h"
#include "game/item/ItemType.h"
#include "ItemStats.h"


namespace game { namespace item { namespace stats {
	TMap<EItemStats, TPair<float, float>> StatClampFactors = { 
		{ EItemStats::AverageDamage, TPair<float, float>(0.f, 160.f) },
		{ EItemStats::AttackSpeed, TPair<float, float>(1.2f, 10.f ) },
		{ EItemStats::AreaDamage, TPair<float, float>(0.f, 230000.f ) },
	};

	FItemStatsTotals getItemStatsTotals(ItemTypeFilterPredicate filterPredicate, const TArray<EItemStats>& stats, int maxScore) {
		FItemStatsTotals totals{ maxScore };

		//copy
		totals.stats = TArray<EItemStats>(stats);
		for(const auto* type : GetItemRegistry().GetValues()) {
			if(!type->isWorkInProgress() && filterPredicate(*type)){
				const auto instance = type->getInstanceClass().GetDefaultObject();
				if (instance) {
					//thewreck: It feels very bad to change the default object - But since we always do 1:1 mapping
					//between itemtype and iteminstance classes - perhaps its ok for now.... 
					//Perhaps we should generate our own cached stats actor instead.					
					totals.generatedItems.Add(type->getId(), instance);
					for (auto statType : stats) {
						const auto itemStats = instance->GetStats(statType);
						const auto est = StatClampFactors.Contains(statType) ? FMath::Clamp(itemStats, StatClampFactors[statType].Key, StatClampFactors[statType].Value) : itemStats;
						if(est >= 0.0f){
							const auto min_value = FMath::Min(totals.mins.Contains(statType) ? totals.mins[statType] : est, est);
							const auto max_value = FMath::Max(totals.maxs.Contains(statType) ? totals.maxs[statType] : est, est);
							totals.mins.Add(statType, min_value);
							totals.maxs.Add(statType, max_value);
						}
					}
				}
			}
		}
		return totals;
	}

	void FItemStatsTotals::PrintAllTo(FOutputDevice& out) const{
		const int padding = 24;
		for (auto pair : generatedItems) {
			if (!pair.Value.IsValid()) {
				continue;
			}
			out.Logf(TEXT("%s\n"), *pair.Key.GetBackingType().ToString());
			const auto scores = GetItemStatsScores(*pair.Value);
			for(auto score : scores.scores) {
				auto Name = GetEnumValueToStringStripped(score.StatType);				
				auto Stripped = Name.RightPad(padding);
				out.Logf(TEXT("\t%s %u\t\t(%1.1f) \t\t, min: %1.1f \t max: %1.1f \t value: %1.1f"), *Stripped, score.ScoreInt, score.ScoreFloat, score.MinValue, score.MaxValue, score.Value);
			}
			out.Logf(TEXT("\t\tRating: %1.1f\n"), scores.GetAverageRating());
		}
	}

	FItemStatsScore FItemStatsTotals::GetItemStatScore(const AItemInstance& item, EItemStats statType) const {
		ensure(stats.Contains(statType));

		const auto itemStats = item.GetStats(statType);
		const auto Value = StatClampFactors.Contains(statType) ? FMath::Clamp(itemStats, StatClampFactors[statType].Key, StatClampFactors[statType].Value) : itemStats;
		if (Value >= 0) {
			const auto MinValue = mins[statType];
			const auto MaxValue = maxs[statType];
			const auto valueRange = MaxValue - MinValue;
			const auto inRangeFraction = (Value - MinValue) / valueRange;
			const auto NormalizedValue = 1.0 + inRangeFraction * ((float)maxScore - 1.0f);
			const int Score = FMath::RoundToInt(NormalizedValue);

			FItemStatsScore score;
			score.ScoreInt = Score;
			score.ScoreFloat = NormalizedValue;
			score.MinValue = MinValue;
			score.MaxValue = MaxValue;
			score.Value = Value;

			score.StatType = statType;

			return score;
		}
		return FItemStatsScore();
	}

	FItemStatsScoreTotals FItemStatsTotals::GetItemStatsScores(const AItemInstance& item) const {
		TArray<FItemStatsScore> scores;
		for (auto stat : stats) {
			auto score = GetItemStatScore(item, stat);
			if(score.Value >= 0.0f){
				scores.Add(score);
			}
		}
		FItemStatsScoreTotals totals{ scores };
		return totals;
	}

	FItemStatsScoreTotals FItemStatsTotals::GetItemStatsScores(const FItemId& ItemTypeId) const {
		auto&& itemInstance = generatedItems.Contains(ItemTypeId) ? generatedItems[ItemTypeId] : nullptr;
		if(itemInstance.IsValid()){
			return GetItemStatsScores(*itemInstance);
		}
		return {};
	}

	float FItemStatsScoreTotals::GetAverageRating() const {
		float avgEstimate = 0.0;
		float numEstimates = 0.0;
		for (auto score : scores) {
			float mul = [&](){
				switch (score.StatType) {
					case EItemStats::HitSnappiness :
						return 3.0f;
					case EItemStats::AttackSpeed:
						return 2.0f;
					case EItemStats::AverageArea:
						return 0.75f;
					case EItemStats::PushbackStr:
						return 0.25f;
					default:
						return 1.0f;
				}				
			}();

			avgEstimate += score.ScoreFloat * mul;
			numEstimates += mul;
		}
		return avgEstimate / numEstimates;
	}
}}}
