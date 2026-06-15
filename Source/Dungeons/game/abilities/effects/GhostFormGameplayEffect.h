#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GhostFormGameplayEffect.generated.h"

UCLASS()
class UGhostFormBaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGhostFormBaseGameplayEffect();

	static const FName GhostFormTakeDamageKey;
	static const float DamageTakenMultiplier;
};