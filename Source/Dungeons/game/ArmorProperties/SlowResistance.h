#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "SlowResistance.generated.h"

UCLASS()
class DUNGEONS_API USlowResistanceGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	USlowResistanceGameplayEffect();
};

UCLASS()
class DUNGEONS_API USlowResistance : public UArmorProperty
{
	GENERATED_BODY()
public:
	USlowResistance();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Resistance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

private:
	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UEnchantmentSlowResistance : public UEnchantment
{
	GENERATED_BODY()
public:
	UEnchantmentSlowResistance();

	FText CreateDescription() const override;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PerLevelResistance = 0.1f;

private:
	FActiveGameplayEffectHandle Handle;
};