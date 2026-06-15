#pragma once

#include "EndGameContentDef.h"
#include "locale/LocalizationUtils.h"

namespace endgamecontent {
	struct MutableEndGameContentDef : public EndGameContentDef {
		using EndGameContentDef::EndGameContentDef;
		MutableEndGameContentDef& grantedProgressKey(const FString& key) { mGrantedProgressKeys.Add(key); return *this; }
		MutableEndGameContentDef& unlockPredicate(const UnlockPredicate& pred) { mUnlockPred = pred; return *this; }
		MutableEndGameContentDef& unlockRequirementText(const UnlockTextProvider& text) { mUnlockRequirementText = text; return *this; }
	};
}
