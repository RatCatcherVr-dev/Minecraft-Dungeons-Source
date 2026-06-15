#include "Dungeons.h"
#include "ReliableRicochet.h"
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

UReliableRicochet::UReliableRicochet() {
	TypeId = EEnchantmentTypeID::ReliableRicochet;
}

bool UReliableRicochet::RollForTrigger(const FRandomStream& randStream) const {
	return bAlwaysTrigger || randStream.FRand() < LevelMultiplier(Level);
}

bool UReliableRicochet::RollForTrigger(const FRandomStream& randStream, ABaseProjectile* projectile) const
{
	return bAlwaysTrigger || projectile->IsCharged;
}