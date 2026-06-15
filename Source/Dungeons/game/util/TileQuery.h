#pragma once

#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "client/resource/Resource.h"
#include "TileQuery.generated.h"

USTRUCT(BlueprintType)
struct FTileRotation {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float DegreesUE4;

	UPROPERTY(BlueprintReadOnly)
	float DegreesJson;
};

FTileRotation TileRotationFromUeDegrees(float);
FTileRotation TileRotationFromJsonDegrees(float);
FTileRotation TileRotationWithFailState(float stateDegrees);


UCLASS()
class DUNGEONS_API UTileQuery : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	// (Tile) Degrees
	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery")
	static FTileRotation GetTileDegrees(const AActor* actor);

	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery", meta = (WorldContext = "WorldContextObject"))
    static FTileRotation GetTileDegreesAtLocation(UObject* WorldContextObject, FVector AtLocation);

	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery", meta = (WorldContext = "WorldContextObject"))
	static FTileRotation GetMyTileDegrees(UObject* WorldContextObject);

	// ResourcePack
	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery")
	static EResourcePack GetResourcePack(const AActor* actor);

	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery", meta = (WorldContext = "WorldContextObject"))
	static EResourcePack GetResourcePackAtLocation(const UObject* WorldContextObject, FVector AtLocation);

	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery", meta = (WorldContext = "WorldContextObject"))
	static EResourcePack GetMyTileResourcePack(const UObject* WorldContextObject);

	// Ambience GroupName
	UFUNCTION(BlueprintPure, Category = "Dungeons|TileQuery", meta = (WorldContext = "WorldContextObject"))
	static FString GetMyTileAmbienceGroupName(const UObject* WorldContextObject);
};
