#pragma once

#include "game/ai/provider/Actors.h"

class UBtNode;
class AMobCharacter;

namespace bt { namespace behavior {

Unique<UBtNode> meleeAttack(AMobCharacter&, const actor::Provider& target = actor::Target(), int attackIndex = -1);

}}
