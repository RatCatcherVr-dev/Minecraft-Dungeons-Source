#pragma once

#include "TrialTypes.h"

struct MissionDef;

namespace trial {

TArray<const MissionDef*> allTrials();

void prepareMissionDefs(const TArray<FTrialDef>&);

}
