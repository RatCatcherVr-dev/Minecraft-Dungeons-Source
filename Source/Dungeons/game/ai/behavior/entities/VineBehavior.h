#pragma once

// D11.DB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createVine(class AMobCharacter&, const UBehaviorOptionsComponent&);
BehaviorTuple createVineSimple(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
