#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererWall(EntityType);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererPoison(EntityType);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeWhispererWeb(EntityType);

}}
