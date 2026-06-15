#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	std::vector<std::pair<FVector, float>> generateIceFormation(uint32_t seed, const FVector& location, int patternIndex, float unitSize, float spreadSpeed);

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeIceRandom(
		const TSubclassOf<AActor>& centerClass,
		const TSubclassOf<AActor>& outerClass,
		int patternIndex,
		float unitSize,
		const bt::Duration& spreadSpeed
	);

}}