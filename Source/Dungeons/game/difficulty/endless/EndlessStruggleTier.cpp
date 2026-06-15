#include "Dungeons.h"
#include "EndlessStruggleTier.h"

EProgressStat EndlessStruggleTier::progressStat() const {
	return ProgressStat;
}

FEndlessStruggle EndlessStruggleTier::unlockedEndlessStruggle() const {
	return mUnlockedEndlessStruggle;
}

bool EndlessStruggleTier::IsUnlocked(const UMissionProgressComponent* progress) const {
	if (mUnlockedPredicate) {
		return mUnlockedPredicate(progress);
	}
	return true;
}

bool EndlessStruggleTier::IsMissionOfInterest(const UMissionProgressComponent* progress, const MissionDef& def, EMissionInterest interestType) const {
	if (mInterestPredicate) {
		return mInterestPredicate(progress, def, interestType);
	}
	return false;
}

FText EndlessStruggleTier::GetUnlockText() const {
	if (mUnlockTextProvider) {
		return mUnlockTextProvider();
	}
	return FText::GetEmpty();
}

FText EndlessStruggleTier::GetProgressText(const UMissionProgressComponent* progress) const {
	if (mProgressTextProvider) {
		return mProgressTextProvider(progress);
	}
	return FText::GetEmpty();
}

void EndlessStruggleTier::GenerateRequiredProgress(UMissionProgressComponent* progress) const {
	if (mProgressGeneratorProvider) {
		mProgressGeneratorProvider(progress);
	}
}
