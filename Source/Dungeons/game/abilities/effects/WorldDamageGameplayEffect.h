#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include <ObjectMacros.h>
#include "WorldDamageGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UWorldDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UWorldDamageGameplayEffect();
};
