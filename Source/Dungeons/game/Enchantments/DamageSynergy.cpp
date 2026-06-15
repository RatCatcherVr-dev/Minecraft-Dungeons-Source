// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DamageSynergy.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"

const FName UDamageSynergyGameplayEffect::DamageBonusKey("DamageBonus");

UDamageSynergyGameplayEffect::UDamageSynergyGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = UDamageSynergyGameplayEffect::DamageBonusKey;

	damageInfo.ModifierMagnitude = dealDamageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Multiplicitive;

	Modifiers.Add(damageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DamageSynergy"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DamageSynergy"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Buff.DamageSynergy"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DamageSynergy.Melee"), 1, 1);
	GameplayCues.Last().MagnitudeAttribute = damageInfo.Attribute;
}

////////////////////////////////////////////////////////////////

UDamageSynergy::UDamageSynergy() {
	TypeId = EEnchantmentTypeID::DamageSynergy;
	Effect = UDamageSynergyGameplayEffect::StaticClass();
	LevelMultiplier = [this](int level) -> float {
		return BaseBonusDamagePercentage + (BonusDamagePercentagePerLevel * level);
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

void UDamageSynergy::OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	auto abilitySystem = GetAbilitySystemComponent();
	if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.DamageSynergy"))) {
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.DamageSynergy.Melee")));
	}
}

void UDamageSynergy::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index,
	FRandomStream& randStream, FSharedPredictionContext window) {
	RemovePendingBuff();
}

void UDamageSynergy::OnItemSuccess() {

	// apply the game status effect.  we'll consume it after a melee hit
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Effect.GetDefaultObject(), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(UDamageSynergyGameplayEffect::DamageBonusKey, LevelMultiplier(Level));
	EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UDamageSynergy::OnEnd() {
	Super::OnEnd();
	RemovePendingBuff();
}

void UDamageSynergy::RemovePendingBuff() const {
	auto abilitySystem = GetAbilitySystemComponent();
	abilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.DamageSynergy")));

}

