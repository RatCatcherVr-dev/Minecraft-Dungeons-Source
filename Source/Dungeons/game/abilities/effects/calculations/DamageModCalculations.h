#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "DamageModCalculations.generated.h"

/**
Gets the effects::HealthName base value - Defaults to -1
Is used in conjuction with effects::CreateGameplayEffectSpec and
settings the effects::HealthName attribute to a base value.
This is a pure health modifier - it will be applied without any modifiers.
*/
UCLASS()
class DUNGEONS_API UNegativeHealthModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UNegativeHealthModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

/**
Modifies the value using
UHealthAttributeSet::TakeDamageMultiplierAttribute
This is considered 'damage' and it will thus be reduced if the target has damage reducing effects.
It is not considered to come from anywhere and will thus not be scaled by the source ability system.
*/
UCLASS()
class DUNGEONS_API UDamageModCalculation : public UNegativeHealthModCalculation {
	GENERATED_BODY()
public:
	UDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition TakeDamageAttributeCapture;

	virtual float PremultiplyAdd(const FGameplayEffectSpec&, const FAggregatorEvaluateParameters&) const { return 0; };
};


/**
Modifies the value using the sources
UDamageAttributeSet::DamageMultiplierAttribute
*/
UCLASS()
class DUNGEONS_API UDamageModSourceCalculation : public UDamageModCalculation {
	GENERATED_BODY()
public:
	UDamageModSourceCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition GiveDamageMultiplierAttribute;
};



UCLASS()
class DUNGEONS_API UAffectorDamageModCalculation : public UDamageModSourceCalculation {
	GENERATED_BODY()
public:
	UAffectorDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition DealDamageAttributeCapture;
};


/**
Modifies the value using
UDifficultyAttributeSet::DifficultyDealDamageMultiplier()
Is used to make mobs deal more damage at higher difficulties.
*/
UCLASS()
class DUNGEONS_API UDifficultyDamageModCalculation : public UAffectorDamageModCalculation {
	GENERATED_BODY()
public:
	UDifficultyDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition DealDamageAttributeCapture;
};

UCLASS()
class DUNGEONS_API UDifficultyTargetDamageModCalculation : public UAffectorDamageModCalculation {
	GENERATED_BODY()
public:
	UDifficultyTargetDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition DealDamageAttributeCapture;
};


/**
Modifies the value using
UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute(),
UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute()
These are multiplied together.
Used for damage types which originate from melee weapons and should be considered melee in nature.
*/
UCLASS()
class DUNGEONS_API UMeleeModDamageCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	UMeleeModDamageCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition MeleeDamageMultiplierCapture;
	const FGameplayEffectAttributeCaptureDefinition MeleeItemPowerFactorCapture;
	const FGameplayEffectAttributeCaptureDefinition MeleeDamageIncreaseCapture;
	const FGameplayEffectAttributeCaptureDefinition TakeMeleeDamageAttributeCapture;

	float PremultiplyAdd(const FGameplayEffectSpec&, const FAggregatorEvaluateParameters&) const override;
};


/**
Modifies the value using
UItemAttributeSet::ItemDamageMultiplierAttribute()
Used for damage types which originate from items.
Item power effect is assumed to be included in the DamageName attribute amount.
*/
UCLASS()
class DUNGEONS_API UItemDamageModCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	UItemDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition ItemDamageModCapture;
};

/**
Modifies the value using
URangedAttributeSet::RangedAttackDamageMultiplerAttribute(),
URangedAttributeSet::RangedAttackItemPowerFactorAttribute()
These are multiplied together.
Used for damage types which originate from ranged weapons and should be considered ranged in nature.
*/
UCLASS()
class DUNGEONS_API URangedDamageModCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	URangedDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition RangedDamageModCapture;
	const FGameplayEffectAttributeCaptureDefinition RangedItemPowerFactorCapture;
	const FGameplayEffectAttributeCaptureDefinition RangedDamageIncreaseCapture;
	const FGameplayEffectAttributeCaptureDefinition TakeRangeDamageAttributeCapture;

	float PremultiplyAdd(const FGameplayEffectSpec&, const FAggregatorEvaluateParameters&) const override;
};


/**
Modifies the value using
UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute()
Used for damage types which originate from melee weapons but are not considered "melee".
*/
UCLASS()
class DUNGEONS_API UMeleeItemPowerOnlyModDamageCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	UMeleeItemPowerOnlyModDamageCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition MeleeItemPowerFactorCapture;
};


/**
Modifies the value using
URangedAttributeSet::RangedAttackItemPowerFactorAttribute()
Used for damage types which originate from ranged weapons but are not considered "ranged".
*/
UCLASS()
class DUNGEONS_API URangedItemPowerOnlyModDamageCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	URangedItemPowerOnlyModDamageCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition RangedItemPowerFactorCapture;
};


UCLASS()
class DUNGEONS_API UItemPowerOnlyAsEffectLevelModDamageCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	UItemPowerOnlyAsEffectLevelModDamageCalculation() {};
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};


/**
Modifies the value using
UHealthAttributeSet::ArmorAttackItemPowerFactorAttribute()
Used for damage types which originate from armors.
*/
UCLASS()
class DUNGEONS_API UArmorItemPowerOnlyModDamageCalculation : public UDifficultyDamageModCalculation {
	GENERATED_BODY()
public:
	UArmorItemPowerOnlyModDamageCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	const FGameplayEffectAttributeCaptureDefinition ArmorItemPowerFactorCapture;
};


/**
Fallback logic for calculating damage which does not have an ability system
All DamageName attribute values are assumed to be unscaled and will be scaled according to difficulty.
*/
UCLASS()
class DUNGEONS_API UWorldDamageModCalculation : public UDamageModCalculation {
	GENERATED_BODY()
public:
	UWorldDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};



/**
Modifies the value using source 
UHealthAttributeSet::TakeDamageMultiplierAttribute
This is damage that has been reflected from taking damage
we need to invert the source TakeDamageAttribute to return the damage to the original
*/
UCLASS()
class DUNGEONS_API UReflectedDamageModCalculation : public UDamageModCalculation {
	GENERATED_BODY()
public:
	UReflectedDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition TakeDamageAttributeCapture;
};

UCLASS()
class DUNGEONS_API UReflectedRangeItemPowerDamageModCalculation : public UDamageModCalculation {
	GENERATED_BODY()
public:
	UReflectedRangeItemPowerDamageModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
	//This only works because this class is never instantiated!
	const FGameplayEffectAttributeCaptureDefinition TakeDamageAttributeCapture;
	const FGameplayEffectAttributeCaptureDefinition RangedItemPowerFactorCapture;
};
