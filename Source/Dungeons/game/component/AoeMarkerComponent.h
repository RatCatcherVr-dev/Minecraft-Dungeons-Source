#pragma once

#include "game/component/AttackComponent.h"
#include "AoeAttackComponent.h"
#include "game/actor/DungeonsTargetMarker.h"
#include "AoeMarkerComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAoeMarkerComponent : public UAoeAttackComponent
{
	GENERATED_BODY()

public:
	UAoeMarkerComponent(const FObjectInitializer& ObjectInitializer);

	virtual FVector GetAttackLocation() const override;
	void SetLockedMarker(ADungeonsTargetMarker* TargetMarker);

	void EndAttack(AActor* attackTarget = nullptr) override;

	UFUNCTION()
	void OnOwnerDeath();

	void StopAttack() override;

	UFUNCTION(BlueprintPure)
	bool IsAttackInProgress() const override;

	virtual bool OnAttackBegan(AActor* attackTarget) { return false; };

protected:
	virtual void BeginPlay() override;

	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;

	TArray<ADungeonsTargetMarker*> MarkerActors;
	ADungeonsTargetMarker* EarliestLockedMarker;
};
