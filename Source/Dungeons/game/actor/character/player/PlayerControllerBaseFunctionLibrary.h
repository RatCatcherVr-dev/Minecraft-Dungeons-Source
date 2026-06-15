#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerControllerBaseFunctionLibrary.generated.h"

class APlayerControllerBase;


UCLASS()
class DUNGEONS_API UPlayerControllerBaseFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	/** Returns Maximum Controller Index */
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static int32 GetMaxControllerIndex();

	/** Returns Number of Current Controllers */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static int32 CountControllers(const UObject* WorldContextObject);

	/** Returns First Player Controller */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static APlayerControllerBase* GetFirstDungeonsPlayerController(const UObject* WorldContextObject);
	
};