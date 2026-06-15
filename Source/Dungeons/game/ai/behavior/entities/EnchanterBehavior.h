#pragma once

#include "game/ai/behavior/BehaviorTuple.h"

class UBehaviorOptionsComponent;

namespace bt { namespace behavior { namespace entities {

bool isBuffable(AActor* candidate);
BehaviorTuple createEnchanter(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
