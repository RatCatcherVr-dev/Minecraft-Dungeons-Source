#include "Dungeons.h"
#include "Ricochet.h"
#include "game/actor/item/Arrow.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/ActorQuery.h"
#include "lovika/LovikaLevelActor.h"
#include "util/Algo.h"
#include "util/CharacterQuery.h"
#include "util/RandomUtil.h"
#include <Engine.h>
#include "GameFramework/ProjectileMovementComponent.h"
#include "game/actor/ImpactActionHandler.h"

URicochet::URicochet() {
	TypeId = EEnchantmentTypeID::Ricochet;
	LevelMultiplier = [this](int level) -> float {
		return 0.2f * level;
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

bool URicochet::RollForTrigger(const FRandomStream& randStream, ABaseProjectile* projectile) const {
	return bAlwaysTrigger || randStream.FRand() < LevelMultiplier(Level);
}

void URicochet::MulticastRetargetArrow_Implementation(uint32 id, ABaseCharacter* newTarget, FVector targetDirection, bool recurse) {
	const auto index = Projectiles.IndexOfByPredicate(RETLAMBDA(it.Key == id));
	if (index == INDEX_NONE) {
		return;
	}

	ABaseProjectile* fromProjectile = Projectiles[index].Value.Get();
	if (!fromProjectile) {
		// I'm a bit worried this array will keep growing! At least this is "free"ish
		Projectiles.RemoveAtSwap(index);
		return;
	}

	fromProjectile->SpawnRecursionCounter++;

	if (newTarget) {
		fromProjectile->RemovePreviouslyHitActor(newTarget);			
		fromProjectile->SetHomingTarget(newTarget);
		targetDirection = newTarget->GetActorLocation() - fromProjectile->GetActorLocation();
	}

	targetDirection.Normalize();
	targetDirection *= fromProjectile->ProjectileMovementComponent->MaxSpeed;
	fromProjectile->ProjectileMovementComponent->Velocity = targetDirection;
	//Make sure we can hit the requested target if we are ping ponging.
		
	fromProjectile->OnOverlapEnded.AddUObject(this, &URicochet::OnProjectileOverlapEnded, !recurse, TWeakObjectPtr<ABaseProjectile>(fromProjectile));

	if(const auto characterOwner = GetCharacterOwner()){
		FGameplayCueParameters params;
		params.Location = fromProjectile->GetActorLocation();
		characterOwner->GetAbilitySystemComponent()->InvokeGameplayCueEvent(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.Ricochet"), EGameplayCueEvent::Executed, params);
	}

	if (!recurse) {
		Projectiles.RemoveAtSwap(index);
	}
}

void URicochet::OnProjectileOverlapEnded(AActor* overlappedActor, AActor* otherActor, bool shouldDestroyOnHit, TWeakObjectPtr<ABaseProjectile> projectile) {
	if (projectile.IsValid()) {
		projectile->SetDestroyOnHit(shouldDestroyOnHit);
		projectile->OnOverlapEnded.RemoveAll(this);
	}
}

void URicochet::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (algo::any_of(projectile->ImpactActionHandler->GetActions<UExplosionAction>(), RETLAMBDA(it->BlockPiercingAndFuseShot))) return;

	if (projectile->SpawnRecursionCounter < static_cast<uint32>(FMath::Max(MaxAllowedRecursionCount,0)) && RollForTrigger(randStream, projectile)) {
		if (projectile->IsA<AArrow>()) {
			projectile->SetDestroyOnHit(false);
			Projectiles.Emplace(currentArrowCount, projectile);
		}
	}
}

void URicochet::OnResetRangedProjectile(ABaseProjectile* fromProjectile) {
	if (fromProjectile->bCanTriggerEnchantments) {
		const auto index = Projectiles.IndexOfByPredicate(RETLAMBDA(it.Value == fromProjectile));
		if (index != INDEX_NONE) {
			Projectiles.RemoveAtSwap(index);
		}
	}
}

void URicochet::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocationm, FRandomStream& randStream) {
	const auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;

	const auto characterOwner = GetCharacterOwner();

	if (const auto entry = Projectiles.FindByPredicate(RETLAMBDA(it.Value == fromProjectile))) {
		BroadcastEnchantmentTriggeredEvent();
		const bool canRecurse = (fromProjectile->SpawnRecursionCounter + 1) < static_cast<uint32>(FMath::Max(MaxAllowedRecursionCount, 0));
		const bool recursion = canRecurse && RollForTrigger(randStream, fromProjectile);
		if (const auto targetMob = GetBestTarget(toWhat, fromProjectile->ProjectileMovementComponent->Velocity)) {
			MulticastRetargetArrow(entry->Key, targetMob, fromProjectile->GetVelocity(), recursion);
		}
		//No suitable target, ricochet in random direction.
		else {
			const FVector velocity = fromProjectile->GetVelocity();
			const FRotator randomAngle(0, FMath::RandRange(-15, 15), 0);
			MulticastRetargetArrow(entry->Key, nullptr, randomAngle.RotateVector(velocity), recursion);
		}
	}
}


ABaseCharacter* URicochet::GetBestTarget(AActor* sourceActor, const FVector& velocity) {
	auto targets = actorquery::getNearbyActors<ABaseCharacter>(sourceActor, 2000.0f);
	const auto isHostile = characterquery::is::hostile(GetCharacterOwner());
	targets = targets.FilterByPredicate([&](const ABaseCharacter* v) { return isHostile(v) && characterquery::is::targetable(v) && actorquery::is::alive(v); });

	const auto furthest = algo::max_element_by(targets, [&](const ABaseCharacter* v) { return sourceActor->GetDistanceTo(v); });
	if (!furthest) {
		return nullptr;
	}
	const float maxDistance = sourceActor->GetDistanceTo(furthest.Get(nullptr));

	const auto best = algo::max_element_by(targets, [&](const ABaseCharacter* v) {
		const FVector direction = v->GetActorLocation() - sourceActor->GetActorLocation();
		const float angleScore = PI - FMath::Acos(direction.CosineAngle2D(velocity));
		const float distanceScore = 1 - direction.Size2D() / maxDistance;
		return angleScore + distanceScore;
	});

	return best.Get(nullptr);
}
