#pragma once

#include "BehaviorTuple.h"

class AMobCharacter;
enum class EntityType : unsigned int;

namespace bt { namespace behavior { namespace internal {

BehaviorTuple wip_createBehaviorForAs(AMobCharacter&, EntityType);

}}}
