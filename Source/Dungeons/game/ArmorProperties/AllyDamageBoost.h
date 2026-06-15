#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/component/AreaBuffComponent.h"
#include "game/util/ValueFormat.h"
#include "AllyDamageBoost.generated.h"

UCLASS()
class DUNGEONS_API UAllyDamageBoostGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	UAllyDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UAllyDamageBoost : public UArmorProperty
{
	GENERATED_BODY()

	UAllyDamageBoost();

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type reason) override;
protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 1000.0f;
private:
	TWeakObjectPtr<class UAreaBuffComponent> OwnerAreaBuffComponent;
};
