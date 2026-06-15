#pragma once

#include <Interface.h>
#include "Attachable.generated.h"

UINTERFACE(BlueprintType)
class DUNGEONS_API UAttachable : public UInterface
{
	GENERATED_BODY()
};

class DUNGEONS_API IAttachable {
	GENERATED_BODY()
public:

	/** Attached to actor was hidden. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeons | Attachable")
	void AttachedToActorHidden(AActor* attachedToActor);
};