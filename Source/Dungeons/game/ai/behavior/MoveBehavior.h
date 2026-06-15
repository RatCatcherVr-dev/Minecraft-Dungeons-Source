#pragma once

#include "game/ai/provider/Move.h"
#include "game/ai/action/SpeedActions.h"
#include "CommonTypes.h"
#include "game/ai/task/TurnTask.h"

class UBtNode;
class UBtGroup;
class FloatRange;
class AMobCharacter;

namespace bt { namespace behavior {

Unique<UBtGroup> moveTo(const location::Provider&, const speed::Speed&, bool);
Unique<UBtGroup> moveTo(const move::Provider&, const speed::Speed&);

Unique<UBtGroup> follow(const location::Provider&, const FAIMoveRequest& = bt::move::defaultRequest());
Unique<UBtGroup> follow(const actor::Provider& ,const location::Provider&);

Unique<UBtGroup> chargeTo(const move::Provider&, const speed::Speed&, Duration maxChargeTime, Duration minTimeBetweenCharges);
Unique<UBtGroup> chargeTo(const move::Provider&, const speed::Speed& charge, Duration maxChargeTime, Duration minTimeBetweenCharges, const speed::Speed& move);
Unique<UBtGroup> chargeTo(const move::Provider&, const speed::Speed& charge, Duration maxChargeTime, Duration minTimeBetweenCharges, const speed::Speed& move, float acceptanceRadius);

Unique<UBtGroup> andMoveCloserTo(Unique<UBtNode> task, const move::Provider&, const speed::Speed&, float acceptanceRadius);

Unique<UBtGroup> chase(const move::Provider&, const FloatRange& ifWithinDistance, const speed::Speed&);
Unique<UBtGroup> chaseInOffensiveRange(const AMobCharacter&, const move::Provider&, const speed::Speed&);
Unique<UBtGroup> chaseIfAttacked(const AMobCharacter& mob, const speed::Speed& speed);
Unique<UBtGroup> chaseIfAttackedForDuration(const AMobCharacter& mob, const speed::Speed& speed, const bt::Seconds& duration);
Unique<UBtGroup> chaseIfWarned(const AMobCharacter& mob, const speed::Speed& speed);
Unique<UBtGroup> chaseIfWarnedForDuration(const AMobCharacter& mob, const speed::Speed& speed, const bt::Seconds& duration);

template <typename ...Tasks>
Unique<UBtGroup> facing(Tasks&&... tasks) {
	return parallel(
		make_unique<UTurnTask>(actor::Target(), 1000.f),
		std::forward<Tasks>(tasks)...
	);
}

template <typename ...Tasks>
Unique<UBtGroup> facing(float turnRate, Tasks&&... tasks) {
	return parallel(
		make_unique<UTurnTask>(actor::Target(), turnRate),
		std::forward<Tasks>(tasks)...
	);
}


}}
