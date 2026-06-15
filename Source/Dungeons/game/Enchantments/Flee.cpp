#include "Flee.h"
#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"

namespace {
	FName FleeMovementSpeedEffectMagnitude(TEXT("FleeMovementSpeedEffectMagnitude"));
	FName FleeDurationMagnitude(TEXT("FleeDurationMagnitude"));
}

UFlee::UFlee() {
	TypeId = EEnchantmentTypeID::Flee;
	ServerOnlyExecution = true;

	FleeEffect = UFleeGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (percentPerLevel * float(level));
	};

	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

FText UFlee::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asDurationRoundedWordSecond(resetDuration)));
}

void UFlee::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	FGameplayTagContainer tags = effects::GetDungeonsDamageTypes(data);
	if (GetOwner()->HasAuthority()) {
		ApplyEffect();
	}
}

void UFlee::ApplyEffect() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
		FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, FleeEffect, Level));
		float multiplier = GetOwner()->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : 3.0f;
		spec.SetSetByCallerMagnitude(::FleeMovementSpeedEffectMagnitude, multiplier);
		spec.SetSetByCallerMagnitude(::FleeDurationMagnitude, resetDuration);
		EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

		if (EffectHandle.WasSuccessfullyApplied() && GetOwnerRole() == ROLE_Authority) {
			BroadcastEnchantmentTriggeredEvent(false);
		}
	}
}

UFleeGameplayEffect::UFleeGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat duration;
	duration.DataName = ::FleeDurationMagnitude;
	DurationMagnitude = duration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FSetByCallerFloat movementSpeedMagnitude;
	movementSpeedMagnitude.DataName = ::FleeMovementSpeedEffectMagnitude;

	FGameplayModifierInfo movementSpeedInfo;
	movementSpeedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	movementSpeedInfo.ModifierMagnitude = movementSpeedMagnitude;
	movementSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(movementSpeedInfo);
	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Flee"), 0, 3);
}