#pragma once
#include <CoreMinimal.h>
#include "ProblemStatus.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FProblemStatus {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText problem;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText remedy;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText progress;
};
