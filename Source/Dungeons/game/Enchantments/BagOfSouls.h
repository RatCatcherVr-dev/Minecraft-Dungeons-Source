#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "EffectApplyingEnchantment.h"
#include "BagOfSouls.generated.h"

UCLASS()
class DUNGEONS_API UBagOfSoulsGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBagOfSoulsGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName MaxSoulsKey;
};

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API UBagOfSouls : public UEffectApplyingEnchantment
{
	GENERATED_BODY()

	UBagOfSouls();
	void OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MaxSoulsPercentageGainedPerLevel = 0.3333f;
};
