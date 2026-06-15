#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createEndermite(class AMobCharacter&, const UBehaviorOptionsComponent&, bool smart = false, bool minion = false);

}}}
