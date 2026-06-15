#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Actors.h"

namespace bt { namespace actorStats {

Provider<float> healthFraction(const actor::Provider& provider = actor::Self());
Pred isTurning(const actor::Provider& provider = actor::Self());

}}
