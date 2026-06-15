#include "Dungeons.h"
#include "util/Algo.hpp"
#include "util/ClassUtil.h"
#include "SkinDef.h"

USkinDef::USkinDef() {
}

USkinDef* USkinDef::CreateDefinition(const FName& dataName, FSkinEntry* dataEntry) {
	if (dataEntry) {
		return CreateDefinition(dataName, *dataEntry);
	}
	return nullptr;
}

USkinDef* USkinDef::CreateDefinition(const FName& dataName, const FSkinEntry& dataEntry) {
	const auto skinDef = NewObject<USkinDef>();
	skinDef->DataName = dataName;
	skinDef->DataEntry = dataEntry;
	return skinDef;
}

FString USkinDef::GetEntitlementName() const {
	return DataEntry->RequiredEntitlement;
}

UMaterialInstance* USkinDef::GetAvatarMaterial() const {
	return DataEntry->AvatarMaterial.LoadSynchronous();
}

UMaterialInstance* USkinDef::GetPortraitMaterial() const {
	return DataEntry->PortraitMaterial.LoadSynchronous();
}

TSoftObjectPtr<UMaterialInstance> USkinDef::GetAvatarMaterialSoftObject() const {
	return DataEntry->AvatarMaterial;
}

TSoftObjectPtr<UMaterialInstance> USkinDef::GetPortraitMaterialSoftObject() const {
	return DataEntry->PortraitMaterial;
}

int USkinDef::GetOrder() const {
	return DataEntry->Order;
}

bool USkinDef::ShouldConsiderForDefault() const {
	return DataEntry->ConsiderForDefault;
}

bool USkinDef::RequiresEntitlement() const {
	return !GetEntitlementName().IsEmpty();
}

const FName& USkinDef::GetSkinId() const {
	return DataName;
}
