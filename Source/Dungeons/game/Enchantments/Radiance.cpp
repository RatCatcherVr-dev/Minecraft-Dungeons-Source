// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"
#include "Radiance.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;

URadianceGameplayEffect::URadianceGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;	

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Radiance.Heal"), 0, 100);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

void URadianceGameplayEffect::AddHealthModifier(UClass* modDamageCalculationClass) {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = modDamageCalculationClass;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);
}

URadianceRangedGameplayEffect::URadianceRangedGameplayEffect() {
	AddHealthModifier(URangedItemPowerOnlyModHealingCalculation::StaticClass());
}

URadianceMeleeGameplayEffect::URadianceMeleeGameplayEffect() {
	AddHealthModifier(UMeleeItemPowerOnlyModHealingCalculation::StaticClass());
}


URadiance::URadiance() {
	
}


FText URadiance::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(HealChance)));
}

void URadiance::SpawnRadiance(FVector location, float HealingAmount, TSubclassOf<class URadianceGameplayEffect> gameplayEffect) const {
	if (GetOwner()->HasAuthority()) {
		FTransform transform;
		transform.SetLocation(location);
		auto healArea = GetWorld()->SpawnActorDeferred<AHealArea>(HealAreaClass, transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		healArea->SetGameplayEffect(gameplayEffect);
		healArea->SetDuration(HealAreaDuration);
		healArea->SetExpandSizePerSecond(HealAreaExpandSizePerSecond);
		healArea->SetHealAmount(HealingAmount);
		UGameplayStatics::FinishSpawningActor(healArea, transform);
		BroadcastEnchantmentTriggeredEvent();
	}
}

URadianceMelee::URadianceMelee() {
	PredictiveExecution = true;
	TypeId = EEnchantmentTypeID::RadianceMelee;

	LevelMultiplier = [this](int level) -> float {
		return BaseHealing * (1.0f + (0.5f * (level-1)));
	};
	ItemPowerMultiplier = game::item::power::HealingIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void URadianceMelee::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext context) {
	if (IsOwnerMob() || (randStream.FRand() < HealChance) || bAlwaysTrigger) {
		bCanTriggerRadiance = true;
		bHasTriggedCue = false;
	}
}

void URadianceMelee::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext conext) {
	if(bCanTriggerRadiance && !bHasTriggedCue) {
		bHasTriggedCue = true;
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.RadianceMelee.Wave"));
	}
}

void URadianceMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (bCanTriggerRadiance) {
		SpawnRadiance(atLocation, IsOwnerMob() ? MobHealing : LevelMultiplier(Level), URadianceMeleeGameplayEffect::StaticClass());

		bCanTriggerRadiance = false;
	}
}

URadianceRanged::URadianceRanged() {
	TypeId = EEnchantmentTypeID::RadianceRanged;

	LevelMultiplier = [this](int level) -> float {
		return BaseHealing * (1.0f + (0.5f * (level - 1)));
	};
	ItemPowerMultiplier = game::item::power::HealingIncreaseMultiplier;
	MultiplierFormatter = valueformat::asDamage;
}

void URadianceRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (GetOwner()->HasAuthority()) {
		bool isOwnerMob = GetOwner()->IsA(AMobCharacter::StaticClass());
		if (isOwnerMob || (randStream.FRand() < HealChance) || bAlwaysTrigger) {
			SpawnRadiance(atLocation, IsOwnerMob() ? MobHealing : LevelMultiplier(Level), URadianceRangedGameplayEffect::StaticClass());

			auto characterOwner = GetCharacterOwner();
			const auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

			FGameplayCueParameters params;
			params.Location = atLocation;

			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.RadianceRanged.Wave"), params);
		}
	}
}

AHealArea::AHealArea() {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	bReplicates = true;
	//InitialLifeSpan = 1.5f;
}

void AHealArea::SetDuration(float durationSeconds) {
	Duration = durationSeconds;
}

void AHealArea::SetExpandSizePerSecond(float expandSize) {
	ExpandSizePerSecond = expandSize;
}

void AHealArea::SetHealAmount(float healAmount) {
	HealAmount = healAmount;
}

void AHealArea::SetGameplayEffect(TSubclassOf<class URadianceGameplayEffect> gameplayEffect){
	Effect = gameplayEffect;
}

void AHealArea::BeginPlay() {
	Super::BeginPlay();

	SetLifeSpan(Duration);
}

void AHealArea::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	HealCircleRadius += ExpandSizePerSecond * DeltaTime;

	if (CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), GetActorLocation(), HealCircleRadius, 50, FColor::Green, false, 0.1f);
	auto characterOwner = GetCharacterOwner();
	if (characterOwner == nullptr) {
		return;
	}

	UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();

	if (HasAuthority()) {
		auto spec = FGameplayEffectSpec(Effect->GetDefaultObject<UGameplayEffect>(), abilitySystem->MakeEffectContext(), 1.0f);
				
		spec.SetSetByCallerMagnitude(effects::HealthName, HealAmount);
		spec.GetContext().AddInstigator(characterOwner, characterOwner);
		spec.GetContext().AddOrigin(characterOwner->GetActorLocation());
		
		auto canHeal = characterquery::can::heal(characterOwner);

		if (CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), GetActorLocation(), HealCircleRadius, 50, FColor::Green, false, 0.1f);
		auto tagets = actorquery::getNearbyActors<ABaseCharacter>(this, HealCircleRadius).FilterByPredicate([&](const ABaseCharacter* v) { return canHeal(v) && !HealedAllies.Contains(v); });
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		for (auto target : tagets) {
			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent());
			HealedAllies.Emplace(target);
		}
	}
}

ABaseCharacter* AHealArea::GetCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}
