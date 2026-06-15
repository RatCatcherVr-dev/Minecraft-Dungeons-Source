#include "GuardingStrike.h"
#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName GuardingStrikeEffectMagnitude(TEXT("GuardingStrikeEffectMagnitude"));
	FName GuardingStrikeDurationMagnitude(TEXT("GuardingStrikeDurationMagnitude"));
}

UGuardingStrike::UGuardingStrike() {
	TypeId = EEnchantmentTypeID::GuardingStrike;
	ServerOnlyExecution = true;

	GuardingStrikeEffect = UGuardingStrikeGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return effectBaseDuration + effectDurationPerLevel * float(level - 1);
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
}

FText UGuardingStrike::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asMultiplierPercentageChange(damageReduction)));
}

void UGuardingStrike::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	if (GetOwner()->HasAuthority()) {
		if (ABaseCharacter* attackTarget = Cast<ABaseCharacter>(toWhom)) {
			if (attackTarget->IsNotAlive()) {
				ApplyEffect();
			}
		}
	}
}

void UGuardingStrike::ApplyEffect() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
		FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, GuardingStrikeEffect, Level));
		float duration = GetOwner()->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : 3.0f;
		spec.SetSetByCallerMagnitude(::GuardingStrikeEffectMagnitude, damageReduction);
		spec.SetSetByCallerMagnitude(effects::DurationName, duration);
		EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

		if (EffectHandle.WasSuccessfullyApplied() && GetOwnerRole() == ROLE_Authority) {
			BroadcastEnchantmentTriggeredEvent(false);
		}
	}
}

UGuardingStrikeGameplayEffect::UGuardingStrikeGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	FSetByCallerFloat resistanceMagnitude;
	resistanceMagnitude.DataName = ::GuardingStrikeEffectMagnitude;

	FGameplayModifierInfo resistanceInfo;
	resistanceInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	resistanceInfo.ModifierMagnitude = resistanceMagnitude;
	resistanceInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(resistanceInfo);
	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.GuardingStrike"), 0, 1);
}