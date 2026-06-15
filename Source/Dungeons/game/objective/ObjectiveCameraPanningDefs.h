#pragma once
#include <WeakObjectPtrTemplates.h>
#include "CommonTypes.h"
#include "ObjectiveCameraPanning.h"

namespace game { namespace objective {

PanningSequence movingPanningSequence(UWorld&, APlayerCharacter&, const std::vector<TransformData>&, float);

}}
