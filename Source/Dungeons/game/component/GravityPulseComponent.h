// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityPulseComponent.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FGravityPulseAudio
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	USoundBase* sound;

	UPROPERTY(EditDefaultsOnly, meta = (ToolTip="Amount of time before gravity pulse is triggered to begin playback."))
	float leadInTime = 0.f;
};

class ABaseCharacter;
class UAbilitySystemComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UGravityPulseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityPulseComponent();

	void BeginPlay() override;

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<TWeakObjectPtr<ABaseCharacter>>& GetPullingCharacters() { return PullingCharacters; }

	void SetPulseActive(bool isActive);

protected:
	bool bHasStarted;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FVector PullToOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PulseInterval = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PullDistancePerPulse = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PullZFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bScaleZFactorByRadialDistance = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGravityPulseAudio GravityPulseAudio;

	float GetExecutionInterval() const { return PulseInterval; }

	bool ShouldIntervalExecute() const { return bHasStarted; }

private:
	TArray<TWeakObjectPtr<ABaseCharacter>> PullingCharacters;

	FTimerHandle IntervalTimerHandle, AudioTimerHandle;

	float UpperRadialDistSq2D;

	void Execution();

	void OnPulse();

	void TryStartPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character, UAbilitySystemComponent* ownerAbilitySystem);

	void EndPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character);

	void OnIntervalExecute();

	void PlayGravityPulseAudio();

	void ResetTimer();
};
