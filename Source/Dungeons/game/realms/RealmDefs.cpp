#include "Dungeons.h"
#include "CommonTypes.h"
#include "MutableRealmDef.h"
#include "util/CollectionUtils.h"
#include "RealmDefs.h"
#include "util/Algo.h"

namespace realms {

auto realmDefs = Util::createDefaultedTArrayOfSize<Unique<MutableRealmDef>>(enum_cast(ERealmName::count));

MutableRealmDef& create(ERealmName realmIndex) {
	const int index = enum_cast(realmIndex);
	check(!realmDefs[index]);
	realmDefs[index] = make_unique<MutableRealmDef>(realmIndex);	
	return *realmDefs[index];
}

#define LOCTEXT_NAMESPACE "Realms"

//
// Realm definitions
//

const RealmDef& Invalid = create(ERealmName::Invalid)
	.disabled()
	;

const RealmDef& ArchIllagerRealm = create(ERealmName::ArchIllagerRealm)	
	.name(LOCTEXT("ArchIllagerRealm_name", "Mainland"))
	.travelText(LOCTEXT("ArchIllagerRealm_travelText", "Travel to"))
	;

const RealmDef& IslandsRealm = create(ERealmName::IslandsRealm)
	.name(LOCTEXT("IslandsRealm_name", "Island Realms"))
	.travelText(LOCTEXT("IslandsRealm_travelText", "Travel to"))	
	.lockedTravelText(LOCTEXT("IslandsRealm_lockedTravelText", "Coming soon"))
	.lockedDescriptionText(LOCTEXT("IslandsRealm_lockedDescriptionText", "The portal is currently closed. The islands map and two brand new adventures will be available for purchase soon."))
	;

const RealmDef& OtherDimensions = create(ERealmName::OtherDimensions)
	.name(LOCTEXT("OtherDimensionsRealm_name", "Other Dimensions"))
	.travelText(LOCTEXT("OtherDimensionsRealm_travelText", "Travel to"))
	.lockedTravelText(LOCTEXT("OtherDimensionsRealm_lockedTravelText", "Coming soon"))
	.lockedDescriptionText(LOCTEXT("OtherDimensionsRealm_lockedDescriptionText", "The portal is currently closed. The other dimensions will be available for purchase soon."))
	;

const RealmDef& get(ERealmName realmName) {
	if (auto realm = getChecked(realmName)) {
		return *realm;
	}
	return Invalid;
}

const RealmDef* getChecked(ERealmName realmName) {
	const int index = static_cast<int>(realmName);

	if (index >= 1 && index < realmDefs.Num()) { // >= 1 Means ERealmName::Invalid maps to nullptr
		return realmDefs[index].get();
	}
	return nullptr;
}


const TArray<ERealmName> getAllEnabled() {
	TArray<ERealmName> all;
	for (auto&& mutableRealmDef : realms::realmDefs) {
		RealmDef* realmDef = mutableRealmDef.get();
		if (realmDef && !realmDef->IsDisabled()) {
			all.Add(realmDef->realm());
		}
	}
	return all;
}

bool shouldPackageAssets(ERealmName realm) {
	if (const auto* realmDef = getChecked(realm)) {
		return !realmDef->IsDisabled() && !realmDef->IsLocked();
	}
	return false;
}

}

//
// Blueprint interface
//

TArray<ERealmName> URealmDefs::GetEnabledRealmNames() {
	return realms::getAllEnabled();
}

bool URealmDefs::IsRealmLocked(ERealmName realm) {
	if(auto realmDef = realms::getChecked(realm)){
		return realmDef->IsLocked();
	}
	return true;
}

const FText& URealmDefs::GetRealmNameText(ERealmName realm) {	
	if(auto realmDef = realms::getChecked(realm)){
		return realmDef->GetName();	
	}
	return FText::GetEmpty();
}

const FText& URealmDefs::GetRealmTravelText(ERealmName realm) {
	if(auto realmDef = realms::getChecked(realm)){
		return realmDef->GetTravelText();
	}
	return FText::GetEmpty();
}

const FText& URealmDefs::GetRealmLockedTravelText(ERealmName realm) {
	if(auto realmDef = realms::getChecked(realm)){
		return realmDef->GetLockedTravelText();
	}
	return FText::GetEmpty();
}

const FText& URealmDefs::GetRealmLockedDescriptionText(ERealmName realm) {
	if(auto realmDef = realms::getChecked(realm)){
		return realmDef->GetLockedDescriptionText();
	}
	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE


