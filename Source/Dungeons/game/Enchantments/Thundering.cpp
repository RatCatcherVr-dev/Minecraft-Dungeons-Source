// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Thundering.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "AbilitySystemComponent.h"
#include <AbilitySystemGlobals.h>
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "util/CharacterQuery.h"
#include "../item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "ChainLightning.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;

UThunderingAoeDamageGameplayEffect::UThunderingAoeDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Damage.Aoe"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UThunderingAoeRangedDamageGameplayEffect::UThunderingAoeRangedDamageGameplayEffect() {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UItemPowerOnlyAsEffectLevelModDamageCalculation::StaticClass();

	Modifiers.Last().ModifierMagnitude = healthMagnitude;
}

UThundering::UThundering() {
	TypeId = EEnchantmentTypeID::Thundering;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseDamage * level;
	};
	ItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

FText UThundering::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)));
}

void UThundering::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext context) {
	if ((randStream.FRand() < TriggerChance) || bAlwaysTrigger) {
		bCanTriggerThundering = true;
	}
}

void UThundering::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (bCanTriggerThundering) {
		auto characterOwner = GetCharacterOwner();
		auto characterTarget = Cast<ABaseCharacter>(toWhom);

		if (!characterTarget) return;

		const auto abilitySystem = characterOwner->GetAbilitySystemComponent();
		if (GetOwner()->HasAuthority()) {
			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UThunderingAoeDamageGameplayEffect>(abilitySystem, effects::HealthName, IsOwnerMob() ? -MobDamage : -LevelMultiplier(Level), GetOwner(), GetOwner(), fromLocation, Level);
			effects::SetStunMultiplier(spec, effects::ENCHANTMENT_STUN_MULTIPLIER);

			const auto hostile = characterquery::is::hostile(characterOwner);
			const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

			TArray<ABaseCharacter*> allTargets = actorquery::getNearbyActors<ABaseCharacter>(characterTarget, IndividualChainRadius).FilterByPredicate(predicate);
			TArray<ABaseCharacter*> aoeTargets = allTargets.FilterByPredicate(actorquery::is::inRange(toWhat, StrikeRadius));

			if (predicate(characterTarget)) {
				aoeTargets.Add(characterTarget);
			}


			if (CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), toWhat->GetActorLocation(), StrikeRadius, 50, FColor::Green, false, 3.0f);

			allTargets.RemoveAllSwap(actorquery::is::inRange(characterTarget, StrikeRadius));

			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
			for (auto target : aoeTargets) {
				const auto targetAbilitySystem = target->GetAbilitySystemComponent();
				abilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem, context.GetKey());
			}
			BroadcastEnchantmentTriggeredEvent();

			if (allTargets.Num()) {
				FGameplayEffectSpec chainLightningSpec = effects::CreateGameplayEffectSpec<UMeleeChainLightningDamageGameplayEffect>(abilitySystem, Level);
				effects::SetStunMultiplier(chainLightningSpec, effects::ENCHANTMENT_STUN_MULTIPLIER);
				chainLightningSpec.SetSetByCallerMagnitude(effects::HealthName, IsOwnerMob() ? -MobDamage : -LevelMultiplier(Level));
				chainLightningSpec.GetContext().AddInstigator(GetOwner(), GetOwner());

				chainlightning::ApplyChainLightning(characterTarget, TArray<TWeakObjectPtr<ABaseCharacter>>(allTargets), ChainLightningDelay, IndividualChainRadius, chainLightningSpec);
			}
		}
		
		FGameplayCueParameters params;
		params.Location = atLocation;
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Thundering.Strike"), params);
		
		bCanTriggerThundering = false;
	}
}