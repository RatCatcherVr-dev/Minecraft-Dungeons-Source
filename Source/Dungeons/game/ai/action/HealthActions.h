#pragma once

#include "game/component/HealthComponent.h"
#include "game/ai/provider/Actors.h"

namespace bt { namespace health {

Action Kill(actor::Provider, FGameplayTag damageType, float impulseMagnitude = 0.f);
Action Heal(actor::Provider, float factor); // D11.DB

}}
