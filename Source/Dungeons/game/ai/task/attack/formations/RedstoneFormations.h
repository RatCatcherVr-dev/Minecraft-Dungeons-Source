#pragma once

#include "world/entity/EntityTypes.h"
#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeRedstoneCubeRandom(
		float radiusMin,
		float radiusMax,
		int count,
		EntityType mobType
	);

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeRedstoneDotRandom(
		const TSubclassOf<AActor>& dotClass,
		float radiusMin,
		float radiusMax,
		float dotRadius,
		float dotDelay,
		float sectionRadius,
		float sectionDelay,
		int count
	);

}}