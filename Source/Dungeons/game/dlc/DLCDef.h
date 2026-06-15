#pragma once
#include "DLCName.h"
#include "game/realms/Realms.h"
#include "locale/LocalizationUtils.h"

class APlayerCharacter;
class UTexture2D;

struct DLCDef {
	DLCDef(EDLCName dlcName);

	EDLCName dlc() const {
		return mDlc;
	}

	bool IsDisabled() const;
	bool IsReleased() const;
	bool IsUnlockedByEntitlements(const UWorld* world) const;
	const FText& GetName() const;
	const TArray<FString>& GetUnlockedByEntitlements() const;
	TOptional<FString> GetStorePageURL() const;

	const FText& GetUpsellTitleText() const;
	const FText& GetUpsellDescriptionText() const;
	const TArray<FText>& GetUpsellBulletPoints() const;
	TOptional<ERealmName> GetRequiredRealm() const;


	
	TOptional<FSoftObjectPath> GetInspectorTexturePath() const;
	TOptional<FSoftObjectPath> GetLogoTexturePath() const;
	UTexture2D* GetInspectorTexture() const;
	UTexture2D* GetLogoTexture() const;

	bool DoesLocaleRequireLogoTranslation(DungeonsLocale locale) const;

protected:
	bool bDisabled = false;
	bool bReleased = false;
	EDLCName mDlc;
	FText mName;	
	TOptional<ERealmName> mRequiredRealm;
	TArray<FString> mUnlockedByEntitlements;
	TOptional<FString> mStorePageURL;

	FText mUpsellTitleText;	
	FText mUpsellDescriptionText;
	TArray<FText> mUpsellBulletPoints;

	TOptional<FSoftObjectPath> mInspectTexturePath;
	TOptional<FSoftObjectPath> mLogoTexturePath;

	TSet<DungeonsLocale> mLocalesRequiringLogoTranslation;
};
