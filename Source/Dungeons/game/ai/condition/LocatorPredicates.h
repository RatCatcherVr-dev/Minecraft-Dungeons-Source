#pragma once

#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Locators.h"

class FloatRange;

namespace bt { namespace locator {

Pred IsInRange(Provider, FloatRange);

}}
