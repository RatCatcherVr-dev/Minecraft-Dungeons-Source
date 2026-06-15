#pragma once

#include "common_header.h"
#include <UnrealString.h>
#include "ArmorPropertyEnumTypes.h"
#include "ArmorProperty.h"
#include "game/difficulty/Difficulty.h"
#include "game/levels.h"

class ArmorPropertyType {

public:
	ArmorPropertyType();
	ArmorPropertyType(EArmorPropertyID id, EArmorPropertyClassification classification);


	EArmorPropertyID getId() const;
	EArmorPropertyClassification getClassification() const;

	const FText& getDisplayName() const;
	const FText getCharacteristicText() const;
	bool hasTag(EArmorPropertyTag tag) const;
	bool isDisabled() const;
	const FName& getRelativeClassPath() const { return mRelativeClassPath; }
	const FName& getRelativeIconPath() const { return mRelativeIconPath; }
	const FName& getRelativeMaterialPath() const { return mRelativeMaterialPath; }
	TSubclassOf<UArmorProperty> GetClass() const;
	bool getRandomGenerationPrevented() const;
	const TSet<ELevelNames>& getRandomGenerationEnabledByLevels() const;
protected:
	EArmorPropertyID ID;

	EArmorPropertyClassification Classification;
	
	FName mRelativeClassPath;
	FName mRelativeIconPath;
	FName mRelativeMaterialPath;
	FText DisplayName;
	FText Characteristic;
	TSet<EArmorPropertyTag> Tags;
	bool bDisabled;
	bool bRandomGenerationPrevented;
	TSet<ELevelNames> mRandomGenerationEnabledByLevels;

	mutable	TSubclassOf<UArmorProperty> mClass;
};