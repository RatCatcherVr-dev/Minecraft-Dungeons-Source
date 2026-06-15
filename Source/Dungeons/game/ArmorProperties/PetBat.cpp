// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PetBat.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/GameBP.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/mobspawn/alpha/AlphaSpawner.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobAction.h"
#include "game/util/LocationQuery.h"
#include "world/entity/EntityTypes.h"
#include <AbilitySystemComponent.h>

namespace {
	FName PetDealDamageEffectMagnitude(TEXT("PetDealDamageEffectMagnitude"));	
}

UPetBat::UPetBat() {
	TypeID = EArmorPropertyID::PetBat;
	PrimaryComponentTick.bCanEverTick = true;
}

void UPetBat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	if (!Bat.IsValid()) {
		if (TrySpawnBat()) {
			SetComponentTickEnabled(false);
		}
	}
}

void UPetBat::EndPlay(const EEndPlayReason::Type reason) {
	if (GetOwner()->HasAuthority()) {
		if (Bat.IsValid()) {
			Bat->Destroy();
			Bat = nullptr;
		}
	}
}

void UPetBat::OnMobSpawned(AMobCharacter* mob) {
	if(mob) {
		UAbilitySystemComponent* abilitySystem = GetAbilitySystemComponent();

		{
			FScopedPredictionWindow tmp(abilitySystem, FPredictionKey(), false);
			FGameplayCueParameters params;
			params.Location = mob->GetActorLocation();

			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.MobSummon.Summon"), params);
		}

		{
			FGameplayEffectSpec spec(Cast<UPetGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), SourceItemPower);

			const float DealDamageItemPowerMultiplier = game::item::power::DamageIncreaseMultiplier(SourceItemPower);

			spec.SetSetByCallerMagnitude(::PetDealDamageEffectMagnitude, DealDamageItemPowerMultiplier);
			abilitySystem->ApplyGameplayEffectSpecToTarget(spec, mob->GetAbilitySystemComponent(), FPredictionKey());
		}
		Bat = mob;
		GetWorld()->GetTimerManager().SetTimer(relocateHandle, this, &UPetBat::Relocate, .5f);
	} else {
		SetComponentTickEnabled(true);
	}
}

void UPetBat::Relocate() {
	if (Bat.IsValid()) {
		Bat->SetActorLocation(Bat->GetMaster()->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

bool UPetBat::TrySpawnBat() {
	const auto owner = GetOwner();

	if (owner->HasAuthority()) {
		auto playerOwner = Cast<APlayerCharacter>(owner);
		if (!playerOwner->IsLoadedInLevel()) {
			return false;
		}

		FHitResult res;
		FCollisionObjectQueryParams queryParams;
		queryParams.AddObjectTypesToQuery(ECC_WorldStatic);

		FVector spawnLocation = owner->GetActorLocation();
		using namespace game::mobspawn;

		spawnAsync(*GetWorld(), EntityType::Bat, providers::Location(spawnLocation), configs::PlayerPets().Action(ChangeMaster(playerOwner)), [weakThis = TWeakObjectPtr<UPetBat>(this)] (auto mob) {
				if(weakThis.IsValid()) {
					weakThis->OnMobSpawned(mob);
				} else if(mob) {
					mob->Destroy();
				}
		}, MobSpawnPriority::ESpawnPriority_High);
	}

	return true;
}

UPetGameplayEffect::UPetGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::PetDealDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}
