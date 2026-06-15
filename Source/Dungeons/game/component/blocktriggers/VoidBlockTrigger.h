#pragma once

#include "VoidLiquidBlockTrigger.h"
#include "game/abilities/ui/DungeonsGameplayEffectUIData.h"
#include "VoidBlockTrigger.generated.h"

UCLASS()
class UVoidBlockTrigger : public UBlockTrigger
{
	GENERATED_BODY()

public:
	UVoidBlockTrigger();

protected:
	void OnEnterOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;
	void OnExitOverlap(EMaterialTypeEnum& overlapMaterial, ABaseCharacter* const character) const override;
	bool IsTrigger(const FullBlock& block, ABaseCharacter* const character) const override;

	void ApplySlowdown(ABaseCharacter* const character, bool isEnter) const;

private:
	const UVoidBlockSlowdownGameplayEffect* slowdownGameplayEffect;
	const UVoidBlockDamageGameplayEffect* damageGameplayEffect;
	const UVoidBlockDurationExtenderGameplayEffect* durationExtenderGameplayEffect;
	const UVoidBlockMagnitudeExtenderGameplayEffect* magntiudeExtenderGameplayEffect;

	const float baseVoidBlockDuration = 2.0f;
	const float baseDamageMultiplier = 2.0f;
	const float baseSlowdown = 0.1f;
	const int initialSlowdownPercentage = 75;
	const float initialSlowdownFalloffSpeed = 0.03f;
	const int slowDownPercentage = 50;
	const float exitSlowDownFalloffSpeed = 0.04f;
};

UCLASS()
class DUNGEONS_API UVoidBlockGameplayEffect : public UVoidLiquidGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidBlockGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidBlockDurationExtenderGameplayEffect : public UVoidLiquidDurationExtenderGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidBlockDurationExtenderGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidBlockDurationExtenderExecution : public UVoidLiquidDurationExtenderExecution
{
	GENERATED_BODY()
public:
	UVoidBlockDurationExtenderExecution();
};

UCLASS()
class DUNGEONS_API UVoidBlockMagnitudeExtenderGameplayEffect : public UVoidLiquidMagnitudeExtenderGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidBlockMagnitudeExtenderGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidBlockMagnitudeExtenderExecution : public UVoidLiquidMagnitudeExtenderExecution
{
	GENERATED_BODY()
public:
	UVoidBlockMagnitudeExtenderExecution();
};

UCLASS()
class DUNGEONS_API UVoidBlockSlowdownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidBlockSlowdownGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidBlockDamageGameplayEffect : public UVoidLiquidDamageGameplayEffect
{
	GENERATED_BODY()
public:
	UVoidBlockDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVoidedGameplayEffectUIData : public UDungeonsGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UVoidedGameplayEffectUIData(const FObjectInitializer& ObjectInitializer);
};