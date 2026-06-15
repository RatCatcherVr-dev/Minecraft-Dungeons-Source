#pragma once

// D11.DB

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createLeaper(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
