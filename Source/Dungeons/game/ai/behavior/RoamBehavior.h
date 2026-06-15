#pragma once

#include "CommonTypes.h"
#include "game/ai/bt/BtTime.h"
#include "game/ai/action/SpeedActions.h"
#include "game/actor/character/BaseCharacterStates.h"

class UBtGroup;

namespace bt { namespace behavior {

Unique<UBtGroup> defaultRoam(Duration minIntervalSeconds = 3s, const speed::Speed& = speed::Speed(EMovementState::Walking));
Unique<UBtGroup> smoothRoam(Duration minIntervalSeconds = 3s, const speed::Speed& = speed::Speed(EMovementState::Walking));

}}
