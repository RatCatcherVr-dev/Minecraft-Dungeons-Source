#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include <ObjectMacros.h>
#include "DamageSelfGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UDamageSelfGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UDamageSelfGameplayEffect();
};
