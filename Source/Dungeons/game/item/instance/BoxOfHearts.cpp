// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "world/entity/EntityTypes.h"
#include "BoxOfHearts.h"
#include "game/util/ActorQuery.h"
#include "AbilitySystemComponent.h"
#include <GameplayEffect.h>
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <unordered_map>
#include "game/util/Tags.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include <UnrealNetwork.h>
#include "world/entity/MobTags.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "../power/ItemPowerUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace {
	FName BoxOfHeartsDealDamageEffectMagnitude(TEXT("BoxOfHeartsDealDamageEffectMagnitude"));
	FName BoxOfHeartsTakeDamageEffectMagnitude(TEXT("BoxOfHeartsTakeDamageEffectMagnitude"));
	FName BoxOfHeartsSpeedEffectMagnitude(TEXT("BoxOfHeartsSpeedEffectMagnitude"));
}

ABoxOfHearts::ABoxOfHearts() {
	Effect = UBoxOfHeartsGameplayEffect::StaticClass();
	bHasManualCooldownActivation = true;
	bCanFail = true;
	bUsePrediction = false;
	PowerEffects = { UItemPowerAsFlatDamageBoost::StaticClass() };
}


void ABoxOfHearts::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABoxOfHearts, BefriendedMobs);
}

void ABoxOfHearts::Activate(const FPredictionKey& predictionKey) {
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	if (!characterOwner->HasAuthority()) {
		Super::Activate(predictionKey);
		return;
	}

	// find nearby targetable mobs
	auto nearbyMobs = actorquery::getNearbyActors<AMobCharacter>(GetOwner(), CharmRange)
		.FilterByPredicate([&](AMobCharacter* mob) { 
		bool isAlive = false;
		if (const auto* HC = mob->FindComponentByClass<UHealthComponent>()) {
			isAlive = HC->IsAlive();
		}
		return mob->IsTargetable() 
			&& isAlive 
			&& characterOwner->IsHostileTowards(mob)
			&& !mob->IsEnchanted()
			&& !mob->ActorHasTag(tags::inLove)
			&& !hasMobTag(mob, MobTags::HashTag_Summoner) //No necromancers
			&& !hasMobTag(mob, MobTags::HashTag_Caster) //No wraiths, enchanters, geomancers (until we can make them actually attack proper)...
			&& !hasMobTag(mob, MobTags::HashTag_Miniboss) 
			&& !hasMobTag(mob, MobTags::HashTag_Unlovable) 
			&& !hasMobTag(mob, MobTags::HashTag_EventMob);
	});

	nearbyMobs.Sort([characterOwner](const AMobCharacter& a, const AMobCharacter& b) { return characterOwner->GetDistanceTo(&a) < characterOwner->GetDistanceTo(&b); });
	
	const auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	if (nearbyMobs.Num() <= 0) {
		FGameplayCueParameters params;
		params.NormalizedMagnitude = 3.f / ItemPower;
		params.Instigator = characterOwner;		
		params.Location = characterOwner->GetActorLocation();
		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.BoxOfHearts.Fail"), params);

		Cooldown().TriggerCooldown(.5f);
	} else {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec(Cast<UBoxOfHeartsGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);

		const float PrimaryItemPowerMultiplier = 1.f + ((GetPowerEffect()->GetMultiplier(ItemPower) - 1.f) * CharmedDamagePercentageIncreasePerDisplayItemPower);
		spec.SetSetByCallerMagnitude(::BoxOfHeartsDealDamageEffectMagnitude, PrimaryItemPowerMultiplier);
		spec.SetSetByCallerMagnitude(::BoxOfHeartsTakeDamageEffectMagnitude, 1.f / CharmedTakeDamageMultiplier);
		spec.SetSetByCallerMagnitude(::BoxOfHeartsSpeedEffectMagnitude, CharmedSpeedMultiplier);

		for(auto mobToBefriend : nearbyMobs){

			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mobToBefriend->GetAbilitySystemComponent(), predictionKey);

			mobToBefriend->Tags.Add(tags::inLove);
			mobToBefriend->ChangeMaster(characterOwner);

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &ABoxOfHearts::OnKillMob, TWeakObjectPtr<AMobCharacter>(mobToBefriend)), CharmedMobDuration, false);

			BefriendedMobs.Add(mobToBefriend);

			if (BefriendedMobs.Num() >= NumCharmedMobs) {
				break;
			}
		}

		Cooldown().TriggerCooldown(CalculateCooldown());
		ActivationSucceeded(predictionKey);
	}

	Super::Activate(predictionKey);
}

void ABoxOfHearts::EndPlay(EEndPlayReason::Type reason) {
	if(HasAuthority()){
		for (auto mob : BefriendedMobs) {
			if (mob.IsValid() && actorquery::is::alive(mob.Get())) {
				mob->SetLastInjuredBy(nullptr);
				mob->Kill();
			}
		}
	}
}

float ABoxOfHearts::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::DamageBoost:
		return CharmedDamagePercentageIncreasePerDisplayItemPower;
	}
	return -1;
}

void ABoxOfHearts::OnKillMob(TWeakObjectPtr<AMobCharacter> mob) {
	BefriendedMobs.Remove(mob);

	if (mob.IsValid()) {
		if (actorquery::is::alive(mob.Get())) {
			mob->SetLastInjuredBy(nullptr);
			mob->Kill();
		}
	}
}

UBoxOfHeartsGameplayEffect::UBoxOfHeartsGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::BoxOfHeartsDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);

	FGameplayModifierInfo takeDamageInfo;
	takeDamageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat takeDamageMagnitude;
	takeDamageMagnitude.DataName = ::BoxOfHeartsTakeDamageEffectMagnitude;

	takeDamageInfo.ModifierMagnitude = takeDamageMagnitude;
	takeDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(takeDamageInfo);

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::BoxOfHeartsSpeedEffectMagnitude;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);


	FGameplayModifierInfo meleeSpeedInfo;
	meleeSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	FSetByCallerFloat meleeSpeedMagnitude;
	meleeSpeedMagnitude.DataName = ::BoxOfHeartsSpeedEffectMagnitude;

	meleeSpeedInfo.ModifierMagnitude = meleeSpeedMagnitude;
	meleeSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeSpeedInfo);


	FGameplayModifierInfo rangedSpeedInfo;
	rangedSpeedInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();

	FSetByCallerFloat rangedSpeedMagnitude;
	rangedSpeedMagnitude.DataName = ::BoxOfHeartsSpeedEffectMagnitude;

	rangedSpeedInfo.ModifierMagnitude = rangedSpeedMagnitude;
	rangedSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedSpeedInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.BoxOfHearts")), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.BoxOfHearts")), 0, 1);
}