#pragma once
#include <CoreMinimal.h>
#include "SuccessStatus.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FSuccessStatus {
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText status;	
};
