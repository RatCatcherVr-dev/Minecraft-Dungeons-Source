#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"

class AActor;

namespace bt { namespace common {

Action Exec(std::function<void()>);
Action Exec(std::function<void(StateRef)>);
Action Apply(std::function<void(AActor*)>);
Action Apply(locator::Provider, std::function<void(AActor*)>);

}}
