#pragma once
#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/component/SpawnActorIntervalComponent.h"
#include "game/actor/ContinousDamageActor.h"
#include "TwistingVineGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UTwistingVineDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTwistingVineDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UTwistingVineUniqueDamageGameplayEffect : public UTwistingVineDamageGameplayEffect {
	GENERATED_BODY()
public:
	UTwistingVineUniqueDamageGameplayEffect();
};
