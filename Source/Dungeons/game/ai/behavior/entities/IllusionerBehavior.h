#pragma once

// D11.DB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createIllusioner(class AMobCharacter&, const UBehaviorOptionsComponent&, bool clone = false);

}}}
