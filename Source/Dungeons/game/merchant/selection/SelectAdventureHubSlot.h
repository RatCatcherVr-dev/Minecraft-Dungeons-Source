#pragma once
#include "MerchantSelectionBase.h"
#include "game/item/ItemTypeDefs.h"
#include "online/seasons/LiveOps.h"
#include "SelectAdventureHubSlot.generated.h"

class UInventoryItemSlot;

UENUM(BlueprintType)
enum class EMonetizationType : uint8 {
	Free,
	Paid
};

UENUM(BlueprintType)
enum class EAdventureHubSlotState : uint8 {
	Unavailable,
	Locked,
	Unlocked,
	Claimed
};

UENUM(BlueprintType)
enum class EAdventureHubRewardType : uint8 {
	Cosmetic,
	Item,
	ActionPoints,
	None
};

UCLASS(BlueprintType)
class DUNGEONS_API UAdventureHubSlot : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	EMonetizationType MonetizationType = EMonetizationType::Free;
	UPROPERTY(BlueprintReadOnly)
	EAdventureHubSlotState SlotState = EAdventureHubSlotState::Unavailable;
	UPROPERTY(BlueprintReadOnly)
	EAdventureHubRewardType RewardType = EAdventureHubRewardType::Cosmetic;
	UPROPERTY(BlueprintReadOnly)
	FName Id;
	UPROPERTY(BlueprintReadOnly)
	int Level = 0;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UTexture2D* GetIcon(UObject* worldContext) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetDisplayName(UObject* worldContext) const;
};

UCLASS(BlueprintType)
class DUNGEONS_API USelectAdventureHubSlot : public UMerchantSelectionBase {
	GENERATED_BODY()

private:
	int mLastChangeIndex = 0;

	void UpdateSelectionCache(const online::liveops::SeasonView& season);

	UPROPERTY(Transient)
	TArray<UAdventureHubSlot*> SelectionCache;

	FDelegateHandle DataUpdateHandle;
protected:
	ESlotType mUISlotTypeIcon = ESlotType::Any;

	UPROPERTY()
	UAdventureHubSlot* mSelectedHubSlot;

	void OnSetupSession() override;
	void OnCleanupSession() override;
public:
	void EnsureSelection() override;
	void ClearSelection() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UAdventureHubSlot* GetHubSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<UAdventureHubSlot*>& GetHubSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SelectHubSlot(UAdventureHubSlot* slot);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool PollHasSelectablesChanged();

	bool CanSelectAny() const override;
	bool HasSelectedAny() const override;

	void OnDataUpdate(online::liveops::UpdateRequestStatus status, const online::liveops::SeasonView& season);
};
