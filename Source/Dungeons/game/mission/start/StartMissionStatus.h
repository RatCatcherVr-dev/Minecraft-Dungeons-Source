#pragma once
#include <CoreMinimal.h>
#include "ui/status/ProblemStatus.h"
#include "ui/status/SuccessStatus.h"
#include "StartMissionStatus.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FStartMissionStatus {
	GENERATED_BODY()

public:

	TOptional<FSuccessStatus> mSuccess;
	TOptional<FProblemStatus> mProblem;
	
	static FStartMissionStatus fail(const FProblemStatus&);

	static FStartMissionStatus success(const FSuccessStatus&);	
};
