#include "Dungeons.h"
#include "DLCDef.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Engine/World.h>
#include <Engine/Texture2D.h>
#include "online/entitlements/EntitlementsValidator.h"

DLCDef::DLCDef(const EDLCName dlcName)
	: mDlc(dlcName) {
}

bool DLCDef::IsDisabled() const {
	return bDisabled;
}

bool DLCDef::IsReleased() const {
	return bReleased;
}

bool DLCDef::IsUnlockedByEntitlements(const UWorld* world) const {
	if (mUnlockedByEntitlements.Num() > 0) {
		if (const auto* gameInstance = world->GetGameInstance<UDungeonsGameInstance>()) {
			const auto* repository = gameInstance->GetEntitlementsRepository();
			return UEntitlementsValidator::HasAnyEntitlement(repository->GetEntitlements(), mUnlockedByEntitlements);
		}
		return false;
	}
	return true;
}

const FText& DLCDef::GetName() const {
	return mName;
}

const TArray<FString>& DLCDef::GetUnlockedByEntitlements() const {
	return mUnlockedByEntitlements;
}

TOptional<FString> DLCDef::GetStorePageURL() const {
	return mStorePageURL;
}

const FText& DLCDef::GetUpsellTitleText() const {
	return mUpsellTitleText;
}

const FText& DLCDef::GetUpsellDescriptionText() const {
	return mUpsellDescriptionText;
}

const TArray<FText>& DLCDef::GetUpsellBulletPoints() const {
	return mUpsellBulletPoints;
}

TOptional<ERealmName> DLCDef::GetRequiredRealm() const {
	return mRequiredRealm;
}

TOptional<FSoftObjectPath> DLCDef::GetInspectorTexturePath() const {
	return mInspectTexturePath;
}

TOptional<FSoftObjectPath> DLCDef::GetLogoTexturePath() const {
	return mLogoTexturePath;
}

UTexture2D* DLCDef::GetInspectorTexture() const {
	if (auto texturePath = GetInspectorTexturePath()) {		
		return Cast<UTexture2D>(texturePath.GetValue().TryLoad());
	}
	return nullptr;
}


UTexture2D* DLCDef::GetLogoTexture() const {
	if (auto texturePath = GetLogoTexturePath()) {
		return Cast<UTexture2D>(texturePath.GetValue().TryLoad());
	}
	return nullptr;
}

bool DLCDef::DoesLocaleRequireLogoTranslation(DungeonsLocale locale) const {
	return mLocalesRequiringLogoTranslation.Contains(locale);
}

