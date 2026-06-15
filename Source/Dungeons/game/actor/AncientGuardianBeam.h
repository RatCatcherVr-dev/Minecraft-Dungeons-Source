#pragma once

#include "character/player/PlayerCharacter.h"
#include "character/BaseCharacter.h"
#include "AncientGuardianBeam.generated.h"

UCLASS()
class DUNGEONS_API UAncientGuardianBeamGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAncientGuardianBeamGameplayEffect(const FObjectInitializer& ObjectInitializer);
};