#pragma once
#include "game/ai/provider/Locators.h"
#include "game/ai/action/SpeedActions.h"

class UBtGroup;
struct FMobParams;

namespace bt { namespace behavior {

Unique<UBtGroup> defaultFleeFrom(const locator::Provider&, float distance = 1000.f, const speed::Speed& speed = Relative(1));
Unique<UBtGroup> defaultFleeFromAfterNumAttacks(const FMobParams&, const locator::Provider&, const Provider<int>& afterNumAttacks, float distance = 1000.f, const speed::Speed& speed = Relative(1));

}}
