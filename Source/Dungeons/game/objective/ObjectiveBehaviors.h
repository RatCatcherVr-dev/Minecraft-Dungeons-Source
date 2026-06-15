#pragma once

#include "game/ai/provider/Locators.h"
#include "game/ai/action/SpeedActions.h"

class AMobCharacter;

namespace game { namespace objective { namespace behavior {

Unique<UBtNode> moveToClosestPlayerThenPop(const bt::speed::Speed&, float popDistance);
Unique<UBtNode> moveToThenPop(const bt::locator::Provider&, const bt::speed::Speed&, float popDistance);

}}}
