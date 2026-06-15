#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "ImmunityBoost.generated.h"

UCLASS()
class DUNGEONS_API UImmunityBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UImmunityBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UImmunityBoost : public UArmorProperty
{
	GENERATED_BODY()
public:
	UImmunityBoost();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(mResistanceMagnitude); };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float mResistanceMagnitude = 0.7f;

	FActiveGameplayEffectHandle Handle;
};
