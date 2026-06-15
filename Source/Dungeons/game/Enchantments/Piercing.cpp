#include "Dungeons.h"
#include "Piercing.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/ImpactActionHandler.h"

UPiercing::UPiercing() {
	TypeId = EEnchantmentTypeID::Piercing;

	LevelMultiplier = [this](int level) -> float {
		return NumArrowsToShootUntilPiercing - (level-1);
	};
	MultiplierFormatter = valueformat::asEveryRoundedOrdinal;
}

void UPiercing::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (algo::any_of(projectile->ImpactActionHandler->GetActions<UExplosionAction>(), RETLAMBDA(it->BlockPiercingAndFuseShot))) return;

	int threshhold = FMath::RoundToInt(LevelMultiplier(Level));

	if (bAlwaysTrigger || threshhold == 0 || currentArrowCount % threshhold == 0) {
		if(GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		projectile->SetDestroyOnHit(false);
	}
}
