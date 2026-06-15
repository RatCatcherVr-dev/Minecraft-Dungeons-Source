#pragma once

#include "MissionDef.h"
#include "MissionData.h"
#include "MissionSelection.h"
#include "theme/MissionTheme.h"
#include "state/MissionState.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/difficulty/Difficulty.h"
#include "game/affector/AffectorTypes.h"
#include "game/mission/variation/LevelVariationType.h"
#include <SoftObjectPtr.h>
#include "MissionDefs.generated.h"

namespace missions {

const MissionDef& get(ELevelNames);
const TArray<const MissionDef*> getVariationsBasedOnLevel(ELevelNames);
const MissionDef* getChecked(ELevelNames);
void ensureLoadAllStringTables();

using MissionPred = Pred<const MissionDef&>;
TArray<const MissionDef*> getAllCurrentlyEnabled();
TArray<const MissionDef*> getAllCurrentlyEnabledMatching(const MissionPred&);
TArray<const MissionDef*> getAll();
TArray<const MissionDef*> getAllMatching(const MissionPred&);

bool areMissionDefsInitialized();
bool shouldPackageAssets(ELevelNames);

struct MutableMissionDef;

namespace overrides {
	const MutableMissionDef& getOriginal(ELevelNames);
	const MutableMissionDef& getCurrent(ELevelNames);
	void add(Unique<MissionDef>);
	void clear();
}

const ELevelNames Default = ELevelNames::creeperwoods;

}

UENUM(BlueprintType)
enum class EThreatDescriptionType : uint8 {
	Default,
	EndlessStruggle,
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FThreatDescription {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	EThreatDescriptionType Type;

	UPROPERTY(BlueprintReadOnly)
	FText Text;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDifficultyRecommendation {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	EGameDifficulty RecommendedDifficulty;

	UPROPERTY(BlueprintReadOnly)
	EThreatLevel RecommendedThreatLevel;

	UPROPERTY(BlueprintReadOnly)
	EExtraChallenge RecommendedExtraChallenge;

	UPROPERTY(BlueprintReadOnly)
	FEndlessStruggle RecommendedEndlessStruggle;

	UPROPERTY(BlueprintReadOnly)
	int GearPowerDistance;

	game::FDifficulty CreateDifficulty() const;
	TOptional<game::FDifficulty> GetNextDifficulty() const;
};


UCLASS(BlueprintType)
class UMissionDefs: public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsTutorial(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsMissionThreatLevelSelectorHidden(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsMissionThreatLevelUnlockedByDefault(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsSecret(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsSalvageEnabled(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionDisplayName(ELevelNames LevelName);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionDisplayStoryTitle(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionDisplayStoryContents(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UTexture2D* GetMissionLoadingScreenTexture(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionConfirmStartTitle(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionConfirmStartWarning(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetMissionRequiresStartConfirmation(ELevelNames LevelName);
	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TSoftObjectPtr<UTexture2D> GetMissionLoadingScreenSoftTexture(ELevelNames LevelName);	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<ELevelNames> GetRequiredMissions(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<ELevelNames> GetAllNonDisabledMissions();

	static const FMapColors& GetMapColors(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ELevelVariationType GetMissionVariationType(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EMissionTheme GetMissionUITheme(ELevelNames LevelName);

	static bool IsVariation(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsHyperMission(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsTrial(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EExtraChallenge GetMissionExtraChallenge(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetMissionRequiresOfferings(ELevelNames LevelName);

	static void PopulateRequiredMissions(ELevelNames mission, TSet<ELevelNames>& RequiredMissions);

	static TSet<ELevelNames> GetRequiredMissionsTree(ELevelNames mission);
	
	static const int GetDifficultyRecommendedTotalGearItemPower(const game::FDifficulty& difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const int GetRecommendedTotalGearItemPower(const FMissionDifficulty& missionDifficulty);

	static FDifficultyRecommendation GetGearPowerDifficultyRecommendation(EExtraChallenge maxExtraChallenge, int DisplayGearPower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FDifficultyRecommendation GetGearPowerDifficultyRecommendation(ELevelNames LevelName, int DisplayGearPower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EThreatLevel GetLastSelectableThreatLevel(ELevelNames LevelName, EGameDifficulty difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EThreatLevel GetFirstSelectableThreatLevel(ELevelNames LevelName, EGameDifficulty difficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FThreatDescription> GetMapThreatLevelDescriptions(const FMissionDifficulty& missionDifficulty, APlayerCharacter* player, int displayGearPower = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionAsRequirementToolTipText(ELevelNames LevelNameRequired);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionAsRequirementText(ELevelNames requirement);	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetSecretUnlockInMissionAsRequirementText(ELevelNames foundInMission);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionTrialText(ELevelNames mission);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionLockedText(ELevelNames mission);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMissionLockedDescription(ELevelNames mission);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FAffectorInfo> GetAffectorInfo(ELevelNames mission);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetThreatLevelFraction(EThreatLevel threatLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetDifficultyThreatGlobalThreatFraction(EGameDifficulty difficulty, EThreatLevel threatLevel);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float GetDisplayGearPowerGlobalThreatFraction(int DisplayGearPower);

	UFUNCTION(BlueprintPure, BlueprintPure)
	static ENightModeType GetMissionNightModeType(ELevelNames LevelName);
};
