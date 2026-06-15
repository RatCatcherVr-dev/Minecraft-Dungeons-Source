// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "FuseShot.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/actor/item/ProjectileUtil.h"
#include "game/actor/item/Arrow.h"
#include "game/item/instance/FireworkArrow.h"
#include "game/actor/ImpactActionHandler.h"

UFuseShot::UFuseShot() {
	TypeId = EEnchantmentTypeID::FuseShot;
	LevelMultiplier = [](int level) -> float {
		return 6.0f - static_cast<float>(level);
	};
	MultiplierFormatter = valueformat::asEveryRoundedOrdinal;
}

FText UFuseShot::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(ExplosionDelaySeconds)), FText::FromString(valueformat::asPercentage(BaseDamageExplosionDamageFactor)));
}

bool UFuseShot::ShouldTriggerForCount(uint32 currentArrowCount) const {
	const int ApplyThreshold = FMath::RoundToInt(LevelMultiplier(Level));
	return bAlwaysTrigger || ApplyThreshold == 0 || currentArrowCount % ApplyThreshold == 0;
}

void UFuseShot::OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (auto characterOwner = GetCharacterOwner()) {
		bool allowedProjectile = !projectile->IsA<AArrow>() || !DeniedProjectiles.Contains(projectile->GetClass());
		if (allowedProjectile && ShouldTriggerForCount(currentArrowCount)) {
			auto arrow = Cast<AArrow>(projectile);

			if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();

			arrow->bShouldSpawnArrowProp = false;

			if (algo::any_of(projectile->ImpactActionHandler->GetActions<UExplosionAction>(), RETLAMBDA(it->BlockPiercingAndFuseShot))) return;

			if (UDelayedExplosionAction* action = NewObject<UDelayedExplosionAction>()) {
				action->ExplosionCue = ExplosionTag;
				action->ExplosionDamage *= BaseDamageExplosionDamageFactor;
				action->ExplosionRadius = ExplosionRadius;
				action->ExplosionDelaySeconds = ExplosionDelaySeconds;
				arrow->ImpactActionHandler->AddRuntimeAction(action);
			}
		}
	}
}