#include "LevelVariationDefs.h"

#define LOCTEXT_NAMESPACE "Mission"
namespace missions { namespace variation {
	auto variationDefs = LevelVariationDefs(enum_cast(ELevelVariationType::seasonal)+1, ELevelVariationType::Invalid);

	const LevelVariationDefs& defs() {
		return variationDefs;
	}

	MutableLevelVariationDef& create(ELevelVariationType levelvariation) {
		return variationDefs.create(levelvariation);
	}

	const LevelVariationDef& invalid = create(ELevelVariationType::Invalid);
	const LevelVariationDef& none = create(ELevelVariationType::none)
		.themedTextTemplate(LOCTEXT("mission_ThemedTemplate", "{0} Mission"));
	const LevelVariationDef& daily = create(ELevelVariationType::daily)		
		.displayText(LOCTEXT("daily_trial", "Daily Trial"))
		.themedTextTemplate(LOCTEXT("trial_ThemedTemplate", "{0} Trial"));
	const LevelVariationDef& weekly = create(ELevelVariationType::weekly)
		.displayText(LOCTEXT("weekly_trial", "Weekly Trial"))
		.themedTextTemplate(LOCTEXT("trial_ThemedTemplate", "{0} Trial"));
	const LevelVariationDef& seasonal = create(ELevelVariationType::seasonal)
		.displayText(LOCTEXT("seasonal_trial", "Seasonal Trial"))
		.themedTextTemplate(LOCTEXT("trial_ThemedTemplate", "{0} Trial"));
}}
#undef LOCTEXT_NAMESPACE
