#pragma once

#include <GameplayModMagnitudeCalculation.h>
#include <GameplayEffectTypes.h>
#include "DifficultyModCalculation.generated.h"


/**
Used to scale mob damage based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyDependantModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UDifficultyDependantModCalculation();
	FOnExternalGameplayModifierDependencyChange* GetExternalModifierDependencyMulticast(const FGameplayEffectSpec& Spec, UWorld* World) const override;	
protected:
};

/**
Used to scale mob damage based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobDamageMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobDamageMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};

/**
Used to scale mob healing based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobHealingMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobHealingMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};


/**
Used to scale mob max health based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobMaxHealthMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobMaxHealthMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};

/**
Used to scale special mob max health based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultySpecialMobMaxHealthMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultySpecialMobMaxHealthMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};



/**
Used to scale mob max endurance based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobMaxEnduranceMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobMaxEnduranceMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};





/**
Used to scale mob endurance recovery based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobEnduranceRecoveryMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobEnduranceRecoveryMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};


/**
Used to scale mob speed based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyMobSpeedMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobSpeedMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};

/**
Used to scale player healing based on difficulty
*/
UCLASS()
class DUNGEONS_API UDifficultyPlayerHealingDivisionCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyPlayerHealingDivisionCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};

/**
Used to scale mob pushback resistance
*/
UCLASS()
class DUNGEONS_API UDifficultyMobPushbackMultiplicationCalculation : public UDifficultyDependantModCalculation {
	GENERATED_BODY()
public:
	UDifficultyMobPushbackMultiplicationCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
protected:
};