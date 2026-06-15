#include "Dungeons.h"
#include "ItemPowerStats.h"
#include "game/difficulty/Difficulty.h"
#include "util/FloatRange.h"
#include "ItemPowerUtil.h"

namespace game { namespace item { namespace power {

	namespace {
		
		static const float GLOBAL_THREAT_LEVELS_AT_RELEASE = 18.0f; //This value should never ever need to be changed again.
		FloatRange GetItemPowerRange() {
			const float DISPLAY_ITEM_POWER_PER_THREAT_LEVEL = 99.0f / (GLOBAL_THREAT_LEVELS_AT_RELEASE-1); //5.823529... - This value should never ever need to be changed again.
			return FloatRange(
				UItemPowerUtil::GetItemPowerFromDisplayValue(1), // First threat level
				UItemPowerUtil::GetItemPowerFromDisplayValue(1 + DISPLAY_ITEM_POWER_PER_THREAT_LEVEL * static_cast<float>(FDifficulty::numGlobalThreats()-1)) // Last threat level
			);
		}

		float GetExponentialPowerEfficiencyFromPowerLevel(float power) {
			const float progressFraction = GetItemPowerProgressFraction(power);

			return GetExponentialEfficiencyFromProgressFraction(progressFraction);
		}

		float GetLinearPowerEfficiencyFromPowerLevel(float power) {
			const float ITEM_EFFICIENCY_PER_THREAT_LEVEL = 2.6f / (GLOBAL_THREAT_LEVELS_AT_RELEASE-1); //0,1529411... - This value should never ever need to be changed again.
			const float minEfficiency = 1.0f;
			const float maxEfficiency = 1.0f + ITEM_EFFICIENCY_PER_THREAT_LEVEL * static_cast<float>(FDifficulty::numGlobalThreats()-1);

			const float progressFraction = GetItemPowerProgressFraction(power);

			return FMath::Lerp(minEfficiency, maxEfficiency, progressFraction);
		}

		float GetPowerAboveOne(float power) {
			return FMath::Max(0.0f, power - 1.0f);
		}
		float GetScaledPowerAboveOne(float power, float scaling) {
			return GetPowerAboveOne(power) * scaling;
		}

		//Formula used for multipliers where an increasing item power should cause the multiplier to become smaller.
		//Item power 1 should still result in a multiplier of 1.
		//Examples
		//
		// 1.0, 1.0, 0.5 -> 1.0
		// 2.0, 1.0, 0.5 -> 0.75
		// 3.0, 1.0, 0.5 -> 0.6667
		//
		// 1.0, 2.0, 0.5 -> 1.0
		// 2.0, 2.0, 0.5 -> 0.6667
		// 3.0, 2.0, 0.5 -> 0.625
		float GetScaledCappedShrinkingMultiplier(float power, float scaling, float infinitePowerResultingMultiplier) {
			ensure(infinitePowerResultingMultiplier <= 1.0f);
			const float ScaledPowerAboveOne = GetScaledPowerAboveOne(power, scaling);			
			const float VariableRemainder = 1.0 - infinitePowerResultingMultiplier;
			return infinitePowerResultingMultiplier + VariableRemainder / (1.0f + ScaledPowerAboveOne);
		}

		//Formula used for multipliers where an increasing item power should result in a linearly increasing multiplier.
		//Item power 1 should still result in a multiplier of 1.
		//Examples
		//
		// 1.0, 0.5 -> 1.0
		// 2.0, 0.5 -> 1.5
		// 3.0, 0.5 -> 2.0
		//
		// 1.0, 2 -> 1.0
		// 2.0, 2 -> 3.0
		// 3.0, 2 -> 5.0
		//
		float GetScaledGrowingMultiplier(float power, float scaling) {
			const float ScaledPowerAboveOne = GetScaledPowerAboveOne(power, scaling);
			return 1.0f + ScaledPowerAboveOne;
		}

		//Formula used for multipliers where an infinite item power should result in target multiplier.
		//Item power 1 should still result in a multiplier of 1.
		//Examples
		//
		// 1.0, 0.5, 2 -> 1.0
		// 2.0, 0.5, 2 -> 1.3333
		// 3.0, 0.5, 2 -> 1.5
		//
		// 1.0, 2, 2 -> 1.0
		// 2.0, 2, 2 -> 1.75
		// 3.0, 2, 2 -> 1.875
		//
		float GetScaledCappedGrowingMultiplier(float power, float scaling, float infinitePowerResultingMultiplier) {
			const float ScaledPowerAboveOne = GetScaledPowerAboveOne(power, scaling);
			const float TargetAboveOne = infinitePowerResultingMultiplier - 1.0f;
			return 1.0f + ( TargetAboveOne - TargetAboveOne / ( 1.0f + ScaledPowerAboveOne ));
		}
	}

	DUNGEONS_API extern float GetExponentialEfficiencyFromProgressFraction(float progressFraction) {
		const float maximumProgress = 3; // We need to support higher than 1 because items can become stronger, but we need a max so items damage/health don't go negative during testing

		const float base = 0.75f; // The start value of the curve
		const float increasePerStep = 1.25f; // How fast the curve increases
		const float progressScale = static_cast<float>(FDifficulty::numGlobalThreats()); // How far away on the curve "1" is
		const float curveValue = base * FMath::Pow(increasePerStep, FMath::Clamp(progressFraction, 0.0f, maximumProgress) * progressScale);
		return curveValue + 0.25f; // Start at 1 for efficiency
	}

	DUNGEONS_API float GetPowerLevelFromProgressFraction(float progressFraction) {
		return GetItemPowerRange().lerp(progressFraction);
	}

	DUNGEONS_API float GetItemPowerProgressFraction(float power) {
		return GetItemPowerRange().fractionAt(power);
	}

	DUNGEONS_API extern float GetItemPowerDeltaProgressFraction(float deltaPower) {
		return deltaPower / GetItemPowerRange().range();
	}

	//--- These multipliers have corresponding increases in mob difficulty

	DUNGEONS_API float DamageReductionMultiplier(float power) {
		return GetScaledCappedShrinkingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 1.0f, 0.25f);
	}

	DUNGEONS_API float DamageIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetExponentialPowerEfficiencyFromPowerLevel(power), 2.0f);
	}	

	DUNGEONS_API float HealingIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetExponentialPowerEfficiencyFromPowerLevel(power), 1.0f);
	}

	DUNGEONS_API float StunDurationIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.5f);
	}

	DUNGEONS_API float MaxHealthMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetExponentialPowerEfficiencyFromPowerLevel(power), 1.f);
	}


	//--- These multipliers don't directly correspond to something that's happening with mobs over progression

	DUNGEONS_API float DurationIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.25f);
	}	

	DUNGEONS_API float ForceIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.33f);
	}

	DUNGEONS_API float RangeIncreaseMultiplier(float power) {
		return GetScaledGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.5f);
	}

	DUNGEONS_API float CooldownReductionMultiplier(float power) {
		return GetScaledCappedShrinkingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.33f, 0.5f);
	}

	DUNGEONS_API float SalvageValueMultiplier(float power) {
		return FMath::Lerp(1.0f, 2.0f, GetItemPowerProgressFraction(power));
	}

	DUNGEONS_API float ShopPriceMultiplier(float power) {
		return SalvageValueMultiplier(power);
	}

	//--- These three are hidden, and we don't care too much about their balance

	DUNGEONS_API float HiddenSpeedIncreaseMultiplier(float power) {
		return GetScaledCappedGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.5f, 1.25f);
	}

	DUNGEONS_API float HiddenRangeIncreaseMultiplier(float power) {
		return GetScaledCappedGrowingMultiplier(GetLinearPowerEfficiencyFromPowerLevel(power), 0.33f, 1.75f);
	}

	DUNGEONS_API extern float HiddenQuiverAmmoIncrease(float power)
	{
		return power * 0.4f;
	}

}}}