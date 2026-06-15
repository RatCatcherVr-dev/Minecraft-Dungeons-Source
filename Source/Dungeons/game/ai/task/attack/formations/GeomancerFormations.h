#pragma once

#include "game/ai/task/attack/SummonFormationTask.h"

namespace bt { namespace formation {
	
	void GeomancerWallLine(bt::StateRef, USummonFormationTask::AddEntryRef);
	void GeomancerWallAround(bt::StateRef, USummonFormationTask::AddEntryRef);
	void GeomancerWallRandom(bt::StateRef, USummonFormationTask::AddEntryRef);
	void GeomancerBombSolo(bt::StateRef, USummonFormationTask::AddEntryRef);
	void GeomancerBombMultiple(bt::StateRef, USummonFormationTask::AddEntryRef);
	void FrostWardenBombSolo(bt::StateRef, USummonFormationTask::AddEntryRef);
	void FrostWardenBombMultiple(bt::StateRef, USummonFormationTask::AddEntryRef);
	void GeomancerAll(bt::StateRef, USummonFormationTask::AddEntryRef);

}}