// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Explorer.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Async.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


UExplorer::UExplorer() {
	TypeId = EEnchantmentTypeID::Explorer;
	LevelMultiplier = [](int level) -> float {
		return  0.0033f * (float)level;
	};
	MultiplierFormatter = valueformat::asSingleDecimalPercentage;
}

void UExplorer::OnStart() {
	if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
		playerCharacter->OnMapBlocksRevealed.AddUObject(this, &UExplorer::OnBlocksExplored);
	}
}

void UExplorer::OnBlocksExplored(int amount) {
	blockCounter += amount;
	if (blockCounter > BlockThreshold) {
		int diff = blockCounter - BlockThreshold;
		blockCounter = diff;

		if (TryHeal()) {
			TryApplyEffect();
		}
	}
}

bool UExplorer::TryHeal() {
	auto characterOwner = GetCharacterOwner();
	if (auto HC = characterOwner->FindComponentByClass<UHealthComponent>()) {
		float maxHealth = HC->GetMaximumHealth();
		if (HC->GetCurrentHealth() < maxHealth) {
			if (characterOwner->HasAuthority()) {
				BroadcastEnchantmentTriggeredEvent();
			}
			float healthIncrease = maxHealth * LevelMultiplier(Level);
			HC->ApplyHeal(healthIncrease);

			return true;
		}
	}
	return false;
}

void UExplorer::TryApplyEffect() {
	auto characterOwner = GetCharacterOwner();
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	if (!abilitySystem->GetActiveGameplayEffect(EffectHandle)) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UExplorerGameplayEffect>(abilitySystem);
		EffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

		FGameplayCueParameters params;
		params.Instigator = GetOwner();
		params.Location = GetOwner()->GetActorLocation();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Explorer"), params);
	}
	GetWorld()->GetTimerManager().SetTimer(ResetTimerHandle, this, &UExplorer::OnResetEffect, 2.0f);
}

void UExplorer::OnResetEffect() {
	auto characterOwner = GetCharacterOwner();
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(EffectHandle);
}

UExplorerGameplayEffect::UExplorerGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Explorer"), 0, 1);
}
