#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
class UBehaviorOptionsComponent;

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createFrozenZombie(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
