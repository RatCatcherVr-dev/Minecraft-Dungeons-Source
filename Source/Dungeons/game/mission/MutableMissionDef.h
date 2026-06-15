#pragma once

#include "MissionDef.h"
#include "game/dlc/DLCName.h"
#include "game/mission/variation/LevelVariationType.h"
#include "game/component/BlockTriggerComponent.h"

class UMediaSource;

namespace missions {

struct MutableMissionDef : public MissionDef {
	using MissionDef::MissionDef;

	MutableMissionDef& setLevel(ELevelNames levelName) { mLevel = levelName; return *this; }

	MutableMissionDef& tutorial() { mIsTutorial = true; return *this; }
	using  MissionDef::levelFilename;
	MutableMissionDef& levelFilename(Provider<FString> filename) { mLevelFilename = std::move(filename); return *this; }
	MutableMissionDef& disabled() { mIsDisabled = true; return *this; }
	MutableMissionDef& permanentlyDisabled() { mIsPermanentlyDisabled = true; return *this; }
	MutableMissionDef& setEnabled(bool enabled = true) { mIsDisabled = !enabled; return *this; }
	MutableMissionDef& dropsDisabled() { mAreDropsEnabled = false; return *this; }
	MutableMissionDef& salvageDisabled() { mIsSalvageEnabled = false; return *this; }
	MutableMissionDef& hyperMission();
	/* note that it is important to match locTableId with the first arg in the LOCTABLE_FROMFILE_GAME -macro */
	MutableMissionDef& loadLocstringsFromFile(std::string locTableId, std::function<void()> f) { mLocTableId = std::move(locTableId); mLoadMissionLoc = std::move(f); return *this; }

	MutableMissionDef& levelPostProcessConfig(std::function<postprocess::Config()> config) { mLevelPostProcess = std::move(config); return *this; }
	MutableMissionDef& metaScorer(levelgen::score::MetaScorer metaScorer) { mMetaScorer = std::move(metaScorer); return *this; }
	MutableMissionDef& name(FText txt) { mName = txt; return *this; }
	MutableMissionDef& levelVariation(ELevelVariationType var, TOptional<ELevelNames> basedOn) { mLevelVariation = var; if (mLevelVariation != ELevelVariationType::none) { mVariationBasedOnLevel = basedOn; } return *this; }
	MutableMissionDef& storyTitle(FText txt) { mStoryTitle = txt; return *this; }
	MutableMissionDef& setTheme(EMissionTheme theme) { mTheme = theme; return *this; }

	MutableMissionDef& confirmStartTitle(FText txt) { mConfirmStartTitle = txt; return *this; }
	MutableMissionDef& confirmStartWarning(FText txt) { mConfirmStartWarning = txt; return *this; }

	MutableMissionDef& selectOrder(int selectOrderIndex) { mSelectOrder = selectOrderIndex; return *this; }

	MutableMissionDef& addRequiredMission(ELevelNames level) { mRequiredMissions.Add(level); return *this; }
	MutableMissionDef& requiredDLC(EDLCName dlcName) { mRequiredDLC = dlcName; return *this; }

	MutableMissionDef& minimalDifficulty(EGameDifficulty difficulty) { mMinimalDifficulty = difficulty; return *this; }
	MutableMissionDef& maximalDifficulty(EGameDifficulty difficulty) { mMaximalDifficulty = difficulty; return *this; }
	MutableMissionDef& fixedDifficulty(EGameDifficulty difficulty) { minimalDifficulty(difficulty); maximalDifficulty(difficulty); return *this; }

	MutableMissionDef& requiredUnlockedDifficulty(EGameDifficulty difficulty) { mRequiredUnlockedDifficulty = difficulty; return *this; }

	MutableMissionDef& requiredEndGameContent(EEndGameContentType type) { mRequiredEndGameContent = type; return *this; }

	MutableMissionDef& grantsUnlockKey(const UnlockKeyGetter& keyGetter) { mMissionGrantUnlockKeys.Add(keyGetter); return *this; }
	MutableMissionDef& grantsUnlockKeys(const TArray<UnlockKeyGetter>& keyGetters) { mMissionGrantUnlockKeys = keyGetters; return *this; }

	MutableMissionDef& minimalThreatLevel(EThreatLevel threatLevel) { mMinimalThreatLevel = threatLevel; return *this; }
	MutableMissionDef& maximalThreatLevel(EThreatLevel threatLevel) { mMaximalThreatLevel = threatLevel; return *this; }
	MutableMissionDef& fixedThreatLevel(EThreatLevel threatLevel) { minimalThreatLevel(threatLevel); maximalThreatLevel(threatLevel); return *this; }
	MutableMissionDef& noDifficultyOrThreatLevelClamping() { mMinimalThreatLevel.Reset(); mMaximalThreatLevel.Reset(); mMinimalDifficulty.Reset(); mMaximalDifficulty.Reset(); return *this; }

	MutableMissionDef& requiredOfferedItemCount(int minCount) { mRequiredOfferedItemsCount = minCount; return *this; }
	MutableMissionDef& requiredOfferedDisplayItemPower(int minDisplayItemPower) { mRequiredOfferedDisplayItemPower = minDisplayItemPower; return *this; }
	MutableMissionDef& requiredNumAncientMobTypes(int minAncientMobTypes) { mRequiredNumAncientMobTypes = minAncientMobTypes; return *this; }

	MutableMissionDef& infiniteLives() { mIsInfiniteLives = true; return *this; }

	MutableMissionDef& boss() { mHasBoss = true; return *this; }

	MutableMissionDef& allowContinue() { mAllowContinue = true; return *this; }

	MutableMissionDef& hiddenThreatLevelSelector() { mHiddenThreatLevelSelector = true; return *this; }
	MutableMissionDef& unlockedThreatLevelByDefault() { mUnlockedThreatLevelByDefault = true; return *this; }
	MutableMissionDef& setExtraChallenge(EExtraChallenge challenge) { mExtraChallenge = challenge; return *this; }


	MutableMissionDef& storyContents(FText txt) { mStoryContents = txt; return *this; }
	MutableMissionDef& mapColor(FMapColors mapColors) { mMapColors = std::move(mapColors); return *this; }
	MutableMissionDef& affectors(affector::RuleData data) { mAffectors = std::move(data); return *this; }
	MutableMissionDef& rewards(MissionItemChances rewards) { mRewards = std::move(rewards); return *this; }
	MutableMissionDef& overrideRewardCount(int count) { mOverrideRewardItemCount = count; return *this; }
	MutableMissionDef& neverOfInterest() { mIsNeverOfInterest = true; return *this; }

	MutableMissionDef& eventMobs(TArray<EventMobDef> mobs) { mEventMobs = std::move(mobs); return *this; }
	MutableMissionDef& nightMode(ENightModeType nightType) { NightModeType = nightType; return *this; }
	MutableMissionDef& nightName(FText nightNameDef) { NightName = nightNameDef; return *this; }
	MutableMissionDef& nightAttackName(FText nightAttackNameDef) { NightAttackName = nightAttackNameDef; return *this; }

	MutableMissionDef& icyBreath() { mIcyBreath = true; return *this; }

	MutableMissionDef& setNeedsSpecificUnlock(bool NeedsSpecificUnlock, TOptional<ELevelNames> specificUnlockMissionNeededOverride = {}) { bNeedsSpecificUnlock = NeedsSpecificUnlock; mSpecificUnlockMissionNeededOverride = specificUnlockMissionNeededOverride; return *this; }
	MutableMissionDef& specificUnlockFoundInMission(ELevelNames level) { mSpecificUnlockFoundInMission = level; return *this; }
	MutableMissionDef& loadingScreen( FSoftObjectPath loadingScreenPath ) { mLoadingScreenTexturePath = loadingScreenPath; return *this; }

	MutableMissionDef& endVideo(std::function<UMediaSource*()> endVideoFunctor) { mEndVideoFunctor = endVideoFunctor; return *this; }
	MutableMissionDef& endVideoMetadata(EndVideoMetadata metadata) { mEndVideoMetadata = metadata; return *this; }
	MutableMissionDef& showVictoryScreen(bool showVictoryScreen = true) { mShowVictoryScreen = showVictoryScreen; return *this; }

	MutableMissionDef& additionalBlockTriggers(std::function<TArray<TSubclassOf<UBlockTrigger>>()> f) { mBlockTriggers = std::move(f); return *this; }
	MutableMissionDef& overrideBlockTriggers(std::function<TArray<TSubclassOf<UBlockTrigger>>()> f) { mBlockTriggerOverrides = std::move(f); return *this; }

	MutableMissionDef& lavaMaterialOverride(const FString& objectPath) { mLavaMaterialOverride = objectPath; return *this; }
	MutableMissionDef& waterMaterialOverride(const FString& objectPath) { mWaterMaterialOverride = objectPath; return *this; }


	MutableMissionDef& victoryDuration(float duration) { mVictoryDuration = duration; return *this; }
	MutableMissionDef& randomSeedProvider(RandomSeedProvider f) { mRandomSeedProvider = std::move(f); return *this; }
	MutableMissionDef& progressToNextMission(ELevelNames nextMissionName) { mNextProgressLevel = nextMissionName; return *this; }

	MutableMissionDef& startWithElytra() { startWithElytraEquipped = true; return *this; }

	MutableMissionDef& appendAffectors(affector::RuleData data);
};

}
