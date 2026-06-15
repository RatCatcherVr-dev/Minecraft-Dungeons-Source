#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "VoidTouched.h"
#include "game/item/instance/WindItemInstance.h"
#include "VoidBlast.generated.h"

UCLASS()
class DUNGEONS_API UVoidBlastGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UVoidBlastGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UVoidBlast : public UEnchantment
{
	GENERATED_BODY()

public:
	UVoidBlast();


	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey);

	UPROPERTY(EditDefaultsOnly)
		float blastRange = 500.0f;
	UPROPERTY(EditDefaultsOnly)
		float blastDamage = 150.0f;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UWindHornSlowGameplayEffect> slowEffect = UWindHornSlowGameplayEffect::StaticClass();
	UPROPERTY(EditDefaultsOnly)
		float slowDuration = 2.f;
	UPROPERTY(EditDefaultsOnly)
		float slowMultiplier = 0.2f;
};

