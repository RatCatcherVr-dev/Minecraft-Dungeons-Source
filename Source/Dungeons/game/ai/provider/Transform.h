#pragma once

#include "game/ai/bt/BtTypes.h"
#include "Actors.h"
#include "Locators.h"

namespace bt { namespace transform {

Provider<float> distanceTo(const locator::Provider& = actor::Target());

}}
