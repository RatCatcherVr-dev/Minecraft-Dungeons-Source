#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "game/util/ValueFormat.h"
#include "game/component/AreaBuffComponent.h"
#include "LifeStealAura.generated.h"

UCLASS()
class DUNGEONS_API ULifeStealAuraGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	ULifeStealAuraGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API ULifeStealAura : public UArmorProperty
{
	GENERATED_BODY()
public:
	ULifeStealAura();

	void BeginPlay() override;
	
	void EndPlay(EEndPlayReason::Type reason) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asPercentage(Amount); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Amount = 0.06f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 1000.0f;
private:
	TWeakObjectPtr<class UAreaBuffComponent> OwnerAreaBuffComponent;
};
