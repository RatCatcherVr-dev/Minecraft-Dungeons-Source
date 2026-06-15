#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "StartMissionStatus.h"
#include "StartMissionStatusUtil.generated.h"

UCLASS(BlueprintType)
class UStartMissionStatusUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool IsStartable(const FStartMissionStatus& status);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool HasProblem(const FStartMissionStatus& status);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FProblemStatus GetProblem(const FStartMissionStatus& status);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool HasSuccess(const FStartMissionStatus& status);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FSuccessStatus GetSuccess(const FStartMissionStatus& status);
};
