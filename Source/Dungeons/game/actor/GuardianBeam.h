#pragma once

#include "character/player/PlayerCharacter.h"
#include "character/BaseCharacter.h"
#include "GuardianBeam.generated.h"

UCLASS()
class DUNGEONS_API UGuardianBeamGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGuardianBeamGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UElderGuardianBeamGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UElderGuardianBeamGameplayEffect(const FObjectInitializer& ObjectInitializer);
};