#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "game/component/AreaBuffComponent.h"
#include "HealingAura.generated.h"

UCLASS()
class DUNGEONS_API UHealingAuraGameplayEffect : public UAreaBuffGameplayEffect
{
	GENERATED_BODY()
public:
	UHealingAuraGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UHealingAura : public UArmorProperty
{
	GENERATED_BODY()
public:
	UHealingAura();

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type reason) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asPercentage(Amount); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float Amount = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float Radius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		TSubclassOf<UHealingAuraGameplayEffect> HealingAuraEffectToApply = UHealingAuraGameplayEffect::StaticClass();
private:
	TWeakObjectPtr<class UAreaBuffComponent> OwnerAreaBuffComponent;
};