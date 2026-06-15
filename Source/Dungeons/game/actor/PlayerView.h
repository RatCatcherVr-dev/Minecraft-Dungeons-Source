#pragma once

#include "character/player/PlayerCharacter.h"
#include <Components/SceneCaptureComponent2D.h>
#include "game/item/ItemTypeDefs.h"
#include <GameFramework/Actor.h>
#include <Animation/AnimInstance.h>
#include "PlayerView.generated.h"

class UItemSlot;
class APlayerCharacterSaveSlot;
class UCosmeticsComponent;
class UCosmeticsSlot;

UCLASS(BlueprintType)
class DUNGEONS_API APlayerView : public AActor {
	GENERATED_BODY()

public:
	APlayerView(const FObjectInitializer& objectInitializer);

	void PostInitializeComponents() override;

	void BeginPlay() override;
	
	void Tick(float deltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetPlayerCharacter(APlayerCharacter* playerCharacter);

	UFUNCTION(BlueprintCallable)
	void SetPlayerCharacterSaveSlot(APlayerCharacterSaveSlot* playerCharacterSaveSlot);

	void BindDeferredCharacterSaveSlot();

	UFUNCTION()
	void OnEquippableItemSlotSelected(const UItemSlot* itemSlot);

	UFUNCTION()
	void RefreshSlotItemTypeCounts(UItemSlot* itemSlot);
	UFUNCTION()
	void RefreshSlotCosmeticCounts(UCosmeticsSlot* cosmeticSlot);

	UFUNCTION()
	void OnCosmeticSlotEquipped(UCosmeticsSlot* cosmeticSlot);

	UFUNCTION()
	void OnCosmeticSlotHighlightChanged(UCosmeticsSlot* cosmeticSlot, UCosmeticItemDef* cosmetic);

	UFUNCTION()
	void OnCosmeticSlotUpdated(UCosmeticsSlot* cosmeticSlot);
	
	UFUNCTION()
	void OnEquippableItemSlotChanged(UItemSlot* itemSlot);

	UFUNCTION()
	void OnPlayerHealthFractionChanged(float newFraction, float oldFraction);

	UFUNCTION()
	void OnOtherPlayerHealthFractionChanged(float newFraction, float oldFraction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnItemTypeCountChanged(const FSerializableItemId& itemType, int count);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCosmeticCountChanged(const FName& cosmetic, int count);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnItemTypeSlotSelected(const FSerializableItemId& itemType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnGearItemSlotChanged(UItemSlot* slot);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCosmeticItemEquipped(UCosmeticItemDef* cosmetic);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCosmeticSlotItemHighlightChanged(UCosmeticsSlot* cosmeticSlot, UCosmeticItemDef* cosmetic);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnWasDamaged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnOtherWasDamaged();
	
	UFUNCTION(BlueprintCallable)
	void ToggleActive(bool active);

	UFUNCTION(BlueprintCallable)
	void SpinCharacter(float rotation);

	UFUNCTION(BlueprintCallable)
	void SetPreferredCharacterRotation(float rotation);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ReEquipCape();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void EquipCapeAfterDelay();
	
private:		
	void RestoreCharacterSpin(float deltaSeconds);
	FRotator CalculateSpinRestoreRotation(USceneComponent* playerRootPosition) const;	
	float CalculateUpdatedSpinRestoreInterpolation(float deltaSeconds) const;
	
	void BindPlayerAvatarComponent(UPlayerAvatarComponent* avatarComponent);

	UFUNCTION()
	void ApplyMaterialToMesh(UMaterialInstance* material) const;

	void BindEquipmentComponent(UEquipmentComponent* equipmentComponent);

	void BindCosmeticsComponent(UCosmeticsComponent* cosmeticsComponent);

	void OnGearItemSlotSelected(const UItemSlot* itemSlot);

	void SetPrioritizedSlotType(ESlotType slot);
	bool IsSlotTypeDisplayable(ESlotType slot) const;

	void RefreshPrioritizedSlotVisibility() const;

	void CountReportedItemTypeCounts();
	void CountReportedCosmeticCounts();

	void OnUpdateItemMaterialSettings(UItemSlot* itemSlot, FItemMaterialSettings itemMaterialSettings) const;

	int CountTotalOfItemType(const FItemId& itemType) const;
	int CountTotalOfCosmetic(const FName& cosmeticName) const;

	void SetItemTypeCount(const FItemId& itemType, int count);
	void SetCosmeticCount(const FName& cosmeticName, int count);

	void OnActivated();
	void OnDeactivated();

	void ResetSpinRestoreInterpolation();

	USceneComponent* GetPlayerRootPosition();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Animation")
	TSubclassOf<UAnimInstance> DefaultAnimationsInstance;

	UPROPERTY()
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY()
	UEquipmentDisplayComponent* EquipmentDisplayComponent;

	UPROPERTY()
	UCosmeticsDisplayComponent* CosmeticsDisplayComponent;

	UPROPERTY()
	TArray<UItemSlot*> BoundEquippableSlots;

	UPROPERTY()
	TArray<UCosmeticsSlot*> BoundCosmeticsSlots;

	UPROPERTY()
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY()
	UPlayerAvatarComponent* BoundAvatarComponent;

	UPROPERTY()
	APlayerCharacter* BoundPlayerHitFlash;

	TMap<FItemId, int> ItemTypeCounts;
	TMap<FName, int> CosmeticCounts;
	ESlotType PrioritizedSlotType = ESlotType::MeleeWeapon;

	UPROPERTY()
	APlayerCharacter* ViewedPlayer;

	UPROPERTY()
	APlayerCharacterSaveSlot* pDeferredPlayerCharacterSaveSlot;

	bool bViewedPlayerUpdated;

	bool bActive;

	static const float SPIN_RESTORE_SPEED;

	float mPreferredRotation;

	float SpinRestoreInterpolation;

	FName capeCosmeticID;
	UCosmeticsSlot* capeCosmeticSlot;
private:
	void RefreshWasDamagedBind();
};