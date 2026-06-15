// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "Acrobat.h"
#include "game/abilities/attributes/MovementAttributeSet.h"


const FName UAcrobatGameplayEffect::CooldownReductionKey("DodgeCooldownReduction");

UAcrobatGameplayEffect::UAcrobatGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dodgeCooldownInfo;
	dodgeCooldownInfo.Attribute = UMovementAttributeSet::DodgeCooldownAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = UAcrobatGameplayEffect::CooldownReductionKey;

	dodgeCooldownInfo.ModifierMagnitude = dealDamageMagnitude;
	dodgeCooldownInfo.ModifierOp = EGameplayModOp::Division;

	Modifiers.Add(dodgeCooldownInfo);
}

UAcrobat::UAcrobat() {
	LevelMultiplier = [this](int level) -> float {
		return FMath::Clamp(1.f - level * ReductionPerlevel, std::numeric_limits<float>::min(), 1.f);
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
	TypeId = EEnchantmentTypeID::Acrobat;
	Effect = UAcrobatGameplayEffect::StaticClass();
}


void UAcrobat::OnPreSpecApplication(FGameplayEffectSpec & mutableSpec) {
	mutableSpec.SetSetByCallerMagnitude(UAcrobatGameplayEffect::CooldownReductionKey, 1.f/LevelMultiplier(Level));
}

