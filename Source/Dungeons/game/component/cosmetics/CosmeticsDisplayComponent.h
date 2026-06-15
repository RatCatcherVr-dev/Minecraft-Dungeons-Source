#pragma once

#include "CoreMinimal.h"
#include "CosmeticsSlot.h"
#include "game/cosmetics/SlotActorManager.h"
#include "CosmeticsDisplayComponent.generated.h"

UCLASS(BlueprintType)
class DUNGEONS_API UCosmeticsDisplayComponent : public UActorComponent {

	GENERATED_BODY()

public:
	UCosmeticsDisplayComponent();

	void BeginPlay() override;	
	void EndPlay(EEndPlayReason::Type endPlayReason) override;

	void BindToSlots(TArray<UCosmeticsSlot*> slots);

	void SetOverridingPlayerMesh(USkeletalMeshComponent* playerMesh);

	TArray<UCosmeticsSlot*> GetBoundSlots();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TArray<UCosmeticsSlot*> BoundSlots;

private:
	void BindSlots(TArray<UCosmeticsSlot*> slots);
	void UnbindSlots();

	UFUNCTION()
	void OnStoreItemUpdated(class UCosmeticsSlot* slot) const;

	USkeletalMeshComponent* GetPlayerMesh() const;

	UPROPERTY()
	USkeletalMeshComponent* OverridingPlayerMesh;

	UPROPERTY()
	USlotActorManager* SlotActorManager;
};