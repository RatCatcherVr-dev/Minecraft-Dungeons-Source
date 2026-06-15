// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "BonusShot.h"
#include "util/CharacterQuery.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/component/RangedAttackComponent.h"
#include "game/actor/character/BaseCharacter.h"

UBonusShot::UBonusShot() {
	TypeId = EEnchantmentTypeID::BonusShot;
	ServerOnlyExecution = true;
	LevelMultiplier = [](int level) -> float {
		return  0.03f + (0.07f * (float)level);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

void UBonusShot::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwner()->HasAuthority()) {
		auto characterOwner = GetCharacterOwner();
		if (const auto* secondaryTarget = GetSecondaryTarget(attackTarget, randStream)) {
			BroadcastEnchantmentTriggeredEvent();

			const auto dir = (secondaryTarget->GetActorLocation() - spawnLocation).GetSafeNormal();
			SpawnProjectilesMulticast(ProjectileClass, projectile->WeaponItemPower, projectile->SourceItemPower.Get(-1.f), FTransform(dir.Rotation().Quaternion(), spawnLocation), characterOwner, secondaryTarget, projectile->IsCharged);
		}
	}
}

void UBonusShot::SpawnProjectilesMulticast_Implementation(TSubclassOf<ABaseProjectile> ProjectileClass, float itemPower, float sourceItemPower, FTransform transform, ABaseCharacter * characterOwner, const ABaseCharacter * secondaryTarget, bool isCharged) {
	if (auto rangedAttackComponent = characterOwner->FindComponentByClass<URangedAttackComponent>()) {
		TOptional<float> SourceItemPower;
		if (sourceItemPower > 0.f) {
			SourceItemPower = sourceItemPower;
		}
		if (auto newProjectile = URangedAttackComponent::SpawnProjectileDeferred(ProjectileClass, itemPower, transform, characterOwner, rangedAttackComponent->AttackDefinition(), isCharged, false, SourceItemPower)) {
			newProjectile->MultiplyDamageFactor(IsOwnerMob() ? MobDamageFraction : LevelMultiplier(Level));
			newProjectile->SetHomingTarget(secondaryTarget);

			newProjectile->LaunchProjectile(characterOwner);
			newProjectile->EnableProjectileCollisions();

			if (rangedAttackComponent->pitchMode == EPitchMode::Adjust || rangedAttackComponent->pitchMode == EPitchMode::Random) {
				URangedAttackComponent::TrySetDistanceBasedVelocity(newProjectile, transform.GetLocation(), secondaryTarget->GetActorLocation());
			}

			auto abilitySystem = characterOwner->GetAbilitySystemComponent();
			FGameplayCueParameters params;
			params.EffectCauser = characterOwner;
			params.Location = characterOwner->GetActorLocation();
			abilitySystem->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Enchantment.BonusShot")), EGameplayCueEvent::Executed, params);
		}
	}
}


ABaseCharacter* UBonusShot::GetSecondaryTarget(AActor* firstTarget, const FRandomStream& randStream) const {
	const auto* characterOwner = Cast<ABaseCharacter>(GetOwner());

	TArray<ABaseCharacter*> targetCharacters;
	actorquery::getInstanceTrackedActorsInWedge<ABaseCharacter>(GetWorld(), characterOwner->GetActorLocation(), characterOwner->GetActorForwardVector(), 0.f, 2000.0f, HALF_PI, 200.f, targetCharacters, true);
	
	const auto hostile = characterOwner ? characterquery::is::hostile(characterOwner) : [](const ABaseCharacter* arg) { return true; };

	const auto predicate = [=](const AActor* targetCandidate) {
		if (const auto targetCandidateCharacter = Cast<ABaseCharacter>(targetCandidate)) {
			return hostile(targetCandidateCharacter) &&
				characterquery::is::targetable(targetCandidateCharacter) &&
				actorquery::is::alive(targetCandidateCharacter)
				&& targetCandidateCharacter != firstTarget;
		}

		return false;
	};

	auto filteredTargets = targetCharacters.FilterByPredicate(predicate);
	return filteredTargets.Num() > 0 ? filteredTargets[randStream.RandRange(0, filteredTargets.Num() - 1)] : nullptr;
}
