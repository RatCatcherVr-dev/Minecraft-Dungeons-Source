 #pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "MissionChancesType.h"
#include "MissionChances.h"
#include "game/levels.h"
#include "game/mission/request/MissionRequest.h"
#include "ui/info/ChanceInfoIconWidget.h"
#include "MissionChancesUtil.generated.h"

class AActor;

UENUM(BlueprintType)
enum class EMobStatus : uint8 {
	NotDefeated,
	Defeated,
	NotOwningDLC,
};


UCLASS(BlueprintType)
class DUNGEONS_API UMissionChancesUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetMissionChancesTypeText(EMissionChancesType chancesType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetMissionChancesText(const FMissionChances& chances);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<EMissionChancesType> GetMissionChancesTypes(ELevelNames LevelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<EMissionChancesType> GetMissionChancesTypesFiltered(ELevelNames LevelName, TArray<EMissionChancesType> AllowedTypes);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FMissionChances GetMissionChances(const FMissionState& missionState, EMissionChancesType chancesType);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionProbability> GetMissionProbabilities(const FMissionState& missionState);
			
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	static TSubclassOf<UChanceInfoIconWidget> GetMobChanceIconClass(UObject* WorldContextObject, const FMissionMobChance& mobChance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FText& GetMobChanceText(const FMissionMobChance& mobChance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetMobChanceDLCText(const FMissionMobChance& mobChance);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool GetCanShowItemType(const FSerializableItemId& itemId, AActor* progressOwner);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EMobStatus GetMobStatus(const FMissionMobChance& mobChance, AActor* progressOwner);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionItemTypeChance> GetMapLootItemTypeChances(const FMissionDifficulty& missionDifficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionItemTypeChance> GetMapArtifactItemTypeChances(const FMissionDifficulty& missionDifficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionItemTypeChance> GetMapRewardItemTypeChances(const FMissionDifficulty& missionDifficulty);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionItemTagChance> GetMapRewardItemTagChances(ELevelNames levelName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FMissionItemRarityChance> GetMapRewardItemRarityChances(ELevelNames levelName);

	static TArray<FMissionMobChance> GetMapUIMobChances(ELevelNames, const FEligibleDLC&, const ItemArchetypeCounts&);
	static TArray<FMissionProbability> GetMapProbabilities(const FMissionState& missionState);

	static float GetMapMobChance(const FMissionState& missionState);
};
