#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include <ObjectMacros.h>
#include "WorldDamageGameplayEffect.h"
#include "TrapDamageGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UTrapDamageGameplayEffect : public UWorldDamageGameplayEffect
{
	GENERATED_BODY()
public:
	UTrapDamageGameplayEffect();
};
