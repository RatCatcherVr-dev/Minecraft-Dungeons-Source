#include "Dungeons.h"
#include "BurningOilVialInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <GameplayEffect.h>
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


ABurningOilVialInstance::ABurningOilVialInstance() {
	Effect = UBurningOilVialGameplayEffect::StaticClass();
}

int ABurningOilVialInstance::GetDisplayCount() const {
	return -1;
}

void ABurningOilVialInstance::Activate(const FPredictionKey& predictionKey) {
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	auto effect = Cast<UBurningOilVialGameplayEffect>(Effect->GetDefaultObject());
	effect->ProjectileClass = ProjectileClass;

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(TEXT("Duration"), game::item::type::BurningOilVial.getDurationSeconds());
	spec.SetSetByCallerMagnitude(TEXT("Magnitude"), ArrowPowerBoostAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	Super::Activate(predictionKey);
}

UBurningOilVialGameplayEffect::UBurningOilVialGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = TEXT("Duration");

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName =TEXT("Magnitude");

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.BurningOilVial"), 0, 1);
}


