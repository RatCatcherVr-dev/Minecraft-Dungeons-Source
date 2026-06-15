#pragma once

#include "game/ai/behavior/BehaviorTuple.h"

class UBehaviorOptionsComponent;
class AMobCharacter;

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createRavager(class AMobCharacter&, const UBehaviorOptionsComponent&);

} } }