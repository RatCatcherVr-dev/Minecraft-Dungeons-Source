#pragma once

#include "game/ai/behavior/BehaviorTuple.h"

class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createEnchanterMulti(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
