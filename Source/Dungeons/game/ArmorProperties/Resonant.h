#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "Resonant.generated.h"

UCLASS()
class DUNGEONS_API UResonantGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UResonantGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UResonant : public UArmorProperty
{
	GENERATED_BODY()
public:
	UResonant();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(mResistMagnitude); };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float mResistMagnitude = 1.2f;

	FActiveGameplayEffectHandle Handle;
};
