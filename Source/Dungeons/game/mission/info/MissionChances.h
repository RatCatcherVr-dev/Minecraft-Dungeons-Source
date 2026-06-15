#pragma  once
#include "game/mission/MissionData.h"
#include "MissionChancesType.h"
#include "MissionChances.generated.h"

USTRUCT(BlueprintType)
struct FMissionChances {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EMissionChancesType missionChancesType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMissionItemTypeChance> typeChances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMissionItemTagChance> tagChances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMissionItemRarityChance> rarityChances;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FMissionMobChance> mobChances;	
};
