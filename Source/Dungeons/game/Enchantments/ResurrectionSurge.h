#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>

#include "ResurrectionSurge.generated.h"

UCLASS()
class DUNGEONS_API UResurrectionSurgeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UResurrectionSurgeGameplayEffect();
};

UCLASS()
class DUNGEONS_API UResurrectionSurge : public UEnchantment {
	GENERATED_BODY()
public:
	UResurrectionSurge();

	UPROPERTY(EditDefaultsOnly)
	float SurgeMultiplier = 1.11f;

	UPROPERTY(EditDefaultsOnly)
	float SurgePerLevel = 0.11f;

	void OnStart() override;

	void OnEnd() override;

	FActiveGameplayEffectHandle handle;

};