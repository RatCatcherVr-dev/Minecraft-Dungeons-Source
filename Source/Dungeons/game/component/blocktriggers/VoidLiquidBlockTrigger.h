#pragma once

#include "LavaBlockTrigger.h"
#include "WaterBlockTrigger.h"
#include "GameplayEffectExecutionCalculation.h"
#include "game/abilities/ui/DungeonsGameplayEffectUIData.h"
#include "VoidLiquidBlockTrigger.generated.h"

UCLASS(BlueprintType)
class UVoidLiquidBlockTrigger : public ULavaBlockTrigger
{
	GENERATED_BODY()

public:
	UVoidLiquidBlockTrigger();
	
	UFUNCTION(BlueprintCallable)
	void OnOverlapStart(ABaseCharacter* const character);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnd(ABaseCharacter* const character);

protected:
	void OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;
	void OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;

	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;

	float baseVoidTouchedDuration = 2.0f;
	float baseDamageMultiplier = 2.0f;

private:
	const UVoidLiquidSlowdownGameplayEffect* slowdownGameplayEffect;
	const UVoidLiquidDamageGameplayEffect* damageGameplayEffect;
	const UVoidLiquidDurationExtenderGameplayEffect* durationExtenderGameplayEffect;
	const UVoidLiquidMagnitudeExtenderGameplayEffect* magntiudeExtenderGameplayEffect;
};

UCLASS()
class DUNGEONS_API UVoidLiquidGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidLiquidGameplayEffect();
};


// Duration Stacker - Start
UCLASS()
class DUNGEONS_API UVoidLiquidDurationExtenderGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidLiquidDurationExtenderGameplayEffect();
	float durationIncreasePeriod = 0.5f;
};

UCLASS()
class DUNGEONS_API UVoidLiquidDurationExtenderExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UVoidLiquidDurationExtenderExecution();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
	float exponetialIncrease = 1.05f;
	float periodIncrease = 0.2f;
	float maxDuration = 30.0f;

	TSubclassOf<UGameplayEffect> effectToApply;
};
// Duration Stacker - End

// Magnitude Stacker - Start
UCLASS()
class DUNGEONS_API UVoidLiquidMagnitudeExtenderGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidLiquidMagnitudeExtenderGameplayEffect();
	float magnitudeIncreasePeriod = 1.0f;
};

UCLASS()
class DUNGEONS_API UVoidLiquidMagnitudeExtenderExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UVoidLiquidMagnitudeExtenderExecution();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	float exponetialIncrease = 1.06f;
	float periodIncrease = 0.5f;
	float maxMagnitude = 15.0f;

	TSubclassOf<UGameplayEffect> effectToApply;
};
// Magnitude Stacker - Start

UCLASS()
class DUNGEONS_API UVoidLiquidSlowdownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidLiquidSlowdownGameplayEffect();

	float slownessAmount = 0.4f;
};

UCLASS()
class DUNGEONS_API UVoidLiquidDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidLiquidDamageGameplayEffect();

	float damagePerPeriod = 2.5f;
	float damagePeriod = 1.0f;
};

UCLASS()
class DUNGEONS_API UVoidTouchedGameplayEffectUIData : public UDungeonsGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UVoidTouchedGameplayEffectUIData(const FObjectInitializer& ObjectInitializer);
};