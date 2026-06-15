#include "Dungeons.h"
#include "StrengthPotionInstance.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName StrengthEffectDuration(TEXT("StrengthDuration"));
	FName StrengthDealDamageEffectMagnitude(TEXT("StrengthDealDamageMagnitude"));
	FName StrengthPushbackEffectMagnitude(TEXT("StrengthPushbackMagnitude"));
}

AStrengthPotionInstance::AStrengthPotionInstance() {
	Effect = UStrengthPotionGameplayEffect::StaticClass();
}

int AStrengthPotionInstance::GetDisplayCount() const {
	return -1;
}

void AStrengthPotionInstance::Activate(const FPredictionKey& predictionKey) {
	auto playerOwner = Cast<APlayerCharacter>(GetOwner());

	auto abilitySystem = playerOwner->GetAbilitySystemComponent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<UStrengthPotionGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(effects::DurationName, game::item::type::StrengthPotion.getDurationSeconds());
	spec.SetSetByCallerMagnitude(::StrengthDealDamageEffectMagnitude, StrengthBoostAmount);
	spec.SetSetByCallerMagnitude(::StrengthPushbackEffectMagnitude, StrengthBoostAmount);
	auto handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	Super::Activate(predictionKey);
}


UStrengthPotionGameplayEffect::UStrengthPotionGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo meleeDamageInfo;
	meleeDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat meleeDamageMagnitude;
	meleeDamageMagnitude.DataName = ::StrengthDealDamageEffectMagnitude;

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::StrengthDealDamageEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);

	FGameplayModifierInfo pushbackInfo;
	pushbackInfo.Attribute = UMeleeAttributeSet::MeleeAttackPushbackMultiplierAttribute();

	FSetByCallerFloat pushbackMagnitude;
	pushbackMagnitude.DataName = ::StrengthPushbackEffectMagnitude;

	pushbackInfo.ModifierMagnitude = pushbackMagnitude;
	pushbackInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(pushbackInfo);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Strength"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.Strength"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Buff.Strength"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}


