#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"
#include "game/component/AncientGuardianComponent.h"

namespace bt { namespace formation {
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> AncientGuardianMineFormation(std::function<TArray<FVector>()>);

}}