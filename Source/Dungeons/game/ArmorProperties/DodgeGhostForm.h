#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/abilities/effects/GhostFormGameplayEffect.h"
#include "DodgeGhostForm.generated.h"

UCLASS()
class UGhostDodgeGameplayEffect : public UGhostFormBaseGameplayEffect {
	GENERATED_BODY()
public:
	UGhostDodgeGameplayEffect();
};

UCLASS()
class DUNGEONS_API UDodgeGhostForm : public UArmorProperty
{
	GENERATED_BODY()
public:
	UDodgeGhostForm();

	UPROPERTY(EditDefaultsOnly)
	float PostDodgeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGhostDodgeGameplayEffect> Effect;


	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;


	void OnDodgeRollEnd(FPredictionKey) override;

private:
	FTimerHandle removeHandle;

	void RemoveGameplayEffect();
};
