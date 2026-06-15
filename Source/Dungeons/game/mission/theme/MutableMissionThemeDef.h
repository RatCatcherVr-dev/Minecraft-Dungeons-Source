#pragma once

#include "MissionThemeDef.h"
#include "locale/LocalizationUtils.h"

namespace themes {
	struct MutableMissionThemeDef : public MissionThemeDef {
		using MissionThemeDef::MissionThemeDef;		
		MutableMissionThemeDef& displayText(FText txt) { mDisplayText = txt; return *this; }
		MutableMissionThemeDef& lockedDisplayText(FText txt) { mLockedDisplayText = txt; return *this; }
		MutableMissionThemeDef& lockedDescriptionText(FText txt) { mLockedDescriptionText = txt; return *this; }
		MutableMissionThemeDef& setRowName(FName name) { mRowName = name; return *this; }
	};
}
