#include "Dungeons.h"
#include "AttackActions.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace bt { namespace attack {

Action OnBeforeCast() {
	static const Action RemoveInvisibilityAction = [](StateRef state) { state.owner->RemoveInvisibility(); };
	return RemoveInvisibilityAction;
}

Action OnAfterCast() {
	return OnBeforeCast();
}

}}
