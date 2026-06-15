#pragma once

#include <GameplayEffectTypes.h>
#include <GameplayEffectCustomApplicationRequirement.h>
#include "BaseGameplayEffectApplicationRequirement.generated.h"


class UAbilitySystemComponent;

UCLASS()
class DUNGEONS_API UBaseGameplayEffectApplicationRequirement : public UGameplayEffectCustomApplicationRequirement
{

public:
	GENERATED_BODY()

	bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect, const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const override;
};
