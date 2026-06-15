#pragma once

#include "CoreMinimal.h"
#include "game/component/AreaBuffComponent.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "MoveSpeedAura.generated.h"

UCLASS()
class DUNGEONS_API UMoveSpeedAuraGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	UMoveSpeedAuraGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UMoveSpeedAura : public UArmorProperty
{
	GENERATED_BODY()
	
public:
	UMoveSpeedAura();

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type reason) override;
protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeMultiplierPercentageChange(Multiplier); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 1.15f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 1000.0f;
private:
	TWeakObjectPtr<class UAreaBuffComponent> OwnerAreaBuffComponent;
};
