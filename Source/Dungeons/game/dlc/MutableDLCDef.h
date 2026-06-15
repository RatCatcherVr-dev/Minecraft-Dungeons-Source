#pragma once

#include "DLCDef.h"
#include "locale/LocalizationUtils.h"

namespace dlc {
	struct MutableDLCDef : public DLCDef {
		using DLCDef::DLCDef;
		MutableDLCDef& name(FText txt) { mName = txt; return *this; }
		MutableDLCDef& upsellTitle(FText txt) { mUpsellTitleText = txt; return *this; }
		MutableDLCDef& upsellDescription(FText txt) { mUpsellDescriptionText = txt; return *this; }
		MutableDLCDef& upsellBulletPoints(TArray<FText> bulletpoints) { mUpsellBulletPoints = bulletpoints; return *this; }
		MutableDLCDef& disabled() { bDisabled = true; return *this; }
		MutableDLCDef& released() { bReleased = true; return *this; }
		MutableDLCDef& requiredRealm(ERealmName realm) { mRequiredRealm = realm; return *this; };
		MutableDLCDef& storePageURL(FString url) { mStorePageURL = url; return *this; };
		MutableDLCDef& addUnlockedByEntitlement(FString entitlement) { mUnlockedByEntitlements.Add(entitlement); return *this; };
		MutableDLCDef& inspectorBackground(FSoftObjectPath inspectorBackground) { mInspectTexturePath = inspectorBackground; return *this; }
		MutableDLCDef& inspectorLogo(FSoftObjectPath inspectorLogo) { mLogoTexturePath = inspectorLogo; return *this; }
		MutableDLCDef& localesRequiringLogoTranslation(const TSet<DungeonsLocale>& locales) { mLocalesRequiringLogoTranslation = locales; return *this; }
	};
}
