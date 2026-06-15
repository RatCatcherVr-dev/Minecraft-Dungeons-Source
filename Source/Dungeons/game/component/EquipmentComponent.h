#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../item/ItemSlot.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/difficulty/DifficultyRecommendation.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTotalEquippedItemPowerChanged);
DECLARE_MULTICAST_DELEGATE(FTotalEquippedItemPowerChangedInternal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnySlotChanged, UItemSlot*, Slot);

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UEquipmentComponent();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type endPlayReason) override;

	UFUNCTION(BlueprintCallable)
	TArray<UItemSlot*> GetSlotsOfType(ESlotType type) const;

	UItemSlot& GetSlot(const ESlotType& slotType) const;
	UItemSlot* GetFirstSlotOfType(const ESlotType& slotType) const;

	TArray<UItemSlot*> GetGearSlots() const;
	TArray<UItemSlot*> GetEquippableSlots() const;

	void OnPawnPossessed() const;
	
	void ResetEquipment() const;

	UFUNCTION() 
	void OnEquippableItemSlotUpdated(UItemSlot* itemSlot) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetTotalEquippedDisplayItemPower() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FTotalEquippedItemPowerChanged OnTotalEquippedItemPowerChanged;

	FTotalEquippedItemPowerChangedInternal OnTotalEquippedItemPowerChangedInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAnySlotChanged OnAnySlotChanged;

	int GetMissionStartingEquippedGearPower() const;
	void SetMissionStartingEquippedGearPower(int startingGearPower);
	bool HasHealthPotionEquipped() const;
	TArray<float> GetCooldowns() const;

	const game::DifficultyRecommendation& GetDifficultyRecommendation();

	void ResetDifficultyRecommendation();

	UFUNCTION()
	void OnMissionDifficultyIncreased(int difficultyLevelIncreased);
protected:
	UPROPERTY(Replicated)
	mutable TArray<UItemSlot*> EquipmentSlots;		
	
	void LazyLoadSlots() const;

	void OnPlayerDown() const;

	UFUNCTION()
	void OnRespawn() const;	

private:
	int CalculateTotalEquippedDisplayItemPower() const;
	void RefreshTotalEquippedDisplayItemPower() const;

	void RefreshDifficultyRecommendation();

	TArray<UItemSlot*> GetEquipmentSlots() const;

	TArray<UItemSlot*> GetSlots(std::function<bool(ESlotType type)> predicate) const;

	static bool IsGearSlot(const ESlotType& type);
	static bool IsEquippableSlot(const ESlotType& type);

	mutable float mCachedEquippedDisplayItemPower = 0;

	int mMissionStartingGearPower = 0;
	
	TOptional<game::DifficultyRecommendation> mDifficultyRecommendation;
};
