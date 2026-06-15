#pragma once

#include "LevelVariationDef.h"
#include "locale/LocalizationUtils.h"

namespace missions { namespace variation {
	struct MutableLevelVariationDef : public LevelVariationDef {
		using LevelVariationDef::LevelVariationDef;		
		MutableLevelVariationDef& displayText(FText txt) { mDisplayText = txt; return *this; }
		MutableLevelVariationDef& themedTextTemplate(FText txt) { mThemedTextTemplate = txt; return *this; }
	};
}}
