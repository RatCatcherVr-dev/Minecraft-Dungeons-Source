#pragma once

#include "CoreMinimal.h"

namespace game { namespace item { namespace power {
	static constexpr float ARTIFACT_POWER_IMPORTANCE = 1 / 3.0f;

	DUNGEONS_API extern float GetExponentialEfficiencyFromProgressFraction(float progressFraction);
	DUNGEONS_API extern float GetPowerLevelFromProgressFraction(float progressFraction);

	DUNGEONS_API extern float GetItemPowerProgressFraction(float power);
	DUNGEONS_API extern float GetItemPowerDeltaProgressFraction(float deltaPower);

	//These multipliers should reflect game difficulty
	DUNGEONS_API extern float DamageReductionMultiplier(float power);
	DUNGEONS_API extern float DamageIncreaseMultiplier(float power);	
	DUNGEONS_API extern float HealingIncreaseMultiplier(float power);
	DUNGEONS_API extern float DurationIncreaseMultiplier(float power);
	DUNGEONS_API extern float ForceIncreaseMultiplier(float power);
	DUNGEONS_API extern float StunDurationIncreaseMultiplier(float power);
	DUNGEONS_API extern float RangeIncreaseMultiplier(float power);
	DUNGEONS_API extern float CooldownReductionMultiplier(float power);
	DUNGEONS_API extern float MaxHealthMultiplier(float power);
	DUNGEONS_API extern float SalvageValueMultiplier(float power);
	DUNGEONS_API extern float ShopPriceMultiplier(float power);

	//These are added bonuses
	DUNGEONS_API extern float HiddenSpeedIncreaseMultiplier(float power);
	DUNGEONS_API extern float HiddenRangeIncreaseMultiplier(float power);
	DUNGEONS_API extern float HiddenDamageReductionMultiplier(float power);	
	DUNGEONS_API extern float HiddenDamageIncreaseMultiplier(float power);
	DUNGEONS_API extern float HiddenQuiverAmmoIncrease(float power);
}}}