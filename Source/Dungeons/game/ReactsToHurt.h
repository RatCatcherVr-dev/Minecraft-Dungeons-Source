#pragma once

#include "ReactsToHurt.generated.h"

UINTERFACE(BlueprintType)
class DUNGEONS_API UReactsToHurt : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONS_API IReactsToHurt {
	GENERATED_BODY()
public:

	/** Reacts to impact of a weapon, passes the variant as level. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeons | Impact")
	void ReactToHurt(AActor* byWhom, float magnitude);
};