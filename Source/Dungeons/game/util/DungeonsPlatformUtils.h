#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DungeonsPlatformUtils.generated.h"

UCLASS()
class DUNGEONS_API UDungeonsPlatformUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetPlatformUsesRightForAccept();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetPlatformSupportsTitleNewsNavigation();

};
