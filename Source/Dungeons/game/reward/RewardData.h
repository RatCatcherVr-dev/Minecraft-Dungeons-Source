#pragma once

#include "game/item/InventoryItemData.h"
#include "RewardData.generated.h"

struct FInventoryItemData;

UENUM(BlueprintType)
enum class ERewardType : uint8
{
	LevelCompletion,
	EmergentDifficulty,
};
ENUM_NAME(ERewardType);

USTRUCT(BlueprintType)
struct DUNGEONS_API FRewardData {
	GENERATED_USTRUCT_BODY()

	FRewardData();
	FRewardData(ERewardType, FInventoryItemData);

public:
	UPROPERTY(BlueprintReadOnly)
	ERewardType RewardType;
	FInventoryItemData ItemData;
};

UCLASS()
class DUNGEONS_API URewardDataFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FInventoryItemData& GetInventoryItemDataFromRewardData(const FRewardData& rewardData);
};

namespace rewardquery {
	int GetMaxEmergentDifficultyCollectedTokens();
}
