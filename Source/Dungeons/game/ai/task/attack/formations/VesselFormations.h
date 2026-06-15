#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
		
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeVesselRandom(
		int countExtras,		
		bt::Duration delayBetween
	);

}}