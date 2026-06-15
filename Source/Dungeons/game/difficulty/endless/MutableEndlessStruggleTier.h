#pragma once
#include "EndlessStruggleTier.h"

namespace endlesstruggle {
	struct MutableEndlessStruggleTier : public EndlessStruggleTier {
		using EndlessStruggleTier::EndlessStruggleTier;
		MutableEndlessStruggleTier& unlockRequirementsPredicate(StruggleUnlockedPredicate predicate) { mUnlockedPredicate = std::move(predicate); return *this; }
		MutableEndlessStruggleTier& missionInterestPredicate(MissionInterestPredicate predicate) { mInterestPredicate = std::move(predicate); return *this; }
		MutableEndlessStruggleTier& unlockTextProvider(UnlockTextProvider unlockTextProvider) { mUnlockTextProvider = std::move(unlockTextProvider); return *this; }
		MutableEndlessStruggleTier& progressTextProvider(ProgressTextProvider progressTextProvider) { mProgressTextProvider = std::move(progressTextProvider); return *this; }
		MutableEndlessStruggleTier& progressStat(EProgressStat stat) { ProgressStat = stat; return *this; }

		MutableEndlessStruggleTier& unlockedEndlessStruggle(FEndlessStruggle unlockedEndlessStruggle) { mUnlockedEndlessStruggle = std::move(unlockedEndlessStruggle); return *this; }

		MutableEndlessStruggleTier& progressGeneratorProvider(ProgressGeneratorProvider progressGeneratorProvider) {
#if !UE_BUILD_SHIPPING
			mProgressGeneratorProvider = std::move(progressGeneratorProvider);
#endif
			return *this;
		}
	};
};
