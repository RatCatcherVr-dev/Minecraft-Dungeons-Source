#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Flee.generated.h"

UCLASS()
class DUNGEONS_API UFleeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFleeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFlee : public UEnchantment
{
	GENERATED_BODY()
public:
	UFlee();

	FText CreateDescription() const override;

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UFleeGameplayEffect> FleeEffect;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float resetDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float percentPerLevel = 0.3f;

	void ApplyEffect();

	FActiveGameplayEffectHandle EffectHandle;
};