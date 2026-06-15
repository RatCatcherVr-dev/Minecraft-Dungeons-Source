#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <WeakObjectPtrTemplates.h>
#include "Ricochet.generated.h"

class ABaseProjectile;

/**
 * 
 */
UCLASS()
class DUNGEONS_API URicochet : public UEnchantment
{
	GENERATED_BODY()

	TArray<TPair<uint32, TWeakObjectPtr<ABaseProjectile>>> Projectiles;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRetargetArrow(uint32 id, ABaseCharacter* newTarget, FVector direction, bool recurse);

	UPROPERTY(EditDefaultsOnly)
	int MaxAllowedRecursionCount = 3;

	virtual bool RollForTrigger(const FRandomStream& randStream, ABaseProjectile* projectile) const;


public:
	URicochet();
	
	void OnAfterRangedAttackPerProjectile(AActor* attackTarget, const FVector& attackTargetLocation, ABaseProjectile* projectile, TSubclassOf<ABaseProjectile> ProjectileClass, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;
	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;
	void OnResetRangedProjectile(ABaseProjectile* fromProjectile);

	void OnProjectileOverlapEnded(AActor* overlappedActor, AActor* otherActor, bool shouldDestroyOnHit, TWeakObjectPtr<ABaseProjectile> projectile);

	class ABaseCharacter* GetBestTarget(AActor* sourceActor, const FVector& velocity);
};
