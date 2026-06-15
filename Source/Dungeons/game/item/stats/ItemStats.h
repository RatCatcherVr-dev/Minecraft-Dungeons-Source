#pragma once
#include "game/item/ItemTypeDefs.h"
#include "game/item/ItemType.h"
#include "game/item/stats/ItemStatsTypes.h"

class AItemInstance;

namespace game { namespace item { namespace stats {			

	struct FItemStatsScore
	{
		int ScoreInt = -1;
		float ScoreFloat = 0.0f;
		float MinValue = 0.0f;
		float MaxValue = 0.0f;
		float Value = 0.0f;
		EItemStats StatType = EItemStats::Invalid;
	};

	struct FItemStatsScoreTotals
	{
		TArray<FItemStatsScore> scores;
		float GetAverageRating() const;

	};

	struct FItemStatsTotals
	{	
		const int maxScore;
		TArray<EItemStats> stats;
		TMap<FItemId, TWeakObjectPtr<AItemInstance>> generatedItems;
		TMap<EItemStats, float> mins;
		TMap<EItemStats, float> maxs;

	public:
		FItemStatsScoreTotals GetItemStatsScores(const AItemInstance& item) const;
		FItemStatsScoreTotals GetItemStatsScores(const FItemId& ItemTypeId) const;
		void PrintAllTo(FOutputDevice& out) const;

	private:
		FItemStatsScore GetItemStatScore(const AItemInstance& item, EItemStats estimateType) const;
	};

	using ItemTypeFilterPredicate = std::function<bool(const ItemType&)>;

	FItemStatsTotals getItemStatsTotals(ItemTypeFilterPredicate filterPredicate, const TArray<EItemStats>& stats, int maxScore = 10);
}}}