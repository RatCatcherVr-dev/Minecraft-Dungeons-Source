// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Frenzied.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace {
	FName FrenziedSpeedMagnitude(TEXT("FenziedSpeedMagnitude"));
}

UFrenzied::UFrenzied() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 2.0f;

	TypeId = EEnchantmentTypeID::Frenzied;
	FrenziedEffect = UFrenziedGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (0.1f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}


FText UFrenzied::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRoundedFractionalQuarters(TriggerThreshold)));
}

void UFrenzied::OnStart() {
	if (GetOwner()->HasAuthority()) {
		if (GetShouldTrigger()) {
			ApplyEffect();
		}

		if (auto HC = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			HC->OnHeal.AddUObject(this, &UFrenzied::OnHeal);
		}
	}
}

void UFrenzied::OnEnd() {
	if (GetOwner()->HasAuthority()) {
		if (IsEffectActive()) {
			RemoveEffect();
		}
	}
}

void UFrenzied::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		if (GetShouldTrigger() && !IsEffectActive()) {
			ApplyEffect();
		}
	}
}

void UFrenzied::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick called every other second, only to ensure that effect isn't wrongly applied mainly when starting map and health and maxhealth aren't in sync
	if (GetOwner()->HasAuthority()) {
		if (GetShouldTrigger()) {
			if (!IsEffectActive()) {
				ApplyEffect();
			}
		}
		else {
			if (IsEffectActive()) {
				RemoveEffect();
			}
		}
	}
}

bool UFrenzied::GetShouldTrigger() const {
	if (const auto* HC = GetOwner()->FindComponentByClass<UHealthComponent>()) {
		return HC->GetCurrentHealth() < (HC->GetMaximumHealth() * TriggerThreshold);
	}
	return false;
}

bool UFrenzied::IsEffectActive() const {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		return abilitySystem->GetActiveGameplayEffect(EffectHandle) != nullptr ? true : false;
	}
	return false;
}

void UFrenzied::ApplyEffect() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, FrenziedEffect, Level));
		float multiplier = GetOwner()->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : 3.0f;
		spec.SetSetByCallerMagnitude(::FrenziedSpeedMagnitude, multiplier);
		EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UFrenzied::RemoveEffect() {
	GetCharacterOwner()->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(EffectHandle);
}

void UFrenzied::OnHeal(float amount) {
	if (!GetShouldTrigger()) {
		RemoveEffect();
	}
}

UFrenziedGameplayEffect::UFrenziedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo meleeSpeedInfo;
	meleeSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	FSetByCallerFloat meleeSpeedMagnitude;
	meleeSpeedMagnitude.DataName = ::FrenziedSpeedMagnitude;

	meleeSpeedInfo.ModifierMagnitude = meleeSpeedMagnitude;
	meleeSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeSpeedInfo);

	FGameplayModifierInfo rangedinfo;
	rangedinfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();

	FSetByCallerFloat rangedSpeedMagnitude;
	rangedSpeedMagnitude.DataName = ::FrenziedSpeedMagnitude;

	rangedinfo.ModifierMagnitude = rangedSpeedMagnitude;
	rangedinfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedinfo);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Frenzied"), 0, 1);
}