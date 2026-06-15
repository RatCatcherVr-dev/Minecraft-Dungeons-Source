#pragma once

#include "game/ai/provider/Actors.h"

class UBtNode;
class AMobCharacter;

namespace bt { namespace behavior {

Unique<UBtGroup> aoeAttack(AMobCharacter&, FName ComponentTag = "");
Unique<UBtGroup> aoeAttackInOrientation(const ABaseCharacter&, const actor::Provider& = actor::Target(), int maxAttacksToIssue = 1);

}}
