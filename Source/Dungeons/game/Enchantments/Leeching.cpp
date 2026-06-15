// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Leeching.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include <GameplayEffect.h>
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

ULeechingGameplayEffect::ULeechingGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UHealingModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.LeechHeal"), 0, 100);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

ULeeching::ULeeching() {
	TypeId = EEnchantmentTypeID::Leeching;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseHealFactor + (level-1) * HealPerLevelFactor;
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void ULeeching::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	auto characterOwner = GetCharacterOwner();
	auto characterTarget = Cast<ABaseCharacter>(toWhom);
	if (characterTarget) {
		auto mobHC = toWhat->FindComponentByClass<UHealthComponent>();
		auto playerHC = characterOwner->FindComponentByClass<UHealthComponent>();
		if (mobHC && mobHC->IsNotAlive()) {

			if (GetOwnerRole() == ROLE_Authority && !playerHC->IsHealthMaxed()) {
				BroadcastEnchantmentTriggeredEvent();
			}

			UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<ULeechingGameplayEffect>(abilitySystem, Level);
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
			
			spec.SetSetByCallerMagnitude(effects::HealthName, mobHC->GetMaximumHealth() * LevelMultiplier(Level));
			spec.GetContext().AddInstigator(characterOwner, characterOwner);
			spec.GetContext().AddSourceObject(this);
			spec.GetContext().AddOrigin(atLocation);

			abilitySystem->ApplyGameplayEffectSpecToSelf(spec, context.GetKey());
		}
	}
}
