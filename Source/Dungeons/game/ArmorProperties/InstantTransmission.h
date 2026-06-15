#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "DodgeGhostForm.h"
#include "DodgeSpeedIncrease.h"
#include "InstantTransmission.generated.h"


UCLASS()
class DUNGEONS_API UInstantTransmissionGameplayEffect : public UGhostDodgeGameplayEffect {
	GENERATED_BODY()
public:
	UInstantTransmissionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API USuppressRollModeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USuppressRollModeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UInstantTransmission : public UDodgeSpeedIncrease
{
	GENERATED_BODY()

public:
	UInstantTransmission();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UInstantTransmissionGameplayEffect> RollEffect;

	UPROPERTY(EditDefaultsOnly)
	float PostDodgeDuration = 0.1f;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;
	void OnDodgeRollEnd(FPredictionKey) override;

	void OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key) override;
	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;
	void OnItemActivated(const AItemInstance*, FPredictionKey key) override;

private:
	FTimerHandle timerHandle;
	FActiveGameplayEffectHandle effectHandle;
	FActiveGameplayEffectHandle rollSuppressEffectHandle;

	void RemoveGameplayEffect(bool removeRollSuppress = false);
};

