#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/item/ItemSlot.h"
#include "Smiting.generated.h"

UCLASS()
class DUNGEONS_API USmiting : public UEnchantment {
	GENERATED_BODY()

public:
	USmiting();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DamageMultiplierBase = 1.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DamageMultiplierPerLevel = 0.1f;

	void OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnBeforeDealtRangedDamage(float& outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

protected:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag SmitingDamageType;
};



