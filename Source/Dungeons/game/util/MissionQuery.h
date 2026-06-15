#pragma once

#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "MissionQuery.generated.h"

enum class ELevelNames : uint8;

UCLASS()
class DUNGEONS_API UMissionQuery : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Dungeons|MissionQuery", meta = (WorldContext = "WorldContextObject"))
	static ELevelNames GetLevelName(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Dungeons|MissionQuery", meta = (WorldContext = "WorldContextObject"))
	static void GetPropNamesForTile(UObject* WorldContextObject, const FString& TileName, TArray<FString>& PropNames );

	UFUNCTION(BlueprintPure, Category = "Dungeons|MissionQuery", meta = (WorldContext = "WorldContextObject"))
	static FText GetMissionNightName(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Dungeons|MissionQuery", meta = (WorldContext = "WorldContextObject"))
	static FText GetMissionNightAttackName(UObject* WorldContextObject);
};
