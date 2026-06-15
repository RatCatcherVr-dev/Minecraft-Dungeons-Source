#pragma once
#include "game/Enchantments/Enchantment.h"
#include "CoreMinimal.h"

#include "ShadowBarb.generated.h"


UCLASS()
class DUNGEONS_API UShadowBarbRanged : public UEnchantment
{
	GENERATED_BODY()
public:
	UShadowBarbRanged();

	void OnBeforeDealtRangedDamage(float &outDamage, FGameplayTag& damageType, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	void OnInvisibilityGained() override;


private:
	int remainingAttacks = 0;
};