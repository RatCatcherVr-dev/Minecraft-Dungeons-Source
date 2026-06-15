#pragma once

// D11.DB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createPoisonVine(class AMobCharacter&, const UBehaviorOptionsComponent&);
BehaviorTuple createPoisonVineSimple(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
