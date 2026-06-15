// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Gravity.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UGravity::UGravity() {
	TypeId = EEnchantmentTypeID::Gravity;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	LevelMultiplier = [this](int level) -> float {
		return 0.5f + PullTime * (float)level;
	};
	MultiplierFormatter = valueformat::asDurationSingleDecimalSecond;
}

void UGravity::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto target : TargetsToPull) {
		if (!target.IsValid()) {
			continue;
		}
		FVector pullDirection = PullLocation - target->GetActorLocation();
		FHitResult res;
		target->PullCharacter(pullDirection * (DeltaTime * 5.0f), true, res, ETeleportType::TeleportPhysics);
	}
}

void UGravity::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	if (GetOwnerRole() != ROLE_Authority) return;
	if (fromProjectile->GetNumHits() < 1 && !IsComponentTickEnabled()) {
		StartPull(fromProjectile->GetActorLocation());
	}
}

void UGravity::StartPull(const FVector& location, bool executeCue) {
	if (auto characterOwner = GetCharacterOwner()) {
		
		if(GetOwnerRole() == ENetRole::ROLE_Authority) {
			PullLocation = location;
			TargetsToPull.Empty();
			SetComponentTickEnabled(true);

			auto hostile = characterquery::is::hostile(characterOwner);
			auto targetmobs = actorquery::getNearbyActors<ABaseCharacter>(GetWorld(), location, PullRadius).FilterByPredicate([&](const ABaseCharacter* v) {
				return hostile(v) && characterquery::is::movable(v) && !characterquery::is::boss(v);
			});
			TargetsToPull.Append(targetmobs);

			FTimerHandle handle;
			GetWorld()->GetTimerManager().SetTimer(handle, this, &UGravity::OnStopPull, IsOwnerMob() ? MobPullTime : LevelMultiplier(Level));
			BroadcastEnchantmentTriggeredEvent();
		}

		if (executeCue) {
			UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();

			FGameplayCueParameters params;
			params.Location = location;

			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Gravity"), params);
		}
	}
}

void UGravity::OnStopPull() {
	TargetsToPull.Empty();
	SetComponentTickEnabled(false);
}


UGravityMelee::UGravityMelee() : UGravity() {
	TypeId = EEnchantmentTypeID::GravityMelee;
	PredictiveExecution = true;
}

void UGravityMelee::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGravityMelee::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	// To make sure the base function isn't called here
}

void UGravityMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	if (CanPull) {
		CanPull = false;
		StartPull(atLocation, false);
	}
}
void UGravityMelee::OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	CanPull = true;
}

void UGravityMelee::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) {
	if (CanPull) {
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Gravity"));
	}
}