#pragma once

#include "CoreMinimal.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/ArrowItemSlot.h"
#include "GameplayEffect.h"
#include "ArrowCraftingItemInstance.generated.h"

class UItemSlot;

UCLASS()
class DUNGEONS_API UArrowCraftingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()

public:
	UArrowCraftingGameplayEffect();
};

UCLASS()
class DUNGEONS_API UArrowCraftingItemPowerGameplayEffect : public UArrowCraftingGameplayEffect {
	GENERATED_BODY()

public:
	UArrowCraftingItemPowerGameplayEffect();
};

UCLASS()
class DUNGEONS_API UArrowCraftingRangedWeaponPowerGameplayEffect : public UArrowCraftingGameplayEffect {
	GENERATED_BODY()

public:
	UArrowCraftingRangedWeaponPowerGameplayEffect();
};


UCLASS()
class DUNGEONS_API AArrowCraftingItemInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:	
	void BeginPlay() override;

	void RemoveEquippedEffects() override;

	AArrowCraftingItemInstance();
	
	void OnItemSlotCountChanged(const UItemSlot* itemSlot);

	int GetDisplayCount() const override;

	bool IsBusy() const override;
	
	bool CanActivate() const override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	void OnCooldownProviderAssigned(ICooldownProvider*) override;
	
	FSerializableItemId craftedArrowItemType;

	int craftedArrowCount = 1;

	bool bShouldEquipFixedAmount = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGameplayTag ArrowEquipEffectTag;

	bool bHasItemPowerCooldownScaling = false;

	TSubclassOf<UArrowCraftingGameplayEffect> Effect;
private:
	void LazyInitializeEquipmentSlot();

	bool OwnerHasRangedWeaponEquipped() const;
	
	void ActivateOnServer();
	void ApplyEquipEffectOnOwner();

	int mHasCraftedArrowsAtCount = -1;
	
	UPROPERTY(Replicated)
	bool bHasCraftedArrows;

	TWeakObjectPtr<UArrowItemSlot> ArrowSlot;

	UPROPERTY()
	UEquipmentComponent* EquipmentComponent;
};
