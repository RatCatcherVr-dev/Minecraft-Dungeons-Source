#pragma once

#include "CommonTypes.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/SpeedActions.h"

class UBtGroup;
class ABaseCharacter;

namespace bt { namespace behavior {

Unique<UBtGroup> rangedAttackInOrientation(const ABaseCharacter& character, FName componentTag, const actor::Provider& = actor::Target(), int maxAttacksToIssue = 1);

Unique<UBtGroup> rangedAttackInOrientation(const ABaseCharacter& character, const actor::Provider& = actor::Target(), int maxAttacksToIssue = 1);

Unique<UBtGroup> rangedAttackIfOriented(const ABaseCharacter& character, const actor::Provider& = actor::Target(), int maxAttacksToIssue = 1);

Unique<UBtGroup> rangedAttack(const ABaseCharacter&, float minAttackDistance, float acquireTargetRangeUnits, float loseTargetRangeUnits, const actor::Provider& = actor::Target(), int maxAttacksToIssue = 1);

Unique<UBtGroup> rangedAttackAndMoveCloser(const ABaseCharacter&, float minAttackDistance, float acquireTargetRangeUnits, float loseTargetRangeUnits, float minMoveCloserDistance = -1, const actor::Provider& = actor::Target(), const speed::Speed& = Relative(0.5f), int maxAttacksToIssue = 1);

}}
