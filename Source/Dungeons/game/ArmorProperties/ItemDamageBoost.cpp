// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "ItemDamageBoost.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/ItemAttributeSet.h"

namespace {
	FName ItemDealDamageEffectMagnitude(TEXT("ItemDealDamageMagnitude"));
}

void UItemDamageBoost::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UItemDamageBoostGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(::ItemDealDamageEffectMagnitude, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UItemDamageBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UItemDamageBoost::UItemDamageBoost() {
	TypeID = EArmorPropertyID::ItemDamageBoost;
}

UItemDamageBoostGameplayEffect::UItemDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UItemAttributeSet::ItemDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::ItemDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}