#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "ItemCooldownDecrease.generated.h"

namespace itemcooldowneffect { const FName DataName = TEXT("ItemCooldownEffectMagnitude"); }

UCLASS()
class DUNGEONS_API UItemCooldownDecreaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UItemCooldownDecreaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UItemCooldownDecrease : public UArmorProperty
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UItemCooldownDecrease();

protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 0.60f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UItemCooldownDecreaseGameplayEffect> Effect;
	
	FActiveGameplayEffectHandle Handle;
};
