#pragma once

#include <UnrealNames.h>
#include "game/cosmetics/CosmeticType.h"
#include "game/actor/cosmetics/CosmeticItemInfo.h"
#include "game/cosmetics/EntitledCosmetic.h"
#include "CosmeticsSlot.generated.h"

class UCosmeticItemDef;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCosmeticsSlotUpdatedInternal, class UCosmeticsSlot*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCosmeticsSlotEquippedInternal, class UCosmeticsSlot*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCosmeticsSlotItemHighlightChanged, class UCosmeticsSlot*, class UCosmeticItemDef*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCosmeticsSlotItemUpdated);

class APlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UCosmeticsSlot : public UActorComponent {

	GENERATED_BODY()

public:
	UCosmeticsSlot();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ECosmeticType GetType() const;

	void SetType(ECosmeticType type);
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ACosmeticItemInfo* GetItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FName& GetEquippedCosmetic() const;

	const FEntitlement& GetEntitlement() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsCosmeticEquipped(UCosmeticItemDef* cosmeticDefinition) const;
	
	UCosmeticItemDef* FindEquippedCosmeticDef() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UCosmeticItemDef*> GetAvailableCosmetics() const;

	///Will change character save data
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void EquipCosmetic(const FName& cosmeticId);

	///Will change character save data
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void UnequipCosmetic();

	///Just clear the slot - don't affect actual equipping
	void ClearCosmetic();

	void SetCosmetic(const FName& cosmeticId);

	void SetIgnoreEntitlement(bool Ignore) { IgnoreEntitlement = Ignore; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsCosmeticNew(UCosmeticItemDef* cosmeticDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnHighlightedInUi(UCosmeticItemDef* highlightedCosmetic);
		
	FOnCosmeticsSlotUpdatedInternal OnCosmeticsSlotUpdatedInternal;
	FOnCosmeticsSlotEquippedInternal OnCosmeticsSlotEquippedInternal;
	FOnCosmeticsSlotItemHighlightChanged OnCosmeticsSlotHighlightChangedInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnCosmeticsSlotItemUpdated OnCosmeticsSlotItemUpdated;

	
protected:
	UPROPERTY()
	ECosmeticType Type;

	UPROPERTY(Transient)
	ACosmeticItemInfo* Item;

	UPROPERTY(ReplicatedUsing = OnRep_CosmeticReplicated, Transient)
	FEntitledCosmetic Cosmetic;

private:	
	UCosmeticItemDef* FindCosmeticDef(const FName&) const;
	
	TOptional<FEntitlement> GetEntitlementForAvailableCosmetic(const FName&) const;	
	TOptional<FName> EquippingCosmeticId;	

	UFUNCTION(Server, WithValidation, Reliable, Category = "Dungeons")
	void ServerSetCosmetic(const FName& equippedCosmeticId, const FEntitlement& entitlement);
	
	UFUNCTION()
	void OnRep_CosmeticReplicated();

	void UpdateItem();
	
	bool IsValidEntitlement(const FName&, const FEntitlement&) const;

	APlayerCharacter* GetPlayerOwner() const;

	bool bHighlighted;

	bool IgnoreEntitlement = false;
};