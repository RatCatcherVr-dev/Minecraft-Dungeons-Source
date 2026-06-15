/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "Tick.h"
//D11.PS changed to {} for console compile conflict
const Tick Tick::MAX{ std::numeric_limits<decltype(Tick::tickID)>::max() };
