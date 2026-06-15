#include "Dungeons.h"
#include "BurningBrewPotionInstance.h"

#include "AbilitySystemComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/GamplayEffects/Drowning.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

ABurningBrewPotionInstance::ABurningBrewPotionInstance()
{
	Effect = UBurningBrewPotionGameplayEffect::StaticClass();
}

int ABurningBrewPotionInstance::GetDisplayCount() const
{
	return -1;
}

void ABurningBrewPotionInstance::Activate(const FPredictionKey& predictionKey)
{
	SetLifeSpan(game::item::type::BurningBrewPotion.getDurationSeconds() + 0.1f);

	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto effect = Cast<UBurningBrewPotionGameplayEffect>(Effect->GetDefaultObject());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	FGameplayEffectSpec spec(effect, abilitySystem->MakeEffectContext(), ItemPower);
	spec.SetSetByCallerMagnitude(TEXT("Duration"), game::item::type::BurningBrewPotion.getDurationSeconds());
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

	Super::Activate(predictionKey);

	FTimerHandle BurnTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(BurnTimerHandle, this, &ABurningBrewPotionInstance::CheckTargets, BurnInterval, true);

	//D11.RR - 'Burning Up' Achievement
	BurningBrewActive(true);
}

void ABurningBrewPotionInstance::CheckTargets()
{
	auto characterOwner = GetCharacterOwner();
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) { return hostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v);  };

	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, Radius).FilterByPredicate(predicate);

	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();

	float damage = MobBurnDamage;
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	for (auto target : targets) {
		auto targetAbilitySystem = target->GetAbilitySystemComponent();
		FGameplayEffectSpec targetspec = effects::CreateGameplayEffectSpec<UBurningBrewPotionDamageGameplayEffect>(ownerAbilitySystem, 1.f);
		targetspec.SetSetByCallerMagnitude(effects::HealthName, -damage);
		ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(targetspec, targetAbilitySystem);
	}
}

void ABurningBrewPotionInstance::LifeSpanExpired()
{
	BurningBrewActive(false);

	Super::LifeSpanExpired();
}

void ABurningBrewPotionInstance::BurningBrewActive(bool isActive)
{
	if (auto* tracker = Cast<APlayerCharacter>(GetOwner())->GetStatTracker()) {
		tracker->BurningBrew(isActive);
	}
	else {
		Client_BurningBrewActive(isActive);
	}
}

void ABurningBrewPotionInstance::Client_BurningBrewActive_Implementation(bool isActive)
{
	if (auto* tracker = Cast<APlayerCharacter>(GetOwner())->GetStatTracker()) {
		tracker->BurningBrew(isActive);
	}
}

UBurningBrewPotionDamageGameplayEffect::UBurningBrewPotionDamageGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Burning"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDifficultyTargetDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}

UBurningBrewPotionGameplayEffect::UBurningBrewPotionGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.BurningBrew"), 0, 1);
}
