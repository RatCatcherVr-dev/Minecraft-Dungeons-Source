#pragma once

#include "game/ai/behavior/BehaviorTuple.h"

class UBehaviorOptionsComponent;
class AMobCharacter;

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createPolarBear(class AMobCharacter&, const UBehaviorOptionsComponent&);

} } }