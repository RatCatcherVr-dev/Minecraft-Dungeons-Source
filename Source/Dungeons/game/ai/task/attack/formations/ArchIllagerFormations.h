#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	//FVector getCenterLocation(UWorld& world, const FVector& origin);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchLine(int halfCountMin, float spacing, float offsetFront);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchCircle(int countMin);
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeArchSingle();

}}