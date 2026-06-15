#include "Dungeons.h"
#include "RealmDef.h"

RealmDef::RealmDef(ERealmName realm)
	: mRealm(realm) {
}

bool RealmDef::IsDisabled() const {
	return bDisabled;
}

const FText& RealmDef::GetName() const {
	return mName;
}

const FText& RealmDef::GetTravelText() const {
	return mTravelText;
}

bool RealmDef::IsLocked() const {
	return bLocked;
}

const FText& RealmDef::GetLockedDescriptionText() const {
	return mLockedDescriptionText;
}

const FText& RealmDef::GetLockedTravelText() const {
	return mLockedTravelText;
}
