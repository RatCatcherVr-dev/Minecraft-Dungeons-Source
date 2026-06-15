#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/util/ValueFormat.h"
#include "GameplayEffect.h"
#include "RollCharge.generated.h"


UCLASS()
class DUNGEONS_API URollChargeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URollChargeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API URollCharge : public UEnchantment
{
	GENERATED_BODY()

public:
	URollCharge();

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Duration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DurationPerLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<URollChargeGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};