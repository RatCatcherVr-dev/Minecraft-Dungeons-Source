// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Deflecting.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDeflecting : public UEnchantment
{
	GENERATED_BODY()

	UDeflecting();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastDeflect(TSubclassOf<ABaseProjectile> projectileClass, AActor* target, uint32 oldProjectileId);

public:
	void OnOverlappedByProjectile(ABaseProjectile* projectile, const FRandomStream& randStream) override;

	void OnProjectileOverlapEnded(AActor* overlappedActor, AActor* otherActor);

	void OnStart() override;

	void OnEnd() override;
private:
	TSubclassOf<ABaseProjectile> GetDeflectedProjectileClass(const ABaseProjectile&) const;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TMap<TSubclassOf<ABaseProjectile>, TSubclassOf<ABaseProjectile>> ProjectileToDeflectedClasses;

	FDelegateHandle delegateHandle;
	TWeakObjectPtr<ABaseProjectile> deflectedProjectile;
};

UCLASS()
class DUNGEONS_API UDeflectingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDeflectingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
