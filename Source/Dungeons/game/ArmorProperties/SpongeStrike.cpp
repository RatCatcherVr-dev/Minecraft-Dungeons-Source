// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SpongeStrike.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include <AbilitySystemGlobals.h>
#include "util/CharacterQuery.h"
#include "../item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "ArmorPropertyEnumTypes.h"
#include "GameplayEffectExtension.h"

USpongeStrike::USpongeStrike() {
	TypeId = EEnchantmentTypeID::SpongeStrike;
}

void USpongeStrike::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream)
{
	if (data.EvaluatedData.Magnitude < 0) {
		// divide by 5 as internal health is 5x greater than we expect.
		DamageTaken += data.EvaluatedData.Magnitude / 5;
	}
}

void USpongeStrike::OnAfterMeleeAttack(AActor* attackTarget, int hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window)
{
	if (const auto meleeAttackComponent = GetOwner()->FindComponentByClass<UMeleeAttackComponent>())
	{
		const auto activeAttackVariants = meleeAttackComponent->GetActiveAttackVariants();

		if (index == 0)
		{
			DamageTaken = 0.f;
		}

		if (activeAttackVariants.Num() > 0) {
			if ((activeAttackVariants.Num() - 1) == index) {
				if (GetOwnerRole() == ROLE_Authority)
				{
					DoSpongeStrikeDamage(attackTarget);
				}
			}
		}
	}
}

void USpongeStrike::OnAfterResurrection()
{
	DamageTaken = 0.f;
}

void USpongeStrike::DoSpongeStrikeDamage(AActor* attackTarget)
{
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal | EGameplayCueParametersField::Instigator);
	auto characterOwner = GetCharacterOwner();
	UAbilitySystemComponent* ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
	UAbilitySystemComponent* targetAbilitySystem = attackTarget ? attackTarget->FindComponentByClass<UAbilitySystemComponent>() : nullptr;

	if (ownerAbilitySystem && targetAbilitySystem && !FMath::IsNearlyZero(DamageTaken))
	{
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USpongeStrikeDamageGameplayEffect>(ownerAbilitySystem, effects::HealthName, DamageTaken, characterOwner, characterOwner, characterOwner->GetActorLocation(), Level);
		FPredictionKey key = ownerAbilitySystem->ScopedPredictionKey;
		ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem, key);
	}

	DamageTaken = 0.f;
}

void USpongeStrike::BeginPlay()
{
	Super::BeginPlay();
}

USpongeStrikeDamageGameplayEffect::USpongeStrikeDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UMeleeModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Damage.Medium"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
}
