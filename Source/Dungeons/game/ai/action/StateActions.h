#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/actor/character/BaseCharacterStates.h"

enum class EAnimLocomotion : uint8;

namespace bt { namespace state {
void setLocomotion(StateRef, EAnimLocomotion);

Action SetLocomotion(EAnimLocomotion);

Action SetAttack(EAttackState);

Action SetEventMobRevealed(bool show);

Action SetCollisionEnabled(bool enabled); // D11.DB
Action SetIsTargetable(bool enabled); // D11.DB
Action SetHealthBarEnabled(bool enabled); // D11.DB

}}
