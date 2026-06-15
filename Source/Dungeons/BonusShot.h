#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "BonusShot.generated.h"

UCLASS()
class DUNGEONS_API UBonusShot : public UEnchantment
{
	GENERATED_BODY()
public:
	UBonusShot();

	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectilesMulticast(TSubclassOf<ABaseProjectile> ProjectileClass, float itemPower, float sourceItemPower, FTransform transform, ABaseCharacter * characterOwner, const  ABaseCharacter * secondaryTarget, bool isCharged);

private:
	ABaseCharacter* GetSecondaryTarget(AActor* firstTarget, const FRandomStream& randStream) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	float MobDamageFraction = 0.5f;
};
