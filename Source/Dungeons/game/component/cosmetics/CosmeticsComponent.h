#pragma once

#include "Components/ActorComponent.h"
#include "DungeonsGameInstance.h"
#include "game/cosmetics/CosmeticType.h"
#include "CosmeticsSlot.h"
#include "game/cosmetics/EntitledCosmetic.h"
#include "CosmeticsComponent.generated.h"

class UCosmeticItemDef;
struct CosmeticsSaveData;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UCosmeticsComponent : public UActorComponent {
	
	GENERATED_BODY()

public:	
	UCosmeticsComponent();	

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type endPlayReason) override;
	TArray<FEntitlement> GetEntitlements() const;

	void OnLocalPawnPossessed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<UCosmeticsSlot*>& GetSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<UCosmeticItemDef*> FindAvailableCosmeticTypeDefinitions(ECosmeticType cosmeticType) const;

	UCosmeticItemDef* FindCosmeticDefinitionById(const FName&) const;

	void Equip(const FName&) const;

	void UnequipAll();
	void Unequip(ECosmeticType);

	void OnCosmeticSlotEquippedCosmetic(UCosmeticsSlot*);
	bool HasEverEquippedCosmetic(const FName&) const;

	bool HasAnyCosmetics() const;
private:
	void OnEntitlementsProvided(const TArray<FEntitlement>&) const;
	void LoadSlots();
	
	bool IsOwnedByLocalPlayer() const;

	UCosmeticsSlot* GetFirstSlotOfType(ECosmeticType) const;
	UCosmeticsSlot* GetIndexedSlotOfType(ECosmeticType, int) const;
	
	void SerializeSaveState();
	void SerializeSaveState(const TArray<UCosmeticItemDef*>&) const;
	void SerializeSaveState(const TArray<CosmeticsSaveData>&) const;

	void DeserializeSaveState() const;

	void ClearSlots();
	void UnequipSlots();
	void LoadCosmeticsIntoSlots(const TArray<CosmeticsSaveData>&) const;
	
	UDungeonsGameInstance* GetDungeonsGameInstance() const;
	
	bool DoesOwnerHaveAuthority() const;

	UPROPERTY(Replicated)
	TArray<UCosmeticsSlot*> Slots;
};
