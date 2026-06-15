#pragma once

#include "GameplayEffect.h"
#include "InvulnerableGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UInvulnerableNoCueGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UInvulnerableNoCueGameplayEffect();
};

UCLASS()
class DUNGEONS_API UInvulnerableGameplayEffect : public UInvulnerableNoCueGameplayEffect
{
	GENERATED_BODY()
public:
	UInvulnerableGameplayEffect();	
};
