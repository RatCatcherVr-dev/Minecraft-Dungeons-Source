#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	void EvokerFangLine(bt::StateRef, USummonFormationTask::AddEntryRef);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeEvokerFangCircle(const Provider<int>& countProvider, const Provider<float>& radiusProvider);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeEvokerVexAround(int count, float radius);

}}