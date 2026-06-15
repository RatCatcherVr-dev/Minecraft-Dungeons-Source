// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Deflecting.h"
#include <AbilitySystemComponent.h>
#include <GameplayEffect.h>
#include "GameplayEffectExtension.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "../component/RangedAttackComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UDeflecting::UDeflecting() {
	TypeId = EEnchantmentTypeID::Deflecting;

	LevelMultiplier = [this](int level) -> float {
		return 0.15f + 0.1f * level;
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

void UDeflecting::MulticastDeflect_Implementation(TSubclassOf<ABaseProjectile> projectileClass, AActor* target, uint32 oldProjectileId) {
	//If we do not have a valid target, we cannot find the projectile in question.
	if (!target) return;
	auto owner = GetCharacterOwner();

	if (auto rangedAttackComponent = target->FindComponentByClass<URangedAttackComponent>()) {
		FVector mobLocation = target->GetActorLocation();
		FVector spawnLocation = owner->GetActorLocation();
		bool isCharged = false;

		auto oldProjectile = rangedAttackComponent->GetProjectileForSpawnId(oldProjectileId);

		float ItemPower = 1.f;
		TOptional<float> SourceItemPower;

		if (oldProjectile) {
			spawnLocation = oldProjectile->GetActorLocation();
			isCharged = oldProjectile->IsCharged;
			ItemPower = oldProjectile->WeaponItemPower;
			SourceItemPower = oldProjectile->SourceItemPower;
			oldProjectile->ForceExipire();
		}
			
		FVector mobToProjectileDist = mobLocation - spawnLocation;
		

		FTransform transform(mobToProjectileDist.Rotation(), spawnLocation);
		if (auto newProjectile = URangedAttackComponent::SpawnProjectileDeferred(projectileClass, ItemPower, transform, owner, rangedAttackComponent->AttackDefinition(), isCharged, false, SourceItemPower)) {
			newProjectile->LaunchProjectile(owner);

			auto effectSpec = oldProjectile && (oldProjectile->GetClass() == projectileClass) ?
				oldProjectile->GetCachedOwnerGameplayEffectSpec() :
				TOptional<FGameplayEffectSpec>{};

			if (effectSpec) {
				effectSpec->GetContext().AddInstigator(effectSpec->GetEffectContext().GetInstigator(), GetOwner());
				effectSpec->DynamicAssetTags.AddTag(damageTag::damageFriends());
				newProjectile->SetGameplayEffectSpec(effectSpec.GetValue());
			}
			newProjectile->EnableProjectileCollisions();
		}

		//Invoke cue locally to minimize traffic
		auto abilitySystem = owner->GetAbilitySystemComponent();
		FGameplayCueParameters params;
		params.Location = owner->GetActorLocation();
		params.Normal = owner->GetActorLocation() - target->GetActorLocation();
		abilitySystem->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Deflecting"), EGameplayCueEvent::Executed, params);
	}
}

void UDeflecting::OnOverlappedByProjectile(ABaseProjectile* projectile, const FRandomStream& randStream) {
	auto characterOwner = GetCharacterOwner();
	if (characterOwner->Role != ROLE_Authority)
		return;

	if (projectile->GetDamage() <= 0.0f) {
		return;
	}
	if ((randStream.FRand() < LevelMultiplier(Level)) || bAlwaysTrigger) {
		if (auto target = projectile->GetInstigator()) {
			if (auto rangedAttackComponent = target->FindComponentByClass<URangedAttackComponent>()) {
				if (auto id = rangedAttackComponent->GetProjectileId(projectile)) {
					BroadcastEnchantmentTriggeredEvent();
					MulticastDeflect(GetDeflectedProjectileClass(*projectile), target, id.Get(0));
				}
			}
		}
	}
}

void UDeflecting::OnProjectileOverlapEnded(AActor* overlappedActor, AActor* otherActor) {
	if (deflectedProjectile.IsValid()) {
		deflectedProjectile->SetDestroyOnHit(true);
		deflectedProjectile->OnOverlapEnded.Remove(delegateHandle);
	}
}

void UDeflecting::OnStart() {
	if (GetOwner()->HasAuthority()) {
		if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
			FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, UDeflectingGameplayEffect::StaticClass()));
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
		}
	}
}

void UDeflecting::OnEnd() {
	if (GetOwner()->HasAuthority()) {
		if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
			abilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.Deflecting")));
		}
	}
}

TSubclassOf<ABaseProjectile> UDeflecting::GetDeflectedProjectileClass(const ABaseProjectile& projectile) const {
	const TSubclassOf<ABaseProjectile> projectileClass = projectile.GetClass();

	if (const auto* deflectedClass = ProjectileToDeflectedClasses.Find(projectileClass)) {
		return *deflectedClass;
	}
	return projectileClass;
}

UDeflectingGameplayEffect::UDeflectingGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bSuppressStackingCues = true;

	const auto deflectingEffectTag = FGameplayTag::RequestGameplayTag("StatusEffect.Deflecting");
	InheritableOwnedTagsContainer.AddTag(deflectingEffectTag);
	InheritableGameplayEffectTags.AddTag(deflectingEffectTag);
}
