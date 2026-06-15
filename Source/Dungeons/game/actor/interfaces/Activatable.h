#pragma once

#include "Activatable.generated.h"

UINTERFACE(BlueprintType)
class DUNGEONS_API UActivatable : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONS_API IActivatable {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeons")
	void OnActivated();
};