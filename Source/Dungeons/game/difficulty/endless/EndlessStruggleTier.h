#pragma once

#include "game/mission/MissionInterest.h"
#include "game/difficulty/EndlessStruggle.h"
#include "save/CharacterSaveData.h"
#include <Optional.h>

class UMissionProgressComponent;
struct MissionDef;
struct FMissionProgress;

struct EndlessStruggleTier {
	using MissionInterestPredicate = std::function<bool(const UMissionProgressComponent*, const MissionDef&, EMissionInterest)>;
	using StruggleUnlockedPredicate = std::function<bool(const UMissionProgressComponent*)>;
	using UnlockTextProvider = std::function<FText()>;
	using ProgressTextProvider = std::function<FText(const UMissionProgressComponent*)>;

	using ProgressGeneratorProvider = std::function<void(UMissionProgressComponent*)>;

protected:
	MissionInterestPredicate mInterestPredicate;
	StruggleUnlockedPredicate mUnlockedPredicate;
	UnlockTextProvider mUnlockTextProvider;
	ProgressTextProvider mProgressTextProvider;

	FEndlessStruggle mUnlockedEndlessStruggle;

	ProgressGeneratorProvider mProgressGeneratorProvider;

	EProgressStat ProgressStat;
public:
	EProgressStat progressStat() const;
	FEndlessStruggle unlockedEndlessStruggle() const;
	bool IsUnlocked(const UMissionProgressComponent*) const;
	bool IsMissionOfInterest(const UMissionProgressComponent*, const MissionDef&, EMissionInterest) const;
	FText GetUnlockText() const;
	FText GetProgressText(const UMissionProgressComponent*) const;

	//Cheating
	void GenerateRequiredProgress(UMissionProgressComponent*) const;
};
