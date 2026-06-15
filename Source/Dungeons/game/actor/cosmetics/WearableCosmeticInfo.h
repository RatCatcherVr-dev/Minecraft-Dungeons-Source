#pragma once

#include "game/component/PaperdollComponent.h"
#include "CosmeticItemInfo.h"
#include "WearableCosmetic.h"
#include "WearableCosmeticInfo.generated.h"

UCLASS(Blueprintable)
class DUNGEONS_API AWearableCosmeticInfo : public ACosmeticItemInfo {

	GENERATED_BODY()

public:
	void Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh) override;

	void Unequip() override;
	
	TSubclassOf<AActor> GetChildActorClass() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AWearableCosmetic> WearableCosmeticClass;
private:
	UPROPERTY()
	TArray<AWearableCosmetic*> EquippedCosmetics;
};