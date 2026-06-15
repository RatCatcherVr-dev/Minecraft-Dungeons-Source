#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../item/ItemSlot.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/cosmetics/SlotActorManager.h"
#include "EquipmentDisplayComponent.generated.h"

class AGearActor;

UCLASS(BlueprintType)
class DUNGEONS_API UEquipmentDisplayComponent : public UActorComponent {

	GENERATED_BODY()
public:
	UEquipmentDisplayComponent();

	static FName SlotTypeToComponentTag(ESlotType type);

	void BeginPlay() override;	
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void SetDefaultAnimationsInstance(TSubclassOf<UAnimInstance> defaultAnimationsInstance);

	void BindToGearSlots(TArray<UItemSlot*> gearSlots);
	
	TArray<UItemSlot*> GetBoundGearSlots() const;	

	static void UpdateItemMaterialSettings(USkeletalMeshComponent* skeletalMesh, ESlotType slotType, FItemMaterialSettings itemMaterialSettings);

	void UpdatePlayerAnimation();

	void SetCurrentWeapon(ESlotType slot);
	
	void RestoreWeaponVisibility();

	void HideWeapons();		

	AGearActor* GetItemSlotCurrentGearActor(UItemSlot* itemSlot) const;

	USkeletalMeshComponent* GetPlayerMesh() const;
	void SetOverridingPlayerMesh(USkeletalMeshComponent* playerMesh);	

	void ToggleEquipmentVisibility(ESlotType slot, bool visible);
	void ToggleEquipmentVisibility(const TMap<ESlotType, bool>& visibilitySettings);

protected:
	UFUNCTION()
	void OnGearItemSlotUpdated(UItemSlot* itemSlot);	

	void OnPlayerDeath();

	UFUNCTION()
	void OnRespawn();
	
	void OnAttackStateChanged(EAttackState attackState);

private:
	void UnbindSlots();
	void BindSlots(TArray<UItemSlot*> gearSlots);
	
	TSubclassOf<class UAnimInstance> GetOverrideAnim() const;
	TSubclassOf<UAnimInstance> GetOverrideAnim(const UItemSlot* slot) const;
	
	void UnsetCurrentWeapon();
	void ShowCurrentWeapon();

	bool IsSlotTypeVisible(ESlotType slot) const;	
	void UpdateComponentVisibility(UChildActorComponent* childComponentActor, bool visible) const;

	void UpdateGearActorForSlot(UItemSlot* itemSlot) const;
	
	void OnUpdateItemMaterialSettings(UItemSlot* itemSlot, FItemMaterialSettings itemMaterialSettings) const;
	
	UPROPERTY()
	TMap<ESlotType, bool> SlotTypeVisibility;

	UPROPERTY()
	TArray<UItemSlot*> BoundGearSlots;

	UPROPERTY()
	USkeletalMeshComponent* OverridingPlayerMesh;

	TOptional<ESlotType> CurrentWeapon;

	UPROPERTY()
	USlotActorManager* SlotActorManager;

	UPROPERTY()
	TSubclassOf<UAnimInstance> DefaultAnimationsInstance;
};
