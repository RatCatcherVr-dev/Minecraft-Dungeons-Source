#pragma once
#include "MissionRequest.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/mission/offerings/MissionOfferings.h"
#include "game/mission/MissionSelection.h"
#include "game/mission/MissionEditor.h"
#include "MissionRequestUtil.generated.h"

class APlayerCharacter;

UCLASS(BlueprintType)
class UMissionRequestUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

private:
	static TOptional<FMissionRequest> MakeOptionalRequest(EMissionRequestType, TOptional<FMissionState>);
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FMissionRequest CreateMissionRequest(const APlayerCharacter* player, EMissionRequestType requestType, const FMissionSelection& selection, int32 seed, const FMissionOfferings& offerings);
	
	static TOptional<FMissionRequest> InspectPlayerStartMissionRequest(const APlayerCharacter* progressOwner, const FMissionSelection& selection);
	static TOptional<FMissionRequest> ProducePlayerStartMissionRequest(APlayerCharacter* progressOwner, const FMissionSelection& selection);
	static TOptional<FMissionRequest> InspectPlayerContinueMissionRequest(const APlayerCharacter* progressOwner, ELevelNames mission);	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const FMissionState& GetMissionState(const FMissionRequest& request);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EMissionEditor GetMissionRequestDesiredEditor(const FMissionRequest& state);
};
