#pragma once

#include "CosmeticItemInfo.h"
#include "world/entity/EntityTypes.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "PetCosmeticInfo.generated.h"

UCLASS(Blueprintable)
class DUNGEONS_API APetCosmeticInfo : public ACosmeticItemInfo {
	
	GENERATED_BODY()
public:
	APetCosmeticInfo();

	void Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh) override;

	void Unequip() override;

	void Tick(const float deltaTime) override;

protected:
	void OnSpawned(AMobCharacter*);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EntityType Mob;

private:
	TWeakObjectPtr<AMobCharacter> target;
	
	bool TrySpawnPet();

	void Relocate();

	FTimerHandle relocateHandle;
};