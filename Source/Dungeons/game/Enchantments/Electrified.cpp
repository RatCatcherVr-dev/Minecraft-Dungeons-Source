// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Electrified.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "util/RandomUtil.h"
#include "util/Algo.hpp"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/util/ValueFormat.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UElectrifiedDamageGameplayEffect::UElectrifiedDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Electrified.Reciever"));
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}


FText UElectrified::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asConstantWord(ZapCount)));
}

UElectrified::UElectrified() {
	LevelMultiplier = [this](int level) -> float {
		return ZapDamage * level;
	};

	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;

	TypeId = EEnchantmentTypeID::Electrified;	
	PredictiveExecution = true;
}

void UElectrified::Execution() {
	if (GetOwner()->HasAuthority()) {
		OnZap();
	}
}

bool UElectrified::ShouldTriggerOnRoll() const {
	return GetCharacterOwner()->IsA(APlayerCharacter::StaticClass());
}

float UElectrified::GetExecutionInterval() const {
	return ZapDelayMob;
}

void UElectrified::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	if(ShouldTriggerOnRoll()){
		StartElectrifiedEffect(key);
	}
}

void UElectrified::OnDodgeRollEnd(FPredictionKey key){
	if (ShouldTriggerOnRoll()) {
		if (GetOwner()->HasAuthority()) {
			BroadcastEnchantmentTriggeredEvent();
		}

		OnZap(key);
		StopElectrifiedEffect(key);
	}
}

bool UElectrified::ShouldIntervalExecute() const {
	return Super::ShouldIntervalExecute() && !ShouldTriggerOnRoll();
}

void UElectrified::IntervalExecutionStarted() {
	StartElectrifiedEffect();
}

void UElectrified::IntervalExecutionStopped() {
	StopElectrifiedEffect();
}

void UElectrified::StopElectrifiedEffect(FPredictionKey key){
	auto ownerAbilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	Swap(ownerAbilitySystem->ScopedPredictionKey, key);
	ownerAbilitySystem->RemoveGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Electrified"));
	Swap(ownerAbilitySystem->ScopedPredictionKey, key);

}

void UElectrified::StartElectrifiedEffect(FPredictionKey key) {
	auto ownerAbilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();	
	Swap(ownerAbilitySystem->ScopedPredictionKey, key);
	ownerAbilitySystem->AddGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Electrified"), FGameplayCueParameters());
	Swap(ownerAbilitySystem->ScopedPredictionKey, key);
}

void UElectrified::OnZap(FPredictionKey key) {
	auto characterOwner = GetCharacterOwner();
	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
	
	if (characterOwner->HasAuthority()) {
		const auto hostile = characterquery::is::hostile(characterOwner);
		const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

		TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);

		const int zapCount = characterOwner->IsA(APlayerCharacter::StaticClass()) ? ZapCount : ZapCountMob;
		const float zapDamage = characterOwner->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : MobZapDamage;

		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal | EGameplayCueParametersField::Instigator);

		for (int times = 0; times < zapCount; times++) {
			if (!targets.Num()) {
				break;
			}
			auto randomIndex = FMath::RandRange(0, targets.Num() - 1);
			auto target = targets[randomIndex];
			auto targetAbilitySystem = target->GetAbilitySystemComponent();
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UElectrifiedDamageGameplayEffect>(ownerAbilitySystem, Level);
			effects::StorePushbackInNormal(spec, pushback::getLaunchVector(OnKillPushback, *characterOwner, *target, 1.0f, 1.0f));
			effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);
			spec.GetContext().AddInstigator(GetOwner(), target);
			spec.SetSetByCallerMagnitude(effects::HealthName, -zapDamage);
			ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
			targets.RemoveAtSwap(randomIndex);
		}
	}

	Swap(ownerAbilitySystem->ScopedPredictionKey, key);
	ownerAbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Electrified.User"), FGameplayCueParameters());
	Swap(ownerAbilitySystem->ScopedPredictionKey, key);
}