// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "EffectApplyingEnchantment.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DUNGEONS_API UEffectApplyingEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	void OnStart() override;

	void OnEnd() override;

	virtual void OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) {};

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};
