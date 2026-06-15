#pragma once

#include "game/ai/provider/Actors.h"
#include "game/component/BehaviorOptionsComponent.h"

class UBtNode;
class AMobCharacter;

namespace bt { namespace behavior {

Unique<UBtNode> chaseAndMeleeAttack(AMobCharacter&, const UBehaviorOptionsComponent&, const actor::Provider& target = actor::Target());

}}
