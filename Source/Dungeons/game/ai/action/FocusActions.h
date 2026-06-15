#pragma once

#include <AIController.h>
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Actors.h"

namespace EAIFocusPriority {
	const Type Attacking = 3;	
}

namespace bt { namespace focus {

Action Set(actor::Provider, EAIFocusPriority::Type = EAIFocusPriority::Gameplay);
Action Set(location::Provider, EAIFocusPriority::Type = EAIFocusPriority::Gameplay);
Action Clear(EAIFocusPriority::Type = EAIFocusPriority::Gameplay);

}}
