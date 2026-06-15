#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "GameplayTagContainer.h"

namespace bt { namespace actor {

Pred IsAlive(Provider);
//Pred HasTarget() { return IsAlive(Target()); } FIX-ME: Produces link error

Pred IsRecentlyDamagedFromAttack(Seconds maxSecondsSince);
Pred IsRecentlyDamagedFromAttackType(Seconds maxSecondsSince, FGameplayTag gameplayTag);
Pred IsRecentlyWarned(Seconds maxSecondsSince);
Pred IsStuckInWeb(Provider);
Pred IsSlowed(Provider);
Pred IsVoided(Provider);
Pred IsFreezing(Provider);
Pred IsVisible(Provider);
Pred IsInWind(Provider);
Pred IsActivated(Provider);

Pred IsBeingLookedAt(Provider, float angleMax = HALF_PI);
Pred IsBeingLookedAway(Provider, float angleMax = HALF_PI);
Pred IsInLineOfSight(Provider);
Pred IsInFront(locator::Provider, float angleMax = HALF_PI);
Pred IsInRight(locator::Provider loc);
Pred IsInLeft(locator::Provider loc);
Pred IsNotSelf(Provider);
Pred CanTargetLastAttacker();
Pred CanTargetLastWarning();
Pred IsPlayer(Provider);
Pred IsAllPlayersInFront();
Pred IsAnyPlayersInFront();
Pred IsAnyPlayersInCone(float angleMax = HALF_PI);

Pred HasRecentlyAttacked(const bt::Duration& maxDurationSince);
Pred HasRecentlyTeleported(const bt::Duration& maxDurationSince);
Pred HasRecentlyDefended(const bt::Duration& maxDurationSince);

Pred IsAnyPlayersInRange(float range);

Pred HasTag(const FName& tag);
Pred HasTag(Provider, const FName& tag);

}}
