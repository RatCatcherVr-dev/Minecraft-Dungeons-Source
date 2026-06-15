#pragma once

#include "CoreMinimal.h"
#include <GameplayEffect.h>
#include <ObjectMacros.h>
#include "WorldDamageGameplayEffect.h"
#include "UpdraftGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UBlastGameplayEffect : public UWorldDamageGameplayEffect
{
	GENERATED_BODY()
public:
	UBlastGameplayEffect();
};

UCLASS()
class DUNGEONS_API UUpdraftGameplayEffect : public UWorldDamageGameplayEffect
{
	GENERATED_BODY()
public:
	UUpdraftGameplayEffect();
};

UCLASS()
class DUNGEONS_API UUpdraftImmunityGameplayEffect : public UUpdraftGameplayEffect
{
	GENERATED_BODY()
public:
	UUpdraftImmunityGameplayEffect();
};
