#pragma once

#include "RealmDef.h"

namespace realms {
	struct MutableRealmDef : public RealmDef {
		using RealmDef::RealmDef;
		MutableRealmDef& name(FText txt) { mName = txt; return *this; }
		MutableRealmDef& travelText(FText txt) { mTravelText = txt; return *this; }
		MutableRealmDef& disabled() { bDisabled = true; return *this; }
		MutableRealmDef& locked() { bLocked = true; return *this; }
		MutableRealmDef& lockedDescriptionText(FText txt) { mLockedDescriptionText = txt; return *this; }
		MutableRealmDef& lockedTravelText(FText txt) { mLockedTravelText = txt; return *this; }		
	};
}
