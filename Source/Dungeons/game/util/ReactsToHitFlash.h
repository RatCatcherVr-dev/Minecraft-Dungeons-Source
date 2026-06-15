#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ReactsToHitFlash.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UReactsToHitFlash : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class DUNGEONS_API IReactsToHitFlash
{
	GENERATED_BODY()
public:
	virtual void OnHitFlash(FLinearColor color, float duration) {};
};
