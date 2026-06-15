#pragma once

#include "game/ai/provider/Locators.h"
#include "game/ai/provider/Actors.h"

class UAttackComponent;

namespace bt { namespace attack {

Pred InAttackRange(const locator::Provider&, UAttackComponent*, bool allowNoTarget = false);
Pred InAttackOrientation(const actor::Provider& target, UAttackComponent* component, bool allowNoTarget = false);
Pred IsAttackInProgress(UAttackComponent* component);
Pred InAttackRangeOrAttacking(const locator::Provider&, UAttackComponent*);

}}
