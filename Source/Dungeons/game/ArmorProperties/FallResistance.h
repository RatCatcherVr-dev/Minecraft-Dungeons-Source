#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "FallResistance.generated.h"

UCLASS()
class DUNGEONS_API UFallResistanceGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UFallResistanceGameplayEffect();
};

UCLASS()
class DUNGEONS_API UFallResistance : public UArmorProperty
{
	GENERATED_BODY()
public:
	UFallResistance();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Resistance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

private:
	FActiveGameplayEffectHandle Handle;
};