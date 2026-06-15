#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)>
		JungleAbominationRadial( 
			EntityType entityType,
			int count,
			float radius);

	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)>
		JungleAbominationPlayer( 
			EntityType entityType,
			int count,
			float radius);

}}
