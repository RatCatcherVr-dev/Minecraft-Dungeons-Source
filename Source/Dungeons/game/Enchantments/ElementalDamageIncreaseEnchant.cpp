// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "ElementalDamageIncreaseEnchant.h"
#include "game/util/ValueFormat.h"
#include "game/abilities/attributes/DamageAttributeSet.h"

const FName UElementalDamageIncreaseGameplayEffect::DamageKey(TEXT("Damage"));

UElementalDamageIncreaseGameplayEffect::UElementalDamageIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UDamageAttributeSet::DamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = DamageKey;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}

ULightningTouchedGameplayEffect::ULightningTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer) {
	Modifiers.Last().SourceTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));
}

UPoisonTouchedGameplayEffect::UPoisonTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer)
 : Super(ObjectInitializer) {
	Modifiers.Last().SourceTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));
}

UFireTouchedGameplayEffect::UFireTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer) 
 : Super(ObjectInitializer) {
	Modifiers.Last().SourceTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));
}

USoulTouchedGameplayEffect::USoulTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {
	Modifiers.Last().SourceTags.RequireTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Soul")));
}

UElementalDamageIncreaseEnchant::UElementalDamageIncreaseEnchant() {
	LevelMultiplier = [this](int level) -> float {
		return 1.f + DamageIncreasePerLevel * level;
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;

	Effect = UElementalDamageIncreaseGameplayEffect::StaticClass();
}


void UElementalDamageIncreaseEnchant::OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) {
	Super::OnPreSpecApplication(mutableSpec);

	mutableSpec.SetSetByCallerMagnitude(UElementalDamageIncreaseGameplayEffect::DamageKey, LevelMultiplier(Level));
}