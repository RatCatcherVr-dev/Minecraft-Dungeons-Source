#pragma once
#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "PotionThirst.generated.h"

UCLASS()
class DUNGEONS_API UPotionThirstMelee : public UEnchantment
{
	GENERATED_BODY()
public:
	UPotionThirstMelee();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;

	void ApplyEffect();

protected:

	// cooldown reduction at level one
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float cooldownReductionBase = 1.0f;

	// additional cooldown reduction for each additional level
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float cooldownReductionPerLevel = 0.5f;
};

UCLASS()
class DUNGEONS_API UPotionThirstRanged : public UEnchantment
{
	GENERATED_BODY()
public:
	UPotionThirstRanged();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	void ApplyEffect();

protected:

	// cooldown reduction at level one
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float cooldownReductionBase = 1.0f;

	// additional cooldown reduction for each additional level
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float cooldownReductionPerLevel = 0.5f;
};