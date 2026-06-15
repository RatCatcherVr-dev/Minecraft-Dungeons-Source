#pragma once

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/component/BehaviorOptionsComponent.h"


namespace bt { namespace behavior { namespace entities {

BehaviorTuple createEvoker(class AMobCharacter&, const UBehaviorOptionsComponent& options);

}}}
