#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "Heavyweight.generated.h"

UCLASS()
class DUNGEONS_API UHeavyweightGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UHeavyweightGameplayEffect();
};

UCLASS()
class DUNGEONS_API UHeavyweight : public UArmorProperty
{
	GENERATED_BODY()
public:
	UHeavyweight();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(1 - Resistance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.f;

private:
	FActiveGameplayEffectHandle Handle;
};


UCLASS()
class DUNGEONS_API UEnchantmentHeavyweight : public UEnchantment
{
	GENERATED_BODY()
public:
	UEnchantmentHeavyweight();

	FText CreateDescription() const override;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float Resistance = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float PerLevelResistance = 0.1f;

private:
	FActiveGameplayEffectHandle Handle;
};