#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "MissionOfferings.h"
#include "game/levels.h"
#include "game/mission/start/StartMissionStatus.h"
#include "game/mission/state/MissionState.h"
#include "ui/status/ProblemStatus.h"
#include "MissionOfferingsUtil.generated.h"

struct FOfferingsSummary;
struct ItemArchetypeCounts;
struct FEligibleDLC;

UCLASS(BlueprintType)
class UMissionOfferingsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TMap<EItemArchetype, int> GetOfferingsArchetypeCounts(const FMissionOfferings& offerings);

	static TOptional<FProblemStatus> QueryMissionOfferingsProblem(ELevelNames, const FOfferingsSummary&, const ItemArchetypeCounts&, const FEligibleDLC&);
	static TOptional<FProblemStatus> QueryMissionOfferingsProblem(const FMissionState& state);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (MissionOfferings)", CompactNodeTitle = "=="), Category = "Mission|MissionOfferings")
	static bool EqualEqual_MissionOfferings(const FMissionOfferings& A, const FMissionOfferings& B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (MissionOfferings)", CompactNodeTitle = "!="), Category = "Mission|MissionOfferings")
	static bool NotEqual_MissionOfferings(const FMissionOfferings& A, const FMissionOfferings& B);
};
