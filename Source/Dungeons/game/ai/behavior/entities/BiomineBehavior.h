#pragma once

#include "game/ai/behavior/BehaviorTuple.h"

class UBehaviorOptionsComponent;
class AMobCharacter;

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createBiomine(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
