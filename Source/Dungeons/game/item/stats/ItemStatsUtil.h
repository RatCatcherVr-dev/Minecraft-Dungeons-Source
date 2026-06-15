#pragma once
#include "game/item/stats/ItemStats.h"
#include "game/item/ItemType.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/SerializableItemId.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ItemStatsUtil.generated.h"


class AItemInstance;

USTRUCT(BlueprintType)
struct DUNGEONS_API FItemStatsEntry {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EItemStats ItemStatsID;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int ScoreInt;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	float ScoreFloat;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	int ScoreMax;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText DisplayNameLow;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText DisplayNameHigh;

};


UCLASS()
class DUNGEONS_API UItemStatsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TArray<FItemStatsEntry> GetItemStats(UObject* WorldContextObject, const FSerializableItemId& ItemTypeId);
	static TArray<FItemStatsEntry> CreateItemStats(const FItemId& ItemTypeId);
	static const int MAX_SCORE;
private:
	static TArray<EItemStats> GetItemStatsTypes(const FItemId& ItemTypeId);
	static FText GetItemStatsDisplayName(EItemStats ItemStatsID);
	static FText GetItemStatsDisplayNameLow(EItemStats ItemStatsID);
	static FText GetItemStatsDisplayNameHigh(EItemStats ItemStatsID);
	
	static TMap<ItemTag, const game::item::stats::FItemStatsTotals> ItemStatsTotalsCache;
	static TMap<FItemId, const TArray<FItemStatsEntry>> ItemTypeStatsEntryCache;	
};