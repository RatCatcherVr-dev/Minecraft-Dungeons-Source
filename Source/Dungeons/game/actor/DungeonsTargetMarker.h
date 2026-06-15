// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "DungeonsTargetMarker.generated.h"

class UAoeMarkerComponent;
class UAoeTargetProvider;

UCLASS()
class DUNGEONS_API ADungeonsTargetMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonsTargetMarker();

	UPROPERTY(EditDefaultsOnly, Category = "TargetMarker")
	USphereComponent* SphereComp;
	// VFX while following target
	UPROPERTY(EditDefaultsOnly, Category = "TargetMarker")
	UParticleSystemComponent* FollowTargetVFX;
	// VFX when we will attack the target
	UPROPERTY(EditDefaultsOnly, Category = "TargetMarker")
	UParticleSystemComponent* AttackVFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "TargetMarker")
		float MarkerSpeed = 3.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginAttack(UAoeMarkerComponent* ownerAttackComponent, AActor* newTarget, UAoeTargetProvider* targetProvider, float lockDelay);
	void OnAttackBegan(float lockDelay);
	void OnAttackEnded();
	void SetOwnerComponent(class UAoeMarkerComponent* componentOwner);
	void SetTargetAndProvider(AActor* newTarget, UAoeTargetProvider* targetProvider);
	void OnCharacterDeath();

	UFUNCTION(BlueprintImplementableEvent, Category = "TargetMarker")
	void OnAttackBeganBP(float LockDelay);
	UFUNCTION(BlueprintImplementableEvent, Category = "TargetMarker")
	void OnAttackEndedBP();
	UFUNCTION(BlueprintImplementableEvent, Category = "TargetMarker")
	void OnTargetLockedBP();

	UFUNCTION(NetMulticast, reliable)
	void OnTargetLockedMulticast(FVector endTarget);
	UFUNCTION(NetMulticast, reliable)
	void ResetLockedMulticast();
	UFUNCTION(NetMulticast, reliable)
	void SetupAttackBeganParticles();
	UFUNCTION(NetMulticast, reliable)
	void SetupTargetLockedParticles();
	UFUNCTION(NetMulticast, reliable)
	void OnAttackBeganMulticast(float LockDelay);
	UFUNCTION(NetMulticast, reliable)
	void OnAttackEndedMulticast();

private:
	UFUNCTION()
		void OnTargetLocked();

	float targetLockedDelay;
	UPROPERTY(Replicated)
	TWeakObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	UAoeTargetProvider* MarkerTargetProvider;
	UPROPERTY()
	UAoeMarkerComponent* MarkerOwner;

	FTimerHandle targetLockedTimerHandle;

	UPROPERTY(Replicated)
	bool bIsTargeting;

	TOptional<FVector> finalTarget;
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
