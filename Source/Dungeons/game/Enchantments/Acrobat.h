// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "EffectApplyingEnchantment.h"
#include "Acrobat.generated.h"

UCLASS()
class DUNGEONS_API UAcrobatGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAcrobatGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName CooldownReductionKey;
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAcrobat : public UEffectApplyingEnchantment
{
	GENERATED_BODY()
public:
	UAcrobat();

	void OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ReductionPerlevel = 0.15f;
};
