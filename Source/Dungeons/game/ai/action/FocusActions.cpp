#include "Dungeons.h"
#include "FocusActions.h"
#include "game/actor/character/mob/MobBtController.h"

namespace bt { namespace focus {

Action Set(actor::Provider prov, EAIFocusPriority::Type priority/*= EAIFocusPriority::Gameplay*/) {
	// @Location
	return [provider = std::move(prov), priority](StateRef state) { 
		state.controller->SetFocus(provider(state), priority);
	};
}

Action Set(location::Provider prov, EAIFocusPriority::Type priority/*= EAIFocusPriority::Gameplay*/) {
	// @Location
	return [provider = std::move(prov), priority](StateRef state) { 
		state.controller->SetFocalPoint(provider(state), priority);
	};
}

Action Clear(EAIFocusPriority::Type priority/*= EAIFocusPriority::Gameplay*/) {
	return [priority](StateRef state) { 
		state.controller->ClearFocus(priority);
	};
}

}}
