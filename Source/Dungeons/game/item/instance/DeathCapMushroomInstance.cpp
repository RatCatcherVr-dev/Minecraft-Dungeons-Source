#include "Dungeons.h"
#include "DeathCapMushroomInstance.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"


ADeathCapMushroomInstance::ADeathCapMushroomInstance() {
	Effect = UDeathCapMushroomGameplayEffect::StaticClass();
	PowerEffects = { UDurationIncrease::StaticClass() };
}

int ADeathCapMushroomInstance::GetDisplayCount() const {
	// Always return 0 on permanent items, unless it has a special condition like the harvester, makes no number displayed
	return 0;
}

void ADeathCapMushroomInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = GetOwner();
	auto playerOwner = Cast<ABaseCharacter>(owner);
	UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(Cast<UDeathCapMushroomGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	float duration = GetItemType().getDurationSeconds() * ItemPowerMultiplier;
	spec.SetSetByCallerMagnitude(TEXT("Duration"), duration);
	spec.SetSetByCallerMagnitude(TEXT("AttackSpeedMagnitude"), AttackSpeedBuff);
	spec.SetSetByCallerMagnitude(TEXT("SpeedMagnitude"), MoveSpeedBuff);
	auto handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);
}


UDeathCapMushroomGameplayEffect::UDeathCapMushroomGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();
	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = TEXT("AttackSpeedMagnitude");

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(dealDamageInfo);

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = TEXT("SpeedMagnitude");

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(speedInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.DeathCapMushroom"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.DeathCapMushroom"), 0, 1);

	//StatusEffect.DisableRangedAttack
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.DisableRangedAttack.DeathCap"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}


