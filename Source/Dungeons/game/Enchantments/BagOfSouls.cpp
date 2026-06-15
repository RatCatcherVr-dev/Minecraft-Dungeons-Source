#include "BagOfSouls.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "Enchantment.h"
#include "game/abilities/effects/GameplayEffectUtil.h"


UBagOfSouls::UBagOfSouls() {
	LevelMultiplier = [this](int level) -> float {
		float percentage = level * MaxSoulsPercentageGainedPerLevel;
		return 1.f + percentage;
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
	TypeId = EEnchantmentTypeID::BagOfSouls;
	Effect = UBagOfSoulsGameplayEffect::StaticClass();
}

void UBagOfSouls::OnPreSpecApplication(FGameplayEffectSpec & mutableSpec) {
	mutableSpec.SetSetByCallerMagnitude(UBagOfSoulsGameplayEffect::MaxSoulsKey, LevelMultiplier(Level));
}

UBagOfSoulsGameplayEffect::UBagOfSoulsGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo soulInfo;
	soulInfo.Attribute = UItemAttributeSet::MaxSoulsAttribute();

	FSetByCallerFloat maxSouls;
	maxSouls.DataName = UBagOfSoulsGameplayEffect::MaxSoulsKey;

	soulInfo.ModifierMagnitude = maxSouls;
	soulInfo.ModifierOp = EGameplayModOp::Multiplicitive;

	Modifiers.Add(soulInfo);
}

const FName UBagOfSoulsGameplayEffect::MaxSoulsKey("MaxSouls");