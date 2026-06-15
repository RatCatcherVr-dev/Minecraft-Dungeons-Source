#include "Dungeons.h"
#include "Rampaging.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/component/HealthComponent.h"
#include "game/util/ValueFormat.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName RampagingAttackSpeedEffectMagnitude(TEXT("RampagingAttackSpeedEffectMagnitude"));
	FName RampagingDurationEffectMagnitude(TEXT("RampagingDurationEffectMagnitude"));
}

URampaging::URampaging() {
	Effect = URampagingGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Rampaging;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BoostTime * (1.0f * (level));
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
}

FText URampaging::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)), FText::FromString(valueformat::asRelativeMultiplierPercentageChange(AttackSpeedBoost)));
}


void URampaging::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	auto owner = GetOwner();
	auto characterOwner = GetCharacterOwner();
	auto mobHC = toWhat->FindComponentByClass<UHealthComponent>();
	if (mobHC && (mobHC->IsNotAlive() || IsOwnerMob()) && (randStream.FRand() <= TriggerChance || bAlwaysTrigger)) {
		if(GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();

		FGameplayEffectSpec spec(Cast<URampagingGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude(::RampagingAttackSpeedEffectMagnitude, AttackSpeedBoost);
		spec.SetSetByCallerMagnitude(effects::DurationName, LevelMultiplier(Level));

		abilitySystem->ApplyGameplayEffectSpecToSelf(spec, context.GetKey());
	}
}

URampagingGameplayEffect::URampagingGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	FGameplayModifierInfo meleeSpeedInfo;
	meleeSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	FSetByCallerFloat meleeSpeedMagnitude;
	meleeSpeedMagnitude.DataName = ::RampagingAttackSpeedEffectMagnitude;

	meleeSpeedInfo.ModifierMagnitude = meleeSpeedMagnitude;
	meleeSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeSpeedInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Rampaging"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Rampaging"), 0, 1);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}
