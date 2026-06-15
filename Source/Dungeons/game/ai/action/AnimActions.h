#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Actors.h"

namespace bt { namespace anim {

	
Action PlayBodySequence(UAnimSequenceBase*, FName slot = FName(TEXT("FullBody")), float blendInTime = 0.f, float blendOutTime = 0.2f, float PlayRate = 1.0f, int LoopCount = 1, float BlendOutTriggerTime = 0.f, float InTimeToStartMontageAt = 0.f);

Action SetLookAtTarget(actor::Provider);

const Action& ClearLookAtTarget();

}}
