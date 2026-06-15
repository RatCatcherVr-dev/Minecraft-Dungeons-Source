#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/actor/item/FishHookItem.h"
#include "world/entity/EntityTypes.h"
#include "FishingRodItemInstance.generated.h"

UENUM()
enum class EHierarchyPlacementComparison {
	Above, 
	Below, 
	Same, 
	None
};

USTRUCT(BlueprintType) 
struct DUNGEONS_API FHierarchyPlacement {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	UClass* classType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	int placement;
};

UCLASS()
class DUNGEONS_API AFishingRodItemInstance : public AItemInstance
{
	GENERATED_BODY()
public:	
	AFishingRodItemInstance();

	bool IsBusy() const override;

	int GetDisplayCount() const override;

	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const;
private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnHook(const FVector& location, const FRotator& rotation, AActor* closestActor, float reach, const FPredictionKey& predictionKey);

	UFUNCTION()
	void SpawnHook(const FVector& location, const FRotator& rotation, AActor* closestActor, float reach, const FPredictionKey& predictionKey);

	UFUNCTION()
	void OnHookThrow(FPredictionKey predictionKey);

	int GetHierarchyPlacement(UClass* classToCheck);

	EHierarchyPlacementComparison GetHierarchyPlacementComparison(int classToCheckPlacement, int compareClassPlacement);

	UFUNCTION()
	void OnHookHitTarget(const FPredictionKey& predictionKey, EntityType& mobEntityType);

	UFUNCTION()
	void OnHookHitPlayer(const FPredictionKey& predictionKey);

	UFUNCTION()
	void OnHookonDenied(const FPredictionKey& predictionKey);
	
	void PlayPullbackAnimation(const FPredictionKey& predictionKey);

	void SpawnRodActor();

	void RemoveRodActor();

	void UpdateFishingRodVisibility();

	void ShowFishingRod();
	void HideFishingrod();

	bool IsCharacterTargetable(const ABaseCharacter* character, const AActor* actor) const;

	bool TargetRaycastHit(const FVector& playerLocation, const FVector& targetLocation);

	void OnOwnerAndOwnerControllerReplicated() override;

	UFUNCTION()
	void OnRep_FishingRodVisible();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UFUNCTION()
	void OnHookedMob(EntityType& mobEntityType);
	UFUNCTION(Client, Reliable)
	void Client_OnHookedMob(EntityType mobEntityType);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AFishHookItem> FishingHookActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AActor> FishingRodActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TWeakObjectPtr<UChildActorComponent> FishingRodChildActor;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStaggerGameplayEffect> Effect = UStaggerGameplayEffect::StaticClass();

	class AFishHookItem *Hook;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float HookReach = 750.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float StunDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FTimerHandle throwHookTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UAnimSequenceBase* FishHookThrowAnimationSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UAnimSequenceBase* FishHookPullAnimationSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	FName Slot = FName(TEXT("UpperBody"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TArray<FHierarchyPlacement> TypeHierarchy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TArray<EntityType> NonHookableMobs;

	UPROPERTY(ReplicatedUsing = OnRep_FishingRodVisible)
	bool FishingRodVisible = false;
};
