// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <GameplayPrediction.h>
#include "game/component/MeleeAttackComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/util/Pushback.h"
#include "GameplayEffectDamageComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"


UGameplayEffectDamageComponent::UGameplayEffectDamageComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	DamageEffect = UWorldDamageGameplayEffect::StaticClass();
}

void UGameplayEffectDamageComponent::AttackLocal(ABaseCharacter* target) {
	if (GetOwner()->HasAuthority()) {
		if (const auto targetAbilitySystem = target->GetAbilitySystemComponent()) {
			ApplyDamage(target);
		}
	}
	else {
		UE_LOG(LogDungeons, Warning, TEXT("Can't apply damage from client"));
	}
}

void UGameplayEffectDamageComponent::ApplyDamage(ABaseCharacter* target) {
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::SourceObject);
	UAbilitySystemComponent* targetAbilitySystem = target->GetAbilitySystemComponent();
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(targetAbilitySystem, DamageEffect);

	spec.SetSetByCallerMagnitude(effects::HealthName, -Damage);
	FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
	spec.GetContext().AddInstigator(GetOwner(), GetOwner());
	spec.GetContext().AddOrigin(GetOwner()->GetActorLocation());
	spec.GetContext().AddSourceObject(GetOwner());

	context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Impact")));

	targetAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	pushback::pushback(Pushback, *GetOwner(), *target);
}

void UGameplayEffectDamageComponent::BeginPlay() {
	Super::BeginPlay();
}