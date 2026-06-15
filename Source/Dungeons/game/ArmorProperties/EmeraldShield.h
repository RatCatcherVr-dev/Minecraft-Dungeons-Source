#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "game/component/WalletComponent.h"
#include "EmeraldShield.generated.h"

UCLASS()
class DUNGEONS_API UEmeraldShieldGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UEmeraldShieldGameplayEffect();
};

UCLASS()
class DUNGEONS_API UEmeraldShield : public UArmorProperty
{
	GENERATED_BODY()
public:
	UEmeraldShield();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

protected:
	//FString createFormattedValueString() const override { return valueformat::asMultiplierPercentageChange(Resistance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Duration = 1.f;	

	UFUNCTION()
	void OnEmeraldsCollected(int32 amount) override;
private:
	FActiveGameplayEffectHandle Handle;
};