#include "Dungeons.h"
#include "MissionDef.h"
#include "MissionDefs.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "lovika/world/level/postprocess/PostProcessConfigs.h"
#include "world/entity/EntityTypes.h"
#include "StringTableRegistry.h"
#include "util/Algo.hpp"
#include "DungeonsDefsMinimal.h"
#include <IConsoleManager.h>
#include "MissionDLCUtil.h"

static TAutoConsoleVariable<int> CVarForcedSeedVal(TEXT("Dungeons.level.forcedseed"), 0, TEXT("Force a seed value for level"), ECVF_Default);

static bool isStringTableLoaded(const FString& id) {
	FStringTableConstPtr table = FStringTableRegistry::Get().FindStringTable(FName(*id));
	return table.IsValid();
}

MissionDef::MissionDef(ELevelNames level)
	: mLevel(level)
	, mNextProgressLevel(ELevelNames::Invalid)
	, mLevelFilename(RETLAMBDA(GetEnumValueToStringStripped(mLevel)))
	, mLevelPostProcess(RETLAMBDA2(postprocess::configs::OnlyDoors()))
	, mLoadMissionLoc()
	, mRewards(itemgen::configs::MissionInspectorItem())
{
}

FString MissionDef::levelFilename() const {
	return mLevelFilename(*this);
}

postprocess::Config MissionDef::levelPostProcessConfig() const {
	return mLevelPostProcess();
}

const TOptional<levelgen::score::MetaScorer>& MissionDef::metaScorer() const {
	return mMetaScorer;
}

void MissionDef::ensureLoadMissionLoc() const {
	if (mLoadMissionLoc && !isStringTableLoaded(FString(mLocTableId.c_str())))
		mLoadMissionLoc();
}

const FText& MissionDef::getNameText() const {
	return mName;
}

const FText& MissionDef::getStoryTitleText() const {
	return mStoryTitle;
}

const FText& MissionDef::getStoryContentsText() const {
	return mStoryContents;
}

const FText& MissionDef::getConfirmStartTitleText() const {
	return mConfirmStartTitle;
}

const FText& MissionDef::getConfirmStartWarningText() const {
	return mConfirmStartWarning;
}



TOptional<ELevelNames> MissionDef::getSecretUnlockFoundInMission() const {
	return mSpecificUnlockFoundInMission;
}

bool MissionDef::isSecretUnlockFoundInMission(ELevelNames level) const {
	if (mSpecificUnlockFoundInMission.IsSet()) {
		return mSpecificUnlockFoundInMission.GetValue() == level;
	}
	return false;
}

const TArray<ELevelNames>& MissionDef::getRequiredMissions() const
{
	return mRequiredMissions;
}

TOptional<EDLCName> MissionDef::getRequiredDLC() const
{
	return mRequiredDLC;
}

TOptional<EGameDifficulty> MissionDef::getRequiredUnlockedDifficulty() const
{
	return mRequiredUnlockedDifficulty;
}

TOptional<EGameDifficulty> MissionDef::getMaximalDifficulty() const
{
	return mMaximalDifficulty;
}

TOptional<EGameDifficulty> MissionDef::getMinimalDifficulty() const
{
	return mMinimalDifficulty;
}

TOptional<EThreatLevel> MissionDef::getMinimalThreatLevel() const
{
	return mMinimalThreatLevel;
}

TOptional<EThreatLevel> MissionDef::getMaximalThreatLevel() const
{
	return mMaximalThreatLevel;
}

EExtraChallenge MissionDef::getExtraChallenge() const
{
	return mExtraChallenge;
}

EGameDifficulty MissionDef::getClampedDifficulty(EGameDifficulty Difficulty) const
{
	if (mMinimalDifficulty.IsSet() && mMinimalDifficulty.GetValue() > Difficulty) {
		return mMinimalDifficulty.GetValue();
	}
	if (mMaximalDifficulty.IsSet() && mMaximalDifficulty.GetValue() < Difficulty) {
		return mMaximalDifficulty.GetValue();
	}
	return Difficulty;
}

EThreatLevel MissionDef::getClampedThreatLevel(EThreatLevel ThreatLevel) const
{
	if (mMinimalThreatLevel.IsSet() && mMinimalThreatLevel.GetValue() > ThreatLevel) {
		return mMinimalThreatLevel.GetValue();
	}
	if (mMaximalThreatLevel.IsSet() && mMaximalThreatLevel.GetValue() < ThreatLevel) {
		return mMaximalThreatLevel.GetValue();
	}
	return ThreatLevel;
}

TOptional<EEndGameContentType> MissionDef::getRequiredEndGameContent() const {
	return mRequiredEndGameContent;
}

const TArray<UnlockKeyGetter>& MissionDef::getMissionGrantUnlockKeys() const {
	return mMissionGrantUnlockKeys;
}

TOptional<int> MissionDef::getRequiredOfferedItemCount() const { 
	return mRequiredOfferedItemsCount; 
}

TOptional<int> MissionDef::getRequiredOfferedDisplayItemPower() const { 
	return mRequiredOfferedDisplayItemPower; 
}

TOptional<int> MissionDef::getRequiredNumAncientMobTypes() const {
	return mRequiredNumAncientMobTypes;
}

bool MissionDef::requiresOfferings() const {
	return mRequiredOfferedDisplayItemPower.IsSet() || mRequiredOfferedItemsCount.IsSet();
}


bool MissionDef::isValidDifficulty(EGameDifficulty Difficulty) const
{
	return getClampedDifficulty(Difficulty) == Difficulty;
}

bool MissionDef::isValidThreatLevel(EThreatLevel ThreatLevel) const
{
	return getClampedThreatLevel(ThreatLevel) == ThreatLevel;
}


TOptional<FSoftObjectPath> MissionDef::getLoadingScreenTexturePath() const {
	return mLoadingScreenTexturePath;
}

UTexture2D* MissionDef::getLoadingScreenTexture() const {
	if (mLoadingScreenTexturePath.IsSet()) {
		return Cast<UTexture2D>(mLoadingScreenTexturePath.GetValue().TryLoad());
	}	
	return nullptr;
}

const FString& MissionDef::getLavaOverrideMaterial() const {
	return mLavaMaterialOverride;
}

const FString& MissionDef::getWaterOverrideMaterial() const {
	return mWaterMaterialOverride;
}

const FMapColors& MissionDef::getMapColors() const {
	return mMapColors;
}

TOptional<io::MobGroup> MissionDef::getEventMobGroup() const {
	if (mEventMobs.Num() == 0) {
		return {};
	}
	auto group = io::MobGroup({}, "mission-def-generated-group");
	for (auto& it : mEventMobs) {
		io::MobType type(it.getEntityType(), it.getProbability());
		type.allowedOnDifficulties = DifficultyRange(difficultyquery::higher(it.getMinDifficulty(), true));
		group.types.push_back(std::move(type));
	}
	return group;
}

RandomSeed MissionDef::provideRandomSeed(const TOptional<FMissionOfferings>& offerings) const {
	if (int iForcedSeed = CVarForcedSeedVal.GetValueOnGameThread()) {
		return iForcedSeed;
	} else if (mRandomSeedProvider) {
		return mRandomSeedProvider(offerings);
	} else {
		return FMath::Rand();
	}
}

int MissionDef::countNumAncientMobTypes(const io::HyperDungeonPredicate& dungeonPredicate) const {
	int total = 0;
	for (const auto& dungeon : *mHyperDungeons) {
		if (dungeon.hasAncient() && dungeonPredicate(dungeon)) {
			total++;
		}
	}
	return total;	
}

TArray<FMissionMobChance> MissionDef::getAncientMobChancesFor(const FEligibleDLC& eligibleDLCs, const ItemArchetypeCounts& archetypeCounts, const DungeonEligibilityFilter& filter) const {
	using namespace  missions::hajper;

	TArray<FMissionMobChance> output;

	const auto dungeonEligibilityPredicate = game::mission::dlc::getDungeonEligibilityPredicate(eligibleDLCs, archetypeCounts);
	
	for (const auto& dungeon : *mHyperDungeons) {
		TOptional<EDLCName> missingDLC;
		auto eligibility = dungeonEligibilityPredicate(dungeon);
		if (dungeon.hasAncient() && filter(eligibility)) {
			if (eligibility == DungeonEligibility::COULD_PLAY_IF_YOU_HAD_DLC) {
				missingDLC = dungeon.requiredDLC;
			}
			for (auto& entity : dungeon.entityTypes) {
				output.Add(FMissionMobChance{ entity, missingDLC });
			}
		}
	}

	return output;
}

FMapColors::FMapColors(FLinearColor Base) 
	: mBaseColor(Base)
	, mAboveColor(Base + DefaultAboveOffset)
	, mBelowColor(Base - DefaultBelowOffset)
{
}

FMapColors::FMapColors(FLinearColor Base, FLinearColor Above, FLinearColor Below)
	: mBaseColor(Base)
	, mAboveColor(Above)
	, mBelowColor(Below)
{
}

const FLinearColor FMapColors::DefaultAboveOffset{ 0.35f, 0.35f, 0.35f, 1.f };
const FLinearColor FMapColors::DefaultBelowOffset{ 0.25f, 0.25f, 0.25f, 1.f };

ENightModeType MissionDef::GetNightModeType() const
{
	return NightModeType;
}

FText MissionDef::GetNightName() const
{
	return NightName;
}

FText MissionDef::GetNightAttackName() const
{
	return NightAttackName;
}
