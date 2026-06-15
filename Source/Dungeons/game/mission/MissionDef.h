#pragma once

#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/dlc/DLCName.h"
#include "game/mission/variation/LevelVariationType.h"
#include "hyper/DungeonEligibility.h"
#include "lovika/LevelCommon.h"
#include "lovika/io/IoHyperTypes.h"
#include "lovika/io/LevelFile.h"
#include "lovika/world/level/levelgen/metascore/MetaScoreTypes.h"
#include "lovika/world/level/postprocess/PostProcessTypes.h"
#include "theme/MissionTheme.h"
#include "offerings/MissionOfferings.h"
#include "trial/TrialTypes.h"
#include "util/LazyEval.h"
#include <Array.h>
#include "game/EndGame/EndGameContent.h"
#include "EndVideoDefinitions.h"
#include "DungeonsDefsMinimal.h"
#include "locale/LocTableFromFile.h"

struct FMissionState;
class UTexture2D;
class UMediaSource;
class UBlockTrigger;
struct ItemArchetypeCounts;
struct FEligibleDLC;

namespace postprocess { struct Config; }

struct EventMobDef {
	EntityType getEntityType() const { return mEntityType; }
	float getProbability() const { return mProbability; }
	EGameDifficulty getMinDifficulty() const { return mMinDifficulty; };

protected:
	EventMobDef(EntityType entityType) : mEntityType(entityType) {}
	EntityType mEntityType;
	float mProbability = 1;
	EGameDifficulty mMinDifficulty = EGameDifficulty::Difficulty_1;
};

struct FMapColors {
	FMapColors(FLinearColor);
	FMapColors(FLinearColor, FLinearColor, FLinearColor);
	FLinearColor mBaseColor, mAboveColor, mBelowColor;

	static const FLinearColor DefaultAboveOffset;
	static const FLinearColor DefaultBelowOffset;
};

using UnlockKeyGetter = std::function<FString()>;

struct MissionDef {
	MissionDef(ELevelNames);

	ELevelNames level() const { return mLevel; }
	ELevelNames GetProgressToNextMission() const { return mNextProgressLevel; }
	ELevelVariationType levelVariation() const { return mLevelVariation; }
	EMissionTheme theme() const { return mTheme; }
	TOptional<ELevelNames> variationBasedOnLevel() const { return mVariationBasedOnLevel; }

	bool isTrial() const { 
		return mLevelVariation == ELevelVariationType::daily
			|| mLevelVariation == ELevelVariationType::weekly
			|| mLevelVariation == ELevelVariationType::seasonal;
	}

	FString levelFilename() const;

	int getSelectOrder() const { return mSelectOrder; }

	bool isTutorial() const { return mIsTutorial; }
	bool isDisabled() const { return mIsDisabled || mIsPermanentlyDisabled; }
	bool isPermanentlyDisabled() const { return mIsPermanentlyDisabled; }
	bool isThreatLevelUnlockedByDefault() const { return mUnlockedThreatLevelByDefault; }
	bool isThreatLevelSelectorHidden() const { return mHiddenThreatLevelSelector; }
	bool isSalvageEnabled() const { return mIsSalvageEnabled; }	
	bool areDropsEnabled() const { return mAreDropsEnabled; }	

	bool isHyperMission() const { return mIsHyperMission; }

	
	int countNumAncientMobTypes(const io::HyperDungeonPredicate&) const;
	using DungeonEligibilityFilter = Pred<missions::hajper::DungeonEligibility>;
	TArray<FMissionMobChance> getAncientMobChancesFor(const FEligibleDLC& eligibleDLCs, const ItemArchetypeCounts&, const DungeonEligibilityFilter& filter) const;	

	postprocess::Config levelPostProcessConfig() const;
	const TOptional<levelgen::score::MetaScorer>& metaScorer() const;

	void ensureLoadMissionLoc() const;

	const FText& getNameText() const;
	const FText& getStoryTitleText() const;
	const FText& getStoryContentsText() const;

	const FText& getConfirmStartTitleText() const;
	const FText& getConfirmStartWarningText() const;
	
	const affector::RuleData& affectors() const { return mAffectors; }
	const MissionItemChances& rewards() const { return mRewards; }

	const TOptional<int>& overrideRewardCount() const { return mOverrideRewardItemCount; }

	bool isInfiniteLives() const { return mIsInfiniteLives; }

	TOptional<ELevelNames> getSecretUnlockFoundInMission() const;	
	bool isSecretUnlockFoundInMission(ELevelNames level) const;	
	const TArray<ELevelNames>& getRequiredMissions() const;
	TOptional<EDLCName> getRequiredDLC() const;	
	TOptional<EGameDifficulty> getRequiredUnlockedDifficulty() const;
	TOptional<EGameDifficulty> getMaximalDifficulty() const;
	TOptional<EGameDifficulty> getMinimalDifficulty() const;
	TOptional<EThreatLevel> getMinimalThreatLevel() const;
	TOptional<EThreatLevel> getMaximalThreatLevel() const;
	EExtraChallenge getExtraChallenge() const;
	bool isValidDifficulty(EGameDifficulty) const;
	bool isValidThreatLevel(EThreatLevel) const;
	EGameDifficulty getClampedDifficulty(EGameDifficulty) const;
	EThreatLevel getClampedThreatLevel(EThreatLevel) const;
	TOptional<EEndGameContentType> getRequiredEndGameContent() const;

	const TArray<UnlockKeyGetter>& getMissionGrantUnlockKeys() const;

	TOptional<int> getRequiredOfferedItemCount() const;
	TOptional<int> getRequiredOfferedDisplayItemPower() const;
	TOptional<int> getRequiredNumAncientMobTypes() const;
	bool requiresOfferings() const;

	TOptional<FSoftObjectPath> getLoadingScreenTexturePath() const;
	UTexture2D* getLoadingScreenTexture() const;

	const FString& getLavaOverrideMaterial() const;
	const FString& getWaterOverrideMaterial() const;

	const FMapColors& getMapColors() const;

	TOptional<io::MobGroup> getEventMobGroup() const;

	bool IsIcyBreath() const { return mIcyBreath; }

	bool hasBoss() const { return mHasBoss; }

	bool allowsContinue() const { return mAllowContinue; }

	bool isNeverOfInterest() const { return mIsNeverOfInterest; }

	bool isSecretMission() const { return bNeedsSpecificUnlock; }
	TOptional<ELevelNames> getNeededSecretMissionUnlock() const { return bNeedsSpecificUnlock ? mSpecificUnlockMissionNeededOverride.Get(mLevel) : TOptional<ELevelNames>{}; }
	UMediaSource* getEndVideo() const { return mEndVideoFunctor ? mEndVideoFunctor() : nullptr; }
	TOptional<EndVideoMetadata> getEndVideoMetadata() const { return mEndVideoMetadata;  }

	bool isShowingVictoryScreen() const { return mShowVictoryScreen; }

	TArray<TSubclassOf<UBlockTrigger>> getAdditionalBlockTriggers() const { return mBlockTriggers ? mBlockTriggers() : TArray<TSubclassOf<UBlockTrigger>>(); }
	TArray<TSubclassOf<UBlockTrigger>> getOverrideBlockTriggers() const { return mBlockTriggerOverrides ? mBlockTriggerOverrides() : TArray<TSubclassOf<UBlockTrigger>>(); }

	template <typename T>
	using Provider = std::function<T(const MissionDef& self)>;

	ENightModeType GetNightModeType() const;
	FText GetNightName() const;
	FText GetNightAttackName() const;

	float GetVictoryDuration() const { return mVictoryDuration; }

	using RandomSeedProvider = std::function<RandomSeed(const TOptional<FMissionOfferings>& offerings)>;
	RandomSeed provideRandomSeed(const TOptional<FMissionOfferings> &) const;

	bool ShouldStartWithElytra() const { return startWithElytraEquipped; }

protected:
	ELevelNames mLevel;
	ELevelNames mNextProgressLevel;
	ELevelVariationType mLevelVariation;
	TOptional<ELevelNames> mVariationBasedOnLevel;

	EMissionTheme mTheme = EMissionTheme::Invalid;

	Provider<FString> mLevelFilename;

	int mSelectOrder = 0;

	bool mIsTutorial = false;
	bool mIsDisabled = false;
	bool mIsPermanentlyDisabled = false;
	bool mIsNeverOfInterest = false;

	bool mAreDropsEnabled = true;
	bool mIsSalvageEnabled = true;

	bool mIsHyperMission = false;

	bool bNeedsSpecificUnlock = false;
	TOptional<ELevelNames> mSpecificUnlockMissionNeededOverride;

	bool mIsInfiniteLives = false;
	std::string mLocTableId;
	std::function<void()> mLoadMissionLoc;
	std::function<postprocess::Config()> mLevelPostProcess;
	TOptional<levelgen::score::MetaScorer> mMetaScorer;

	FText mName;
	FText mStoryTitle;
	FText mStoryContents;
	FMapColors mMapColors{ FColor{105, 147, 66, 255} };

	FText mConfirmStartTitle;
	FText mConfirmStartWarning;

	EExtraChallenge mExtraChallenge = EExtraChallenge::NoExtraChallenge;

	TOptional<ELevelNames> mSpecificUnlockFoundInMission;
	TArray<ELevelNames> mRequiredMissions;
	TOptional<EGameDifficulty> mRequiredUnlockedDifficulty;
	TOptional<EGameDifficulty> mMinimalDifficulty;
	TOptional<EGameDifficulty> mMaximalDifficulty;
	TOptional<EThreatLevel> mMinimalThreatLevel;
	TOptional<EThreatLevel> mMaximalThreatLevel;

	TOptional<EEndGameContentType> mRequiredEndGameContent;

	TOptional<EDLCName> mRequiredDLC;

	TOptional<int> mRequiredOfferedDisplayItemPower;
	TOptional<int> mRequiredOfferedItemsCount;
	TOptional<int> mRequiredNumAncientMobTypes;

	bool mHiddenThreatLevelSelector = false;
	bool mUnlockedThreatLevelByDefault = false;
	bool mIcyBreath = false;

	ENightModeType NightModeType = ENightModeType::Normal;
	FText NightName = LocTableFromFile::Get("HUDFeatureUILabels.csv", "NightName_Base");
	FText NightAttackName = FText::GetEmpty();

	bool mHasBoss = false;
	bool mAllowContinue = false;

	TArray<UnlockKeyGetter> mMissionGrantUnlockKeys;

	affector::RuleData mAffectors;
	MissionItemChances mRewards;
	TOptional<int> mOverrideRewardItemCount;
	TArray<EventMobDef> mEventMobs;
	std::function<TArray<TSubclassOf<UBlockTrigger>>()> mBlockTriggers;
	std::function<TArray<TSubclassOf<UBlockTrigger>>()> mBlockTriggerOverrides;

	FString mLavaMaterialOverride;
	FString mWaterMaterialOverride;

	std::function<UMediaSource*()> mEndVideoFunctor;
	TOptional<EndVideoMetadata> mEndVideoMetadata;
	bool mShowVictoryScreen = true;

	TOptional<FSoftObjectPath> mLoadingScreenTexturePath;

	RandomSeedProvider mRandomSeedProvider;

	using LazyHyperDungeons = lazyeval::OutParameter0<std::vector<io::HyperDungeon>>;
	LazyHyperDungeons mHyperDungeons;

	float mVictoryDuration = 5.0f;

	bool startWithElytraEquipped = false;
};
