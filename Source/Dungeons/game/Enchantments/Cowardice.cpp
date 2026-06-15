// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Cowardice.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace {
	FName CowardiceMeleeDamageEffectMagnitude(TEXT("CowardiceMeleeDamageEffectMagnitude"));
	FName CowardiceRangedDamageEffectMagnitude(TEXT("CowardiceRangedDamageEffectMagnitude"));
}

UCowardice::UCowardice() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 2.0f;

	Effect = UCowardiceGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Cowardice;

	LevelMultiplier = [this](int level) {
		return 1.1f + (0.1f * (float)level);
	};

	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UCowardice::OnStart() {
	Super::OnStart();
	if (GetOwner()->HasAuthority()) {
		if (const auto HC = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			HC->OnHeal.AddUObject(this, &UCowardice::OnHeal);
			HealthComponent = HC;
		}
		if (HasFullHealth()) {
			ApplyEffect();
		}
	}
}

void UCowardice::OnEnd() {
	Super::OnEnd();
	RemoveEffect();
}

void UCowardice::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		if (!HasFullHealth()) {
			RemoveEffect();
		}
	}
}

void UCowardice::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick called every other second, only to ensure that effect isn't wrongly applied mainly when starting map and health and maxhealth aren't in sync
	if (GetOwner()->HasAuthority()) {
		if (HasFullHealth()) {
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

bool UCowardice::HasFullHealth() const {
	if (HealthComponent) {
		if (HealthComponent->IsHealthMaxed()) {
			return true;
		}
	}
	return false;
}

bool UCowardice::IsEffectActive() const {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		return abilitySystem->GetActiveGameplayEffect(EffectHandle) != nullptr ? true : false;
	}
	return false;
}

void UCowardice::ApplyEffect() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec(Cast<UCowardiceGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		float multiplier = LevelMultiplier(Level);
		spec.SetSetByCallerMagnitude(::CowardiceMeleeDamageEffectMagnitude, multiplier);
		spec.SetSetByCallerMagnitude(::CowardiceRangedDamageEffectMagnitude, multiplier);
		EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UCowardice::RemoveEffect() {
	GetCharacterOwner()->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(EffectHandle);
}

void UCowardice::OnHeal(float amount) {
	if (HasFullHealth() && !IsEffectActive()) {
		ApplyEffect();
	}
}

UCowardiceGameplayEffect::UCowardiceGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo meleeDamageInfo;
	meleeDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat meleeDamageMagnitude;
	meleeDamageMagnitude.DataName = ::CowardiceMeleeDamageEffectMagnitude;

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::CowardiceRangedDamageEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Cowardice"), 0, 1);
}
