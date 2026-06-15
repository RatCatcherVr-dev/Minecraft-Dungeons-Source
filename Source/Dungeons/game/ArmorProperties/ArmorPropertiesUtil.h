#pragma once

#include "ArmorPropertyType.h"

namespace game { namespace armorproperties {

TArray<FArmorPropertyData> generateRandomArmorProperties(int positivePropertiesCount, const TSet<ELevelNames>& unlockedLevels);

}}
