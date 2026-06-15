#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/mission/MissionSelection.h"
#include "game/mission/offerings/MissionOfferings.h"
#include "game/mission/MissionDef.h"
#include "MissionState.h"
#include "game/item/ItemBulletPoint.h"
#include "MissionStateUtil.generated.h"

class APlayerCharacter;


UCLASS(BlueprintType)
class UMissionStateUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()	
public:
	static TOptional<FMissionState> InspectProducedMissionState(const APlayerCharacter*, const FMissionSelection&);
	static TOptional<FMissionState> ProduceMissionState(APlayerCharacter*, const FMissionSelection&);
	static TOptional<FMissionState> GetMissionState(const APlayerCharacter*, ELevelNames);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static EGameDifficulty GetDifficulty(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static EThreatLevel GetThreatLevel(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static ELevelNames GetLevelName(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FEndlessStruggle GetEndlessStruggle(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FItemBulletPoint> GetBulletPoints(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int GetTotalDisplayItemPower(const FMissionState& missionState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FMissionDifficulty& getMissionDifficulty(const FMissionState& missionState);
};
