#include "Dungeons.h"
#include "TreeActions.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BehaviorComponent.h"

namespace bt { namespace tree {

void removeThis(StateRef state) {
	state.owner->Behavior->Remove(state.root);
}

const Action& RemoveThis() {
	static Action action = [](StateRef state) { removeThis(state); };
	return action;
}

}}
