// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Supercharge.h"
#include "game/actor/item/BaseProjectile.h"


USupercharge::USupercharge() {
	TypeId = EEnchantmentTypeID::Supercharge;
	ServerOnlyExecution = true;
	LevelMultiplier = [](int level) -> float {
		return  1.1f + (0.1f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void USupercharge::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwner()->HasAuthority()) {
		if (projectile->IsCharged) {
			BroadcastEnchantmentTriggeredEvent();
			projectile->MultiplyDamageFactor(LevelMultiplier(Level));
			projectile->pushback.pushbackStrength *= LevelMultiplier(Level);
		}
	}
}
