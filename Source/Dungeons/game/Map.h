#pragma once

#include "Dungeons.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "MapTexture.h"
#include "Map.generated.h"

class AGameBP;

UCLASS()
class DUNGEONS_API UMapUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static UMapTexture2D* createMapTexture2D(UObject* WorldContextObject, EMapTextureSampler samplerMode);

	static const int defaultWidth;
	static const int defaultHeight;	
};