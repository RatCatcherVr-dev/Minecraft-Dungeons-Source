#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeHorsemenSquare(float radiusBase, float radiusIncrement);

}}