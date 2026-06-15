#include "Dungeons.h"
#include "CommonActions.h"
#include <game/actor/character/mob/MobCharacter.h>

namespace bt { namespace common {

Action Exec(std::function<void()> fun) {
	return [fun](StateRef state) {
		fun();
	};
}

Action Exec(std::function<void(StateRef)> fun) {
	return [fun](StateRef state) {
		fun(state);
	};
}

Action Apply(std::function<void(AActor*)> fun) {
	return [fun](StateRef state) {
		fun(state.owner);
	};
}

Action Apply(locator::Provider locatorProvider, std::function<void(AActor*)> fun) {
	return [locator = std::move(locatorProvider), fun](StateRef state) {
		auto actor = locator(state).actor;
		if (actor.IsValid()) {
			fun(actor.Get());
		}
	};
}

}}
