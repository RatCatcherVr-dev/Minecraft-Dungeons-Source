#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineFunctionLib.generated.h"

UCLASS()
class DUNGEONS_API UOnlineFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/* Verify can toggle friends bar */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Allow toggle friends bar by [KEY]"), Category = "Online")
	static bool AllowToggleFriendsSidebar();

};
