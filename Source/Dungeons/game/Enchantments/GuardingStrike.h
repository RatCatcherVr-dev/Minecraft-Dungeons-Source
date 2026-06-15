#pragma once
#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GuardingStrike.generated.h"

UCLASS()
class DUNGEONS_API UGuardingStrikeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGuardingStrikeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UGuardingStrike : public UEnchantment
{
	GENERATED_BODY()
public:
	UGuardingStrike();

	FText CreateDescription() const override;

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UGuardingStrikeGameplayEffect> GuardingStrikeEffect;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float effectBaseDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float effectDurationPerLevel = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float damageReduction = 0.5f;

	void ApplyEffect();

	FActiveGameplayEffectHandle EffectHandle;
};