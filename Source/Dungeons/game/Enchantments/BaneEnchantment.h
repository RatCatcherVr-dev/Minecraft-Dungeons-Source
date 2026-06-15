#pragma once

#include "CoreMinimal.h"
#include "Dungeons.h"
#include "game/Enchantments/Enchantment.h"
#include "BaneEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UBaneEnchantment : public UEnchantment {
	GENERATED_BODY()
public:
	UBaneEnchantment();

	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float BaneMultiplier = 1.2f;

	UPROPERTY(EditDefaultsOnly)
	float BanePercentagePerLevel = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag BaneType;

	size_t BaneMobTag;

	FGameplayAbilitySpecHandle CurrentSpec;
};

UCLASS()
class DUNGEONS_API UBaneOfIllagers : public UBaneEnchantment {
	GENERATED_BODY()
public:
	UBaneOfIllagers();
};
