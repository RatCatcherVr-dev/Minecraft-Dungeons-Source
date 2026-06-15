#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
class UBehaviorOptionsComponent;

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createDrownedNecromancer(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
