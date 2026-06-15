#include "Dungeons.h"
#include "ArmorProperty.h"
#include "Assets/ArmorPropertyAssetFinder.h"
#include "util/ClassUtil.h"
#include "ArmorPropertyType.h"

ArmorPropertyType::ArmorPropertyType() : mClass(nullptr) {}

ArmorPropertyType::ArmorPropertyType(EArmorPropertyID id, EArmorPropertyClassification classification)
	: ID(id), 
	Classification(classification),	
	bDisabled(false),
	bRandomGenerationPrevented(false),
	mClass(nullptr) {
}

EArmorPropertyID ArmorPropertyType::getId() const {
	return ID;
}

EArmorPropertyClassification ArmorPropertyType::getClassification() const {
	return Classification;
}

 TSubclassOf<UArmorProperty> ArmorPropertyType::GetClass() const {
	if (!mClass) {
		mClass = classutil::LoadClassAsRoot<UArmorProperty>(IDungeonsModule::Get().GetArmorPropertyAssetFinder()->GetClassPath(ID).Get(FSoftObjectPath()).ToString());
		if (!mClass) {
			UE_LOG(LogDungeons, Warning, TEXT("Armor property %s has no blueprint class, defaulting back to UArmorProperty."), *DisplayName.ToString());
			UE_LOG(LogDungeons, Warning, TEXT("If you are creating a new armor property and seeing this, you will have to restart the egnine after you have created the blueprint."));
			mClass = UArmorProperty::StaticClass();
		}
	}

	return mClass;
}

bool ArmorPropertyType::getRandomGenerationPrevented() const {
	return bRandomGenerationPrevented;
}

const TSet<ELevelNames>& ArmorPropertyType::getRandomGenerationEnabledByLevels() const {
	return mRandomGenerationEnabledByLevels;
}

const FText& ArmorPropertyType::getDisplayName() const {
	return DisplayName;
}


const FText ArmorPropertyType::getCharacteristicText() const {	
	auto armorclass = GetClass();
	auto defaltobj = static_cast<UArmorProperty*>(armorclass->GetDefaultObject());
	auto displayValue = defaltobj->createFormattedDisplayValueString();
	return FText::Format(Characteristic, displayValue);
}

bool ArmorPropertyType::hasTag(EArmorPropertyTag tag) const {
	return Tags.Contains(tag);
}

bool ArmorPropertyType::isDisabled() const {
	return bDisabled;
}
