#pragma once

#include "Realms.h"

struct RealmDef {
	RealmDef(ERealmName name);

	ERealmName realm() const { 
		return mRealm;
	}

	bool IsDisabled() const;
	const FText& GetName() const;
	const FText& GetTravelText() const;
	bool IsLocked() const;
	const FText& GetLockedDescriptionText() const;
	const FText& GetLockedTravelText() const;	
	
protected:
	bool bDisabled = false;
	ERealmName mRealm;
	FText mName;
	FText mTravelText;
	bool bLocked = false;
	FText mLockedDescriptionText;
	FText mLockedTravelText;	
};
