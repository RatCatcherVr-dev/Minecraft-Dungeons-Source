#pragma once

#include "Region.h"

namespace regionpredicates {

const RegionPredicate& all();
RegionPredicate type(const RegionType&);
RegionPredicate name(const std::string&);
RegionPredicate exactName(const std::string&);
RegionPredicate hasTag(const std::string&);

const RegionPredicate& isCheckPoint();
const RegionPredicate& isRegularSpawn();
const RegionPredicate& isRegularSpawnBlocked();
const RegionPredicate& isPuzzleSpawn();
const RegionPredicate& isArenaSpawn();
const RegionPredicate& isPlayerStart();
const RegionPredicate& isTrigger();
const RegionPredicate& isLoot();
const RegionPredicate& isDeathTrigger();
const RegionPredicate& isObjectiveHACK();
const RegionPredicate& isImmobileMob();
const RegionPredicate& isFixedMob();
const RegionPredicate& isStaticMesh();
const RegionPredicate& isSpawn();
const RegionPredicate& isCollisionCullAbove();
const RegionPredicate& isCollisionCullBelow();
const RegionPredicate& isShadowCullAbove();
const RegionPredicate& isShadowCullBelow();
const RegionPredicate& isQuickCullAbove();
const RegionPredicate& isQuickCullBelow();

}
