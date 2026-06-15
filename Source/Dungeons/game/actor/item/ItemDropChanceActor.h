 #pragma once

#include "GameFramework/Info.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/component/ItemStashComponent.h"
#include "game/item/drop/ItemDropData.h"
#include "ItemDropChanceActor.generated.h"

class UItemSlot;
class APlayerCharacter;

struct FPlayerDropData {

	TOptional<float> HealthFraction;
	float ConsumableUseTimestamp;
	TOptional<float> ArrowLimitTimestamp;

	TOptional<float> Get(const EDropCategory category)  const {
		switch (category)
		{
		case EDropCategory::Food:
			return HealthFraction;
		case EDropCategory::Consumable:
		case EDropCategory::ConsumableNoWaterBreathing:
			return ConsumableUseTimestamp;
		case EDropCategory::Arrow:
			return ArrowLimitTimestamp;
		default:
			return TOptional<float>();
		}
	}

	void Reset(const EDropCategory category, const float now) {
		switch (category)
		{
		case EDropCategory::Consumable:
		case EDropCategory::ConsumableNoWaterBreathing:
			ConsumableUseTimestamp = now;
		case EDropCategory::Arrow:
			if (ArrowLimitTimestamp.IsSet()) {
				ArrowLimitTimestamp = now;
			}
			break;
		default:
			break;
		}
	}
};

/**
 * World global actor that keeps track of player drops and provides logic for rolling drops.
 */
UCLASS()
class DUNGEONS_API AItemDropChanceActor : public AInfo {
	GENERATED_BODY()

public:
	AItemDropChanceActor(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;

	void Disable() { mDisabled = true; }
	
	TArray<FNetworkedItemDropData> RollForItemDropData(const FItemDropSource& dropSource);

	void RegisterPlayer(APlayerCharacter* playerCharacter);
	void UnRegisterPlayer(APlayerCharacter* playerCharacter);
protected:
	/** The arrow count at which point the drop rate of arrows will start to increase toward the max drop rate (over time) */
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int32 ArrowCountDropLimit = 10;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TArray<FDropCategoryDescription> Drops;

	UFUNCTION()
	void OnSlotCountChanged(UItemSlot* slot);	

	UFUNCTION()
	void OnPlayerHealthChanged(float amount, APlayerCharacter* playerCharacter);
private:	
	TArray<FDropCategoryDescription> RollForDropCategories(const FItemDropSource& dropSource);
	float CalculateDropProbability(const FDropCategoryDescription& drop, APlayerCharacter* playerCharacter);
	static bool IsRollSuccessful(const FDropCategoryDescription& drop, float probability);
	static bool IsRerollSuccessful(const FDropCategoryDescription& drop, const FItemDropSource& dropSource, float probability);
	void ResetDroprate(const FDropCategoryDescription& drop, APlayerCharacter* playerCharacter);
	bool IsAllowedToRoll() const;

	void OnActorUsedItem(const AActor* actor, const class AItemInstance* itemInstance);	
	void OnArrowSlotChanged(UItemSlot* slot, APlayerCharacter* playerCharacter);

	bool mDisabled;
	game::CachedDifficultyStats mDifficultyStats;
	TMap<const APlayerCharacter*, FPlayerDropData> DroprateData;
};
