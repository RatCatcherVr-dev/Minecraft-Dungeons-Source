// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Particles/ParticleSystemComponent.h>
#include "BeamTargetUpdaterComponent.generated.h"

USTRUCT()
struct DUNGEONS_API FBeamUpdaterInfo {
	GENERATED_BODY()

	FBeamUpdaterInfo() {};
	FBeamUpdaterInfo(UParticleSystemComponent* system, TArray<int32>&& indecies);

	UPROPERTY()
	UParticleSystemComponent* System = nullptr;

	UPROPERTY()
	TArray<int32> EmitterIndecies;

	bool operator==(const FBeamUpdaterInfo& other) const {
		return System == other.System;
	}
};


UCLASS( Abstract )
class DUNGEONS_API UBeamTargetUpdaterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBeamTargetUpdaterComponent();

	UFUNCTION(BlueprintCallable)
	void AddParticleSystemComponent(UParticleSystemComponent* Component, TArray<int32> Indecies);
	
	UFUNCTION(BlueprintCallable)
	void RemoveParticleSystemComponent(UParticleSystemComponent* Component);


	void SetBeamLocation(const FVector Location);

protected:
	UPROPERTY(Transient)
	TArray<FBeamUpdaterInfo> ParticleSystems;

private:
	TOptional<FVector> CurrentTarget;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UActorBeamTargetUpdaterComponent : public UBeamTargetUpdaterComponent {
	GENERATED_BODY()
public:
	UActorBeamTargetUpdaterComponent();

	UFUNCTION(BlueprintCallable)
	void SetActorTarget(const AActor* targetActor);

	UFUNCTION(BlueprintCallable)
	void ForceUpdate();
	
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
private:

	TWeakObjectPtr<const AActor> TargetActor;
};