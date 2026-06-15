#include "Dungeons.h"
#include "SkinsUtil.h"
#include "SkinsLibrary.h"
#include "DungeonsGameInstance.h"
#include <Themida/Anticheat.hpp>

namespace game { namespace skins {

	UDungeonsGameInstance* getDungeonsGameInstance(const UObject* worldContextObject) {
		return worldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	}

	USkinsLibrary* getSkinsLibrary(const UObject* worldContextObject) {
		if (const auto* gameInstance = getDungeonsGameInstance(worldContextObject)) {
			return gameInstance->GetSkinsLibrary();
		}
		return nullptr;
	}

	USkinDef* getSkinDef(const UObject* worldContextObject, const FName skinId) {
		if (auto* library = getSkinsLibrary(worldContextObject)) {
			return library->GetOrCreateDefinition(skinId);
		}
		return nullptr;
	}
}}

UMaterialInstance* USkinsUtil::GetSkinAvatarMaterialInstance(const UObject* worldContextObject, const FName skinId) {
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {		
		return skinDef->GetAvatarMaterial();
	}
	return nullptr;
}

UMaterialInstance* USkinsUtil::GetSkinPortraitMaterialInstance(const UObject* worldContextObject, const FName skinId) {
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		return skinDef->GetPortraitMaterial();
	}
	return nullptr;
}

TSoftObjectPtr<UMaterialInstance> USkinsUtil::GetSkinAvatarMaterialSoft(const UObject* worldContextObject, FName skinId)
{
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		return skinDef->GetAvatarMaterialSoftObject();
	}
	return nullptr;
}

TSoftObjectPtr<UMaterialInstance> USkinsUtil::GetSkinPortraitMaterialSoft(const UObject* worldContextObject, FName skinId)
{
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		return skinDef->GetPortraitMaterialSoftObject();
	}
	return nullptr;
}

bool USkinsUtil::IsSkinValid(const UObject* worldContextObject, const FName skinId) {
	return game::skins::getSkinDef(worldContextObject, skinId) != nullptr;
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool USkinsUtil::IsEntitlementRequired(const UObject* worldContextObject, const FName skinId) {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		return skinDef->RequiresEntitlement();
	}
	return false;
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool USkinsUtil::IsSkinEntitled(const UObject* worldContextObject, const FName skinId) {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		if (skinDef->RequiresEntitlement()) {
			const auto* repository = game::skins::getDungeonsGameInstance(worldContextObject)->GetEntitlementsRepository();
			return repository->GetEntitlement(skinDef->GetEntitlementName()).IsSet();
		}
		return true;
	}
	return false;
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

ANTICHEAT_NO_OPTIMIZATION_BEGIN
TOptional<FEntitlement> USkinsUtil::FindEntitlement(const UObject* worldContextObject, const TArray<FEntitlement>& entitlements, const FName skinId) {
	ANTICHEAT_OBFUSCATE_BEGIN
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		if (skinDef->RequiresEntitlement()) {
			return UEntitlementsValidator::FindEntitlement(entitlements, skinDef->GetEntitlementName());
		}
		return FEntitlement();
	}
	return TOptional<FEntitlement>();
	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

FColor USkinsUtil::GetPlayerColor(const int playerNumber) {
	if (PlayerIdentityColors.IsValidIndex(playerNumber)) {
		return PlayerIdentityColors[playerNumber];
	}

	UE_LOG(LogDungeons, Warning, TEXT("PlayerNumber %d OOB for PlayerIdentityColors."), playerNumber);
	return PlayerIdentityErrorColor;
}

TArray<FName> USkinsUtil::GetSkinIds(const UObject* worldContextObject) {	
	TArray<FName> skinIds;	
	if (auto library = game::skins::getSkinsLibrary(worldContextObject)) {
		auto definitions = library->GetOrCreateDefinitions();
		definitions.Sort([](const USkinDef& a, const USkinDef& b){
			if (a.GetOrder() == b.GetOrder()) {
				return a.GetSkinId() < b.GetSkinId();
			}
			return a.GetOrder() < b.GetOrder();
		});

		for (auto* skinDef : definitions) {
			skinIds.Add(skinDef->GetSkinId());			
		}
	}
	return skinIds;	
}

TArray<FName> USkinsUtil::GetDefaultSkinIds(const UObject* worldContextObject) {
	TArray<FName> skinIds;
	if (auto library = game::skins::getSkinsLibrary(worldContextObject)) {		
		for (auto* skinDef : library->GetOrCreateDefinitions(RETLAMBDA(it.ShouldConsiderForDefault()))) {
			skinIds.Add(skinDef->GetSkinId());
		}
	}
	return skinIds;
}

bool USkinsUtil::IsEntitlementTamperedWith(const UObject* worldContextObject, const FName skinId, const FEntitlement& entitlement, const FString userId) {
	if (const auto* skinDef = game::skins::getSkinDef(worldContextObject, skinId)) {
		if (skinDef->RequiresEntitlement()) {
			auto* repository = game::skins::getDungeonsGameInstance(worldContextObject)->GetEntitlementsRepository();
			return repository->IsEntitlementTamperedWith(entitlement, skinDef->GetEntitlementName(), userId);
		}
	}
	return false;
}

