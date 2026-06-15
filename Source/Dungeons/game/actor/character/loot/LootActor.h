#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "game/actor/character/loot/LootDefs.h"
#include "game/actor/PropActor.h"
#include "GameplayTagContainer.h"
#include "LootActor.generated.h"

class APlayerCharacter;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnOpenLoot, const FVector&, AActor*, AActor*);
DECLARE_MULTICAST_DELEGATE(FOnActivatedLoot);

UCLASS()
class DUNGEONS_API ALootActor : public APropActor {
	GENERATED_BODY()

public:
	ALootActor();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimationAsset* unlockingAnim;
	
	// isaveg: offset to guarantee item loot never spawns bellow ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FVector AboveGroundOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool SpawnUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool OnlyOwnerCanLoot = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class USceneComponent* RootSceneComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Dungeons")
	class UReplicatedInteractableComponent* InteractableComp;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Open(APlayerCharacter* playerCharacter);
	bool IsOpen() const { return bLootUnlocked; }

	FOnOpenLoot OnOpenLoot;
	FOnActivatedLoot OnActivatedLoot;

protected:

	UFUNCTION()
	void OnRep_ActivatedLoot();

	UPROPERTY(EditDefaultsOnly, Transient, Replicated)
	class UMapPinComponent* MapPinComponent;

private:
	class USkeletalMeshComponent* MeshComp;
	void LazyLoadMeshComponent();
	static void RemoveAndDestroyComponent(UActorComponent* Component);
	
	UPROPERTY(ReplicatedUsing = OnRep_ActivatedLoot)
	bool bLootUnlocked;

	float mLockingDelay;
	bool bLockingDelayed;

	void _onInteract(AActor* instigator);

	UPROPERTY(EditDefaultsOnly)
	float DropVelocityMinMultiplier;

	UPROPERTY(EditDefaultsOnly)
	float DropVelocityMaxMultiplier;

	UPROPERTY(EditDefaultsOnly)
	float DropConeRadiusMultiplier;
};
