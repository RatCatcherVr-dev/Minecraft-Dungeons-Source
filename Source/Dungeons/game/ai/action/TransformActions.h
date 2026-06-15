#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"

class AActor;

namespace bt { namespace transform {

void lookAt(AActor&, FVector dst, bool yaw, bool pitch, bool roll);
void lookAtYaw(AActor&, FVector dst);

Action LookAtYaw(const locator::Provider&);

}}
