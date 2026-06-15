#include "Dungeons.h"
#include "game/item/ItemType.h"
#include "ItemStatsUtil.h"
#include "game/item/instance/AItemInstance.h"
#include "DungeonsGameInstance.h"


TMap<ItemTag, const game::item::stats::FItemStatsTotals> UItemStatsUtil::ItemStatsTotalsCache;
TMap<FItemId, const TArray<FItemStatsEntry>> UItemStatsUtil::ItemTypeStatsEntryCache;
const int UItemStatsUtil::MAX_SCORE = 14;

TArray<FItemStatsEntry> UItemStatsUtil::GetItemStats(UObject* WorldContextObject, const FSerializableItemId& itemType) {
	auto* instance = WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	return instance->GetInventoryItemDataHolder()->GetStats(itemType);
}

TArray<FItemStatsEntry> UItemStatsUtil::CreateItemStats(const FItemId& ItemTypeId) {
	TArray<FItemStatsEntry> StatsEntries = ItemTypeStatsEntryCache.Contains(ItemTypeId) ? ItemTypeStatsEntryCache[ItemTypeId] : TArray<FItemStatsEntry>();
	if (StatsEntries.Num() <= 0) {
		auto&& type = GetItemRegistry().Get(ItemTypeId);
		auto&& ItemStatsTypes = GetItemStatsTypes(ItemTypeId);
		
		if(ItemStatsTypes.Num() > 0) {

			auto&& tag = type.getTag();
			auto&& totals = ItemStatsTotalsCache.Contains(tag) ? ItemStatsTotalsCache[tag] : game::item::stats::getItemStatsTotals([&](const ItemType& t) { return t.getTag() == tag; }, ItemStatsTypes, MAX_SCORE);
			ItemStatsTotalsCache.Add(tag, totals);

			game::item::stats::FItemStatsScoreTotals scores = totals.GetItemStatsScores(ItemTypeId);
			for (auto score : scores.scores) {
				StatsEntries.Add({
					score.StatType,
					score.ScoreInt,
					score.ScoreFloat,
					totals.maxScore,
					GetItemStatsDisplayName(score.StatType),
					GetItemStatsDisplayNameLow(score.StatType),
					GetItemStatsDisplayNameHigh(score.StatType)
					});
			}
		}

		ItemTypeStatsEntryCache.Add(ItemTypeId, StatsEntries);
	}
	return StatsEntries;
}



TArray<EItemStats> UItemStatsUtil::GetItemStatsTypes(const FItemId& ItemTypeId) {
	auto&& type = GetItemRegistry().Get(ItemTypeId);
	switch (type.getTag()) {
	case ItemTag::RangedWeapon:
		return {
			EItemStats::AverageDamage,
			EItemStats::AttackSpeed,
			EItemStats::QuiverAmmo
		};
	case ItemTag::MeleeWeapon:
		return {
			EItemStats::AverageDamage,
			EItemStats::AttackSpeed,
			EItemStats::AreaDamage
		};	
	default:
		return {};
	}
}

#define LOCTEXT_NAMESPACE "ItemStats"
FText UItemStatsUtil::GetItemStatsDisplayName(EItemStats ItemStatsID) {
	switch (ItemStatsID) {
	case EItemStats::AverageDamage:
		return LOCTEXT("AverageDamage", "Power");
	case EItemStats::AttackSpeed:
		return LOCTEXT("AttackSpeed", "Speed");
	case EItemStats::QuiverAmmo:
		return LOCTEXT("QuiverAmmo", "Ammo");
	case EItemStats::AreaDamage:
		return LOCTEXT("AverageArea", "Area");
	case EItemStats::RechargeSpeed:
		return LOCTEXT("RechargeSpeed", "Recharge");
	default:
		return LOCTEXT("Unknown", "Unknown");
	}
}

FText UItemStatsUtil::GetItemStatsDisplayNameLow(EItemStats ItemStatsID) {
	switch (ItemStatsID) {
	case EItemStats::RechargeSpeed:
		return LOCTEXT("RechargeSpeed_Low", "Slow");
	default:
		return FText();
	}
}

FText UItemStatsUtil::GetItemStatsDisplayNameHigh(EItemStats ItemStatsID) {
	switch (ItemStatsID) {
	case EItemStats::RechargeSpeed:
		return LOCTEXT("RechargeSpeed_High", "Fast");
	default:
		return FText();
	}
}
#undef LOCTEXT_NAMESPACE