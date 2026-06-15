#include "InstantTransmission.h"
#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>

UInstantTransmissionGameplayEffect::UInstantTransmissionGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	// Dump old cues

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();
	info.ModifierOp = EGameplayModOp::Override;
	info.ModifierMagnitude = FScalableFloat(0.f);
	Modifiers.Add(info);

	GameplayCues.Empty();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.InstantTransmission"), 0, 1);
}

USuppressRollModeGameplayEffect::USuppressRollModeGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	DurationPolicy = EGameplayEffectDurationType::Infinite;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.SuppressRollMode"));
}

UInstantTransmission::UInstantTransmission() : RollEffect(UInstantTransmissionGameplayEffect::StaticClass()) {
	TypeID = EArmorPropertyID::DodgeSpeedIncrease;
}

void UInstantTransmission::BeginPlay() {
	Super::BeginPlay();

	if (auto ownerCharacter = GetOwnerCharacter()) {
		if (auto abilitySystem = ownerCharacter->GetAbilitySystemComponent()) {
			auto suppressRollSpec = effects::CreateGameplayEffectSpec<USuppressRollModeGameplayEffect>(abilitySystem, 1);
			rollSuppressEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(suppressRollSpec);
		}
	}
}

void UInstantTransmission::EndPlay(const EEndPlayReason::Type) {
	RemoveGameplayEffect(true);
}

void UInstantTransmission::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
 	if (GetOwnerRole() == ENetRole::ROLE_Authority && timerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	}

	RemoveGameplayEffect();

	if (auto abilitySystem = GetOwnerCharacter()->GetAbilitySystemComponent()) {
		effectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(FGameplayEffectSpec(RollEffect.GetDefaultObject(), abilitySystem->MakeEffectContext(), 1), key);
		FGameplayCueParameters params;
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Teleport.End"), params);
	}
}

void UInstantTransmission::OnDodgeRollEnd(FPredictionKey key) {
	GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateUObject(this, &UInstantTransmission::RemoveGameplayEffect, false), PostDodgeDuration, false);
}

void UInstantTransmission::OnBeforeRangedAttack(AActor * attackTarget, bool& attackDenied, FPredictionKey key) {
	RemoveGameplayEffect();
}

void UInstantTransmission::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	RemoveGameplayEffect();
}

void UInstantTransmission::OnItemActivated(const AItemInstance*, FPredictionKey key) {
	RemoveGameplayEffect();
}

void UInstantTransmission::RemoveGameplayEffect(bool removeRollSuppress) {
	if (auto character = GetOwnerCharacter()) {
		if (auto abilitySystem = character->GetAbilitySystemComponent()) {
			if (abilitySystem->RemoveActiveGameplayEffect(effectHandle)) {
				abilitySystem->RemoveActiveGameplayEffectBySourceEffect(RollEffect, abilitySystem);
				FGameplayCueParameters params;
				abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Teleport.End"), params);
			}

			if (removeRollSuppress) {
				abilitySystem->RemoveActiveGameplayEffect(rollSuppressEffectHandle);
			}
		}
	}
}
