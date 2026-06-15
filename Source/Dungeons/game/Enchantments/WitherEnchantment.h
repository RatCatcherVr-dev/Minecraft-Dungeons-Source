#pragma once

#include "CoreMinimal.h"
#include "Enchantment.h"
#include "WitherEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UWitherEnchantment : public UEnchantment {
	GENERATED_BODY()
public:
	UWitherEnchantment();
protected:
	void ApplyWitherPoison(ABaseCharacter* character);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UWitherApplierGameplayEffect> ApplierEffect;

	UPROPERTY(EditDefaultsOnly)
	float WitherDamageTotalFraction = 0.1f;
};


UCLASS()
class DUNGEONS_API UWitherEnchantmentMelee : public UWitherEnchantment {
	GENERATED_BODY()
public:
	UWitherEnchantmentMelee();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};

UCLASS()
class DUNGEONS_API UWitherEnchantmentRanged : public UWitherEnchantment {
	GENERATED_BODY()
public:
	UWitherEnchantmentRanged();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
};