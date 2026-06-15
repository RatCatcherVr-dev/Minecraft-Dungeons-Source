#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemSlot.h"
#include "instance/RangedWeaponGearItemInstance.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include <UserWidget.h>
#include "ArrowItemSlot.generated.h"

USTRUCT()
struct FSourceItemStack {
	GENERATED_BODY()

	FSourceItemStack();
	FSourceItemStack(FGameplayEffectSpec&& spec, int stack);
	FSourceItemStack(const FGameplayEffectSpec& spec, int stack);

	FGameplayEffectSpec SpecToApply;

	UPROPERTY()
	int Stack;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UArrowItemSlot : public UItemSlot
{
	GENERATED_BODY()

	UArrowItemSlot();
public:
	void BeginPlay() override;

	/**
	Returns the conversion factor from default to the current stack size (useful for calculations assuming default stack size for arrows).
	*/
	float GetStackConversionFactor() const;

	UPROPERTY(EditDefaultsOnly)
	bool ForceUnscaledStackCount = false;


	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UUserWidget> GetAmmoCounterWidgetClass() const;

	int GetCount() const override;

	void ConsumeAmount(int32 amount) override;

	void AddSourceItem(FSourceItemStack&& itemStack, const FItemId& type, bool fixedAmount, EEquipmentSource source);

	const FSourceItemStack* GetTopSourceItem() const;

	void SetAsActiveArrowSlot();
	void SetAsInactiveArrowSlot();

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UUserWidget> GetAmmoCounterLiteContentWidgetClass() const;

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class UUserWidget> GetAmmoCounterLiteOverlayWidgetClass() const;

	void EquipFixedAmount(const FInventoryItemData& itemData, int count /*= 1*/, EEquipmentSource source = EEquipmentSource::Inventory);

	bool IsRangedOverride() const { return RangedOverride; }

protected:
	UPROPERTY(EditAnywhere)
	bool RangedOverride = false;

	void SourceItemAdded(const FItemId& type, bool fixedAmount);
	void EmptySourceItems();

	void UpdateGameplayEffectSpec();

	UAbilitySystemComponent* GetOwnerAbilitySystem();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> AmmoCounterWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> AmmoCounterLiteContentWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> AmmoCounterLiteOverlayWidgetClass;

	void EquipItem_Implementation(const FInventoryItemData& itemData, int count = 1, EEquipmentSource source = EEquipmentSource::Inventory) override;
private:
	bool IsActive = false;
	int ProjectileStackCount;
	int DefaultStackCount;

	FActiveGameplayEffectHandle CurrentEffectHandle;
	TWeakObjectPtr<UAbilitySystemComponent> OwnerAbilitySystem;

	UFUNCTION()
	void OnRangedWeaponUpdated(UItemSlot* itemSlot);

	TArray<FSourceItemStack> SourceItemStacks;
	TWeakObjectPtr<ARangedWeaponGearItemInstance> EquippedRangedWeapon;
};
