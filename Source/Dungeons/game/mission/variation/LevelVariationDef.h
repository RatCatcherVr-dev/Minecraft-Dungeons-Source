#pragma once
#include "LevelVariationType.h"
#include <Text.h>

struct LevelVariationDef {
	LevelVariationDef(ELevelVariationType variation);

	ELevelVariationType variation() const {
		return mVariation;
	}

	const TOptional<FText>& displayText() const {
		return mDisplayText;
	}

	const TOptional<FText>& themedTextTemplate() const {
		return mThemedTextTemplate;
	}

protected:
	ELevelVariationType mVariation;	
	TOptional<FText> mDisplayText;
	TOptional<FText> mThemedTextTemplate;
};
