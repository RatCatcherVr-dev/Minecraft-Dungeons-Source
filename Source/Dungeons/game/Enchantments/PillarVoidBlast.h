#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "VoidTouched.h"
#include "PillarVoidBlast.generated.h"

UCLASS()
class DUNGEONS_API UPillarVoidBlastGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPillarVoidBlastGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UPillarVoidBlast : public UEnchantment
{
	GENERATED_BODY()

public:
	UPillarVoidBlast();

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey);

	UPROPERTY(EditDefaultsOnly)
		float blastRange = 500.0f;
	UPROPERTY(EditDefaultsOnly)
		float blastDamage = 100.0f;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UStaggerGameplayEffect> StunEffect = UStaggerGameplayEffect::StaticClass();
	UPROPERTY(EditDefaultsOnly)
		float stunDuration = 1.f;
	UPROPERTY(EditDefaultsOnly)
		float voidTouchDuration = 2.f;
};

