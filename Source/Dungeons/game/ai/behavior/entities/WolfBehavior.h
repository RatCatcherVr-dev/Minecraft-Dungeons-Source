#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createWolf(class AMobCharacter&, const UBehaviorOptionsComponent&, bool usePathFinding=true);

}}}
