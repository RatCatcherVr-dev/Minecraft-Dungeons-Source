#include "Dungeons.h"
#include "Locations.h"
#include "AITypes.h"
#include "GameFramework/Actor.h"
#include "game/ai/bt/BtEvalState.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/Locator.h"

namespace bt { namespace location {

FVector AnchorGS::operator()(StateRef state) const {
	return state.params().anchor;
}

void AnchorGS::operator()(StateRef state, FVector location) const {
	state.params().anchor = location;
}

Provider Actor(const bt::Provider<AActor*>& provider) {
	return [provider](StateRef state) { AActor* actor = provider(state); return actor ? actor->GetActorLocation() : FAISystem::InvalidLocation; };
}

const Provider& StartPos() {
	static Provider provider = [](StateRef state) { return state.params().startPos; };
	return provider;
}

const Provider& Self() {
	static Provider provider = [](StateRef state) { return state.owner->GetActorLocation(); };
	return provider;
}

const AnchorGS& Anchor() {
	static AnchorGS anchorGS;
	return anchorGS;
}

const Provider From( const locator::Provider& provider )
{
	return [provider](StateRef state) {
		return provider(state).location;
	};
}

const Provider From(const move::Provider& provider) {
	return [=](StateRef state) -> FVector {
		auto request = provider(state);
		if (!request.IsValid()) {
			return {};
		}
		if (request.IsMoveToActorRequest()) {
			if( auto actor = request.GetGoalActor() ) {
				return actor->GetActorLocation();
			}
			else {
				return {};
			}
		}
		return { request.GetDestination() };
	};
}

}}
