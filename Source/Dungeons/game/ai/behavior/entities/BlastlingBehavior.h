#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"

namespace bt {namespace behavior { namespace entities {

BehaviorTuple createBlastling(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
