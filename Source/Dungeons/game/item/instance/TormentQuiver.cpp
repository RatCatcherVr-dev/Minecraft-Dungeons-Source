// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TormentQuiver.h"

#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/SoulComponent.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/util/ComponentUtils.h"

ATormentProjectile::ATormentProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	ConsumesStorableItemId = game::item::type::TormentProjectile.getId();
}

void ATormentProjectile::BeginPlay() {
	VelocityFactor = 1.0f;
	RotationSpeed = -1500.0f;
	RotationPivot = componentutils::GetComponentByTag<USceneComponent>(this, "Pivot");
	CollisonBox = FindComponentByClass<UBoxComponent>();

	Super::BeginPlay();
}

void ATormentProjectile::Tick(float DeltaSecs) {
	Super::Tick(DeltaSecs);

	const float LifeLeft = MaxProjectileLifeTime - CurrentProjectileLifetime;

	if (LifeLeft < LifetimeThresholdSeconds && !bLifeTimeThresholdReached) {
		OnLifetimeThresholdReached();
		bLifeTimeThresholdReached = true;
		if (CollisonBox) {
			CollisonBox->SetGenerateOverlapEvents(false);
		}
		RotationSpeed = -500.0f;
	}

	if (RotationPivot)
	{
		const FRotator NewRot(0.0f, 0.0f, DeltaSecs * RotationSpeed);
		RotationPivot->AddLocalRotation(NewRot);
	}

}

void ATormentProjectile::ResetProjectile()
{
	Super::ResetProjectile();
}

void ATormentProjectile::LaunchProjectile(AActor* pOwner)
{
	bLifeTimeThresholdReached = false;
	if (CollisonBox) {
		CollisonBox->SetGenerateOverlapEvents(true);
	}
	RotationSpeed = -1500.0f;

	Super::LaunchProjectile(pOwner);
}

const float UTormentProjectileGameplayEffect::BASE_HIT_DAMAGE = 50.f;

UTormentProjectileGameplayEffect::UTormentProjectileGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	DurationMagnitude = FScalableFloat(0.5);
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.25f;

	FCustomCalculationBasedFloat damageMagnitude;
	damageMagnitude.CalculationClassMagnitude = URangedDamageModCalculation::StaticClass();
	damageMagnitude.Coefficient = BASE_HIT_DAMAGE;

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = damageMagnitude;
	Modifiers.Add(info);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Soul")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.TormentProjectile"), 0, 1);
}

ATormentQuiver::ATormentQuiver() {
	craftedArrowItemType = game::item::type::TormentProjectile.getId();
	craftedArrowCount = 3;

	PowerEffects = { UItemRangedDamageIncrease::StaticClass() };
}

float ATormentQuiver::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::HighestDamage:
	case EItemStats::LowestDamage:
		return UTormentProjectileGameplayEffect::BASE_HIT_DAMAGE;
	}
	return -1;
}

