#include "Dungeons.h"
#include "DodgeRoot.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

UDodgeRoot::UDodgeRoot() {
	TypeID = EArmorPropertyID::DodgeRoot;
}

void UDodgeRoot::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	using namespace characterquery;
	const auto hostile = is::hostile(GetOwnerCharacter());
	auto characters = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(GetOwner(), Range, RETLAMBDA(hostile(it) && is::targetable(it) && actorquery::is::alive(it)));
	auto owner = GetOwner();

	characters.Sort([&](const ABaseCharacter& a, const ABaseCharacter& b) { return actorquery::getActorDistanceSquared2D(owner, &a) < actorquery::getActorDistanceSquared2D(owner, &b); });
	characters.SetNum(FMath::Min(characters.Num(), NumberOfMobsToRoot));
	ApplyStun(characters, key);
}

FGameplayEffectSpec UDodgeRoot::CreateRootSpec() const {

	auto spec = effects::CreateGameplayEffectSpec<UDodgeRootGameplayEffect>(GetAbilitySystemComponent());
	spec.GetContext().AddInstigator(GetOwner(), GetOwner());
	spec.SetSetByCallerMagnitude(effects::DurationName, RootDuration);

	return spec;
}

FGameplayEffectSpec UDodgeRoot::CreatePoisonSpec() const {
	auto spec = effects::CreateGameplayEffectSpec<UDodgeRootPoisonGameplayEffect>(GetAbilitySystemComponent());
	spec.GetContext().AddInstigator(GetOwner(), GetOwner());
	spec.SetSetByCallerMagnitude(effects::DurationName, RootDuration);
	spec.SetSetByCallerMagnitude(effects::HealthName, -DamagePerSecond * Period);
	spec.Period = Period;

	return spec;
}

void UDodgeRoot::ApplyStun(const TArray<ABaseCharacter*>& characters, FPredictionKey key) {
	auto abilitySystem = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent();
	auto vineEffectSpec = effects::CreateGameplayEffectSpec<UDodgeRootVisualGameplayEffect>(abilitySystem);
	vineEffectSpec.GetContext().AddInstigator(GetOwner(), GetOwner());
	vineEffectSpec.SetSetByCallerMagnitude(effects::DurationName, RootDelay + RootDuration);

	if (characters.Num() > 0) {
		FParameterFilterContextWindow filterWindow(EGameplayCueParametersField::Instigator);

		for (auto character : characters) {
			auto targetAbilitySystem = character->GetAbilitySystemComponent();
			abilitySystem->ApplyGameplayEffectSpecToTarget(vineEffectSpec, targetAbilitySystem, key);
		}

		if (GetOwnerRole() == ROLE_Authority) {
			TArray<FGameplayEffectSpec> specs;
			specs.Reserve(2);

			specs.Emplace(CreatePoisonSpec());
			specs.Emplace(CreateRootSpec());
			
			FTimerHandle handle;
			auto weakCharacters = algo::map_tarray(characters, RETLAMBDA(TWeakObjectPtr<ABaseCharacter>(it)));
			GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UDodgeRoot::ApplyStunDeferred, specs, weakCharacters), RootDelay, false);
		}
	}
	else {
		FParameterFilterContextWindow filterWindow(EGameplayCueParametersField::Location);
		Swap(abilitySystem->ScopedPredictionKey, key);
		FGameplayCueParameters params;
		params.Location = GetOwner()->GetActorLocation();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.ArmorProperty.DodgeRoot.Fizzle")), params);
		Swap(abilitySystem->ScopedPredictionKey, key);
	}
}

void UDodgeRoot::ApplyStunDeferred(TArray<FGameplayEffectSpec> specs, TArray<TWeakObjectPtr<ABaseCharacter>> characters) {
	for (auto character : characters) {
		if (character.IsValid() && character->IsAlive()) {
			auto targetAbilitySystem = character->GetAbilitySystemComponent();
			for (const auto spec : specs) {
				targetAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}
		}
	}
}

UDodgeRootVisualGameplayEffect::UDodgeRootVisualGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArmorProperty.DodgeRoot"), 0, 1);
}


UDodgeRootGameplayEffect::UDodgeRootGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;


	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	//Root
	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	info.ModifierMagnitude = FScalableFloat(0.f);
	info.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(info);
}

UDodgeRootPoisonGameplayEffect::UDodgeRootPoisonGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = effects::DurationName;
	DurationMagnitude = durationMagnitude;
	Period = 0.5f;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	//Poison Damage
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	//Damage Tags
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));

	//Damage Numbers
	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Poisoned"), 0, 1);
}