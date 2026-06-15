#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include <GameplayEffect.h>
#include "game/Enchantments/Enchantment.h"
#include "EnvironmentalProtection.generated.h"

UCLASS()
class DUNGEONS_API UEnvironmentalProtectionGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UEnvironmentalProtectionGameplayEffect();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float FrozenSolidOwnerInteractionIncrement = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float FrozenSolidOtherInteractionIncrement = 1.f;
};

UCLASS()
class DUNGEONS_API UEnvironmentalProtection : public UArmorProperty
{
	GENERATED_BODY()
public:
	UEnvironmentalProtection();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;


	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

protected:
	FString createFormattedValueString() const override { return valueformat::asPercentage(Resistance); };

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

private:
	FActiveGameplayEffectHandle Handle;
};