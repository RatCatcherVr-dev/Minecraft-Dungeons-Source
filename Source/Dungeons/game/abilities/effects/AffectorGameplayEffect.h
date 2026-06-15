#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "calculations/DamageModCalculations.h"
#include "game/affector/AffectorTypes.h"
#include "AffectorGameplayEffect.generated.h"



UCLASS()
class DUNGEONS_API UAffectorDamageEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAffectorDamageEffect();
};

UCLASS()
class DUNGEONS_API UAffectorHealthEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAffectorHealthEffect();
};

UCLASS()
class DUNGEONS_API UAffectorSpeedEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAffectorSpeedEffect();
};

UCLASS()
class DUNGEONS_API UAffectorUnderwaterEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAffectorUnderwaterEffect();
};

UCLASS()
class DUNGEONS_API UAffectorMaxHealthModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UAffectorMaxHealthModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	const FGameplayEffectAttributeCaptureDefinition MaxHealthAttribute;
};


namespace affector { namespace effect {
	using EffectMagnitudeModifier = std::function<float(float)>;

struct EffectType {
	EffectType(const FName& dataName, const TSubclassOf<UGameplayEffect>&, const EffectMagnitudeModifier& magnitudeModifier = [](float magnitude) { return magnitude; });

	FName DataName;
	TSubclassOf<UGameplayEffect> EffectClass;
	EffectMagnitudeModifier MagnitudeModifier;
};


const EffectType& damageEffectType();
const EffectType& maxHealthEffectType();
const EffectType& speedEffectType();
const EffectType& artifactCooldownType();
const EffectType& underwaterEffectType();

void applyEffects(UAbilitySystemComponent*, bool bApplyOnlyDefaults = false);
void applyEffect(UAbilitySystemComponent*, const AffectorType&);

}
}
