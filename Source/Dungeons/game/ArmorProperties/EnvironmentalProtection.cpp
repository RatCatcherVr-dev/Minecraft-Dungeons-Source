// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "EnvironmentalProtection.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>
#include "../abilities/attributes/FrozenSolidIceCubeAttributeSet.h"

// ----- ENVIRONMENTAL PROTECTION GAMEPLAY EFFECT ----- //
UEnvironmentalProtectionGameplayEffect::UEnvironmentalProtectionGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FScalableFloat OwnerInteractionMagnitude = FrozenSolidOwnerInteractionIncrement;
	FGameplayModifierInfo OwnerInteractionInfo;
	OwnerInteractionInfo.Attribute = UFrozenSolidIceCubeAttributeSet::OwnerInteractionMagnitudeAttribute();
	OwnerInteractionInfo.ModifierOp = EGameplayModOp::Type::Additive;
	OwnerInteractionInfo.ModifierMagnitude = OwnerInteractionMagnitude;

	Modifiers.Add(OwnerInteractionInfo);

	FScalableFloat OtherInteractionMagnitude = FrozenSolidOtherInteractionIncrement;
	FGameplayModifierInfo OtherInteractionInfo;
	OtherInteractionInfo.Attribute = UFrozenSolidIceCubeAttributeSet::OtherInteractionMagnitudeAttribute();
	OtherInteractionInfo.ModifierOp = EGameplayModOp::Type::Additive;
	OtherInteractionInfo.ModifierMagnitude = OtherInteractionMagnitude;
	Modifiers.Add(OtherInteractionInfo);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.EnvironmentalProtection"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::EnvironmentalProtectionMagnitudeAttribute(), EGameplayModOp::Override);
}

// ----- ENVIRONMENTAL PROTECTION ARMOUR PROPERTY ----- //
void UEnvironmentalProtection::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UEnvironmentalProtectionGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::EnvironmentalProtectionMagnitudeAttribute().GetName(), Math::max(0, 1 - Resistance));
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UEnvironmentalProtection::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

void UEnvironmentalProtection::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	FGameplayTagContainer tags;
	data.EffectSpec.GetAllAssetTags(tags);
	if (tags.HasTag(FGameplayTag::RequestGameplayTag("Damage.Environmental"))) {
		BroadcastArmorPropertyTriggeredEvent();
	}
}

UEnvironmentalProtection::UEnvironmentalProtection() {
	TypeID = EArmorPropertyID::EnvironmentalProtection;
}