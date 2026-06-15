

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "Optional.h"
#include "game/util/ValueFormat.h"
#include "PotionCooldownDecrease.generated.h"

UCLASS()
class DUNGEONS_API UPotionCooldownDecreaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPotionCooldownDecreaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UPotionCooldownDecrease : public UArmorProperty
{
	GENERATED_BODY()

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type reason) override;
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(CooldownDecrease); };

	FActiveGameplayEffectHandle Handle;
	TOptional<float> CachedMagnitude;
public:
	UPotionCooldownDecrease();

	UPROPERTY()
	float CooldownDecrease = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPotionCooldownDecreaseGameplayEffect> EffectToApply;
};
