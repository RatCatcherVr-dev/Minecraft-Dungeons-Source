#pragma once

#include "GameplayEffect.h"
#include "game/actor/PropActor.h"
#include "FishHookItem.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHookHitPlayer, const FPredictionKey&, predictionKey);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnHookHitTarget, const FPredictionKey&, predictionKey, EntityType&, mobEntityType);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHookonDenied, const FPredictionKey&, predictionKey);

UCLASS()
class DUNGEONS_API AFishHookItem : public APropActor
{
	GENERATED_BODY()
	
public:
	AFishHookItem(const class FObjectInitializer& OI);

	void BeginPlay() override;

	void SetTargetActor(AActor *target);

	void SetMaxReach(float reach);

	void SetStunDuration(float stunDuration);

	void SetPredictionKey(const FPredictionKey& predictionKey);

	UFUNCTION()
	void OnHitObject(AActor* OverlappedActor, AActor* OtherActor);
protected:
	void OnRep_Owner() override;

	void Tick(float DeltaSeconds) override;
private:
	float StunDuration = 1.0f;

	float HookSpeed = 0.0f;

	float HookAcc = 2000.0f;

	float MaxReach = 0.0f;

	bool ShouldGoback = false;

	bool MissedActor = false;

	TOptional<float> StopPullbackDistance;

	FVector LastValidPlayerToHookDirection;

	TOptional<FVector> CachedPlayerForwardDirectionOnSpawn;

	FVector CachedPullbackDirection;

	AActor *TargetActor = nullptr;

	bool bFirstTick = true;

	USkeletalMeshComponent* PlayerSkeleton;

	void CachePullbackDirection();

	void InitializeOwner();
public:
	UPROPERTY()
	FOnHookHitPlayer OnHookHitPlayer;

	UPROPERTY()
	FOnHookHitTarget OnHookHitTarget;

	UPROPERTY()
	FOnHookonDenied OnHookonDenied;

	TSubclassOf<UGameplayEffect> Effect;

	FPredictionKey PredictionKey;
};
