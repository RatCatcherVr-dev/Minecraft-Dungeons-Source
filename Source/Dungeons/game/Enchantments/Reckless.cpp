// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "Reckless.h"
#include "game/util/ValueFormat.h"
#include "EnchantmentUtil.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

const FName URecklessGameplayEffect::HealthReductionKey(TEXT("Health"));
const FName URecklessGameplayEffect::DamageIncreaseKey(TEXT("Damage"));

URecklessGameplayEffect::URecklessGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = DamageIncreaseKey;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Multiplicitive;
	Modifiers.Add(dealDamageInfo);

	FGameplayModifierInfo reduceHealthInfo;
	reduceHealthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();

	FSetByCallerFloat reduceHealthMagnitude;
	reduceHealthMagnitude.DataName = HealthReductionKey;

	reduceHealthInfo.ModifierMagnitude = reduceHealthMagnitude;
	reduceHealthInfo.ModifierOp = EGameplayModOp::Division;
	Modifiers.Add(reduceHealthInfo);
}

UReckless::UReckless() {
	LevelMultiplier = [this](int level) -> float {
		return 1.f + (DamageIncreaseAtLevelOne + DamageIncreasePerLevel * (level - 1));
	};

	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
	TypeId = EEnchantmentTypeID::Reckless;

	Effect = URecklessGameplayEffect::StaticClass();
}


void UReckless::OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) {
	Super::OnPreSpecApplication(mutableSpec);

	mutableSpec.SetSetByCallerMagnitude(URecklessGameplayEffect::DamageIncreaseKey, LevelMultiplier(Level));
	mutableSpec.SetSetByCallerMagnitude(URecklessGameplayEffect::HealthReductionKey, 1.f / GetHealthReductionAsMultipierClamped());
}

FText UReckless::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRelativeMultiplierPercentageChange(GetHealthReductionAsMultipierClamped())));
}

float UReckless::GetHealthReductionAsMultipierClamped() const {
	return FMath::Clamp(1.f - HealthDecrease, 0.01f, 0.99f);
}
