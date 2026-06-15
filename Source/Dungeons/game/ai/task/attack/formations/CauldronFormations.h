#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeCauldronPoison(const TSubclassOf<AActor>& poisonClass);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeCauldronSpawn(int count, float radiusMin, float radiusMax);

}}