#include "Dungeons.h"
#include "MoveBehavior.h"
#include "NodeFactory.h"
#include "game/ai/action/BtAction.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/task/move/MoveTo.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "util/FloatRange.h"

namespace bt { namespace behavior {

Unique<UBtGroup> moveTo(const location::Provider& provider, const speed::Speed& speed, bool continuously) {
	return sequence("move-to",
		make_unique<UMoveTo>(provider, continuously),
		onTick(speed::Set(speed))
	);
}

Unique<UBtGroup> follow(const location::Provider& provider, const FAIMoveRequest& moveRequest) {
	return sequence("move-to",
		make_unique<UMoveTo>(provider, true, moveRequest),
		onTick(speed::SetAbsolute([](StateRef state) { return FMath::Max(state.owner->GetMaster()->GetVelocity().Size2D() * 1.1f, 400.f); }))
	);
}

Unique<UBtGroup> follow(const actor::Provider& targetProvider, const location::Provider& locationProvider) {
	return sequence("move-to",
		make_unique<UMoveTo>(locationProvider, true),
		onTick(speed::SetAbsolute([targetProvider](StateRef state) { 
			const auto* target = targetProvider(state);
			return target ? FMath::Max(target->GetVelocity().Size2D() * 1.1f, 400.f) : 400.f;
		}))
	);
}

Unique<UBtGroup> moveTo(const move::Provider& provider, const speed::Speed& speed) {
	return sequence("move-to",
		make_unique<UMoveTo>(provider),
		onTick(speed::Set(speed))
	);
}

Unique<UBtGroup> chargeTo(const move::Provider& provider, const speed::Speed& speed, Duration maxChargeTime, Duration minTimeBetweenCharges) {
	return sequence("charge-to",
		maxRunTime(maxChargeTime),
		minTimeBetweenStarts(minTimeBetweenCharges),
		behavior::moveTo(provider, speed)
	);
}

Unique<UBtGroup> chargeTo(const move::Provider& provider, const speed::Speed& charge, Duration maxChargeTime, Duration minTimeBetweenCharges, const speed::Speed& move) {
	return selector("charge-or-move-to",
		chargeTo(provider, charge, maxChargeTime, minTimeBetweenCharges),
		moveTo(provider, move)
	);
}

Unique<UBtGroup> chargeTo( const move::Provider& provider, const speed::Speed& charge, Duration maxChargeTime, Duration minTimeBetweenCharges, const speed::Speed& speed, float acceptanceRadius ) {
	return sequence("charge-to",
		maxRunTime(maxChargeTime),
		minTimeBetweenStarts(minTimeBetweenCharges),
		behavior::moveTo(move::withSettings(provider, move::defaultRequest().SetAcceptanceRadius(acceptanceRadius)), speed)
	);
}

Unique<UBtGroup> andMoveCloserTo(Unique<UBtNode> task, const move::Provider& provider, const speed::Speed& speed, float acceptanceRadius) {
	return greedySequence("...and-move-closer",
		std::move(task),
		moveTo(move::withSettings(provider, move::defaultRequest().SetAcceptanceRadius(acceptanceRadius)), speed)
	);
}


Unique<UBtGroup> chase(const move::Provider& provider, const FloatRange& ifWithinDistance, const speed::Speed& speed) {
	const FloatRange chaseDistance{
		ifWithinDistance.min(),
		ifWithinDistance.max() + ifWithinDistance.max() - ifWithinDistance.min()
	};

	return sequence("chase",
		isInRange(locator::From(provider), ifWithinDistance, chaseDistance),
		moveTo(provider, speed)
	);
}


Unique<UBtGroup> chaseInOffensiveRange(const AMobCharacter& mob, const move::Provider& provider, const speed::Speed& speed) {
	return chase(provider, mob.OffensiveRange, speed);
}

Unique<UBtGroup> chaseIfAttacked(const AMobCharacter& mob, const speed::Speed& speed) {
	return chaseIfAttackedForDuration(mob, speed, 10s);
}

Unique<UBtGroup> chaseIfWarned(const AMobCharacter& mob, const speed::Speed& speed) {
	return chaseIfWarnedForDuration(mob, speed, 10s);
}

Unique<UBtGroup> chaseIfAttackedForDuration(const AMobCharacter& mob, const speed::Speed& speed, const bt::Seconds& duration) {
	return sequence(
		predicate(actor::IsRecentlyDamagedFromAttack(duration)),
		predicate(actor::CanTargetLastAttacker()),
		predicate(actor::IsPlayer(actor::LastAttacker())),
		chase(actor::LastAttacker(), 4000, speed)
	);
}

Unique<UBtGroup> chaseIfWarnedForDuration(const AMobCharacter& mob, const speed::Speed& speed, const bt::Seconds& duration) {
	return sequence(
		predicate(actor::IsRecentlyWarned(duration)),
		predicate(actor::CanTargetLastWarning()),
		predicate(actor::IsPlayer(actor::LastWarning())),
		chase(actor::LastWarning(), 4000, speed)
	);
}

}}
