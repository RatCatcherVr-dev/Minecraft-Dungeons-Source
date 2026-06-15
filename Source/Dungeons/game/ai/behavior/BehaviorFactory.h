#pragma once

#include "BehaviorTuple.h"

class UBtNode;
class AMobCharacter;

enum class EntityType : unsigned int;

namespace bt { namespace behavior {

void decorateMob(AMobCharacter& mob);
BehaviorTuple createBehaviorFor(AMobCharacter&);
BehaviorTuple createBehaviorForAs(AMobCharacter&, EntityType);

}}
