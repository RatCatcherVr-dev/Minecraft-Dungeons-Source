

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "DungeonsGameplayCueNotify_Static.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsGameplayCueNotify_Static : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	bool HandlesEvent(EGameplayCueEvent::Type EventType) const override;
};
