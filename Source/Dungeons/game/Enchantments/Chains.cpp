// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "util/CollectionUtils.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Chains.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "world/entity/MobTags.h"

namespace {
	FName ChainsEffectDuration(TEXT("ChainsEffectDuration"));
	FName ChainsEffectMagnitude(TEXT("ChainsEffectMagnitude"));
}

UChains::UChains() {
	TypeId = EEnchantmentTypeID::Chains;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return BaseChainDuration * level;
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
}


FText UChains::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentageChance(TriggerChance)));
}

void UChains::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	bCanTriggerChains = (randStream.FRand() < TriggerChance) || bAlwaysTrigger;
}

void UChains::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) {
	if (bCanTriggerChains) {
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Chains"));
	}
}

void UChains::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	if (bCanTriggerChains) {
		const auto target = Cast<ABaseCharacter>(toWhat);
		if (target && GetOwnerRole() == ROLE_Authority) {
			if (UChains::CanBeChained(target)) {

				auto nearbyChainableMobs = actorquery::getNearbyActors<ABaseCharacter>(target, ChainRange).FilterByPredicate([&](const ABaseCharacter* v) { return CanBeChained(v); });
				nearbyChainableMobs.Sort([&target](const ABaseCharacter& a, const ABaseCharacter& b) { return target->GetDistanceTo(&a) < target->GetDistanceTo(&b); });
				TArray<TWeakObjectPtr<ABaseCharacter>> targetsToChain = Util::map(RETLAMBDA(TWeakObjectPtr<ABaseCharacter>(it)),nearbyChainableMobs, 0, BaseMobChainAmount + (Level - 1));


				if (targetsToChain.Num()) {
					targetsToChain.Emplace(target);

					SpawnChain(GetCharacterOwner(), targetsToChain, ChainDelayTime);
					BroadcastEnchantmentTriggeredEvent();
				}
			}
		}

		bCanTriggerChains = false;
	}
}

void UChains::SpawnChain(TWeakObjectPtr<ABaseCharacter> insitigator, TArray<TWeakObjectPtr<ABaseCharacter>> MobsToChain, float delay) {
	auto playerOwner = Cast<ABaseCharacter>(GetOwner());

	check(playerOwner && "Must have base character as owner")

	auto abilitySystem = playerOwner->GetAbilitySystemComponent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Instigator);
	do {
		auto target = MobsToChain.Pop();
		if (!target.IsValid() || !insitigator.IsValid() || !CanBeChained(target.Get())) {
			break;
		}

		FGameplayEffectSpec spec(Cast<const UGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude(effects::DurationName, IsOwnerMob() ? MobChainDuration : LevelMultiplier(Level));
		spec.GetContext().AddInstigator(insitigator.Get() == GetCharacterOwner() ? nullptr : insitigator.Get(), playerOwner);
		FActiveGameplayEffectHandle effectHandle = abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent());
		
		if (!effectHandle.IsValid()) {
			break;
		}

		insitigator = target;
	} while (MobsToChain.Num() && delay <= 0.f);

	if (delay > 0 && MobsToChain.Num()) {
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UChains::SpawnChain, insitigator, MobsToChain, delay), delay, false);
	}
}


bool UChains::CanBeChained(const ABaseCharacter* target) {
	bool isUnchainable = false;
	if (const auto mobTarget = Cast<AMobCharacter>(target))
	{
		isUnchainable = hasMobTag(mobTarget->EntityType, MobTags::HashTag_Unchainable);
	}
	return !GetCharacterOwner()->IsFriendlyTowards(target) && characterquery::is::targetable(target) && actorquery::is::alive(target)
		&& !isUnchainable
		&& !target->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.Chain")) 
		&& !target->IsActorBeingDestroyed();
}



UChainsGameplayEffect::UChainsGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	info.ModifierMagnitude = FScalableFloat(0.f);
	info.ModifierOp = EGameplayModOp::Type::Override;

	Modifiers.Add(info);

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Chain"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Chain"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Chain"), 0, 1);
}