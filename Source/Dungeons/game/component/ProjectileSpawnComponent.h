

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/item/BaseProjectile.h"
#include <Engine/EngineTypes.h>
#include "ProjectileSpawnComponent.generated.h"


USTRUCT(BlueprintType)
struct DUNGEONS_API FProjectileSpawnDefinition {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABaseProjectile> Projectile = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int NumberOfProjectiles = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelaySecondsPerProjectile = 0.f;
	//Defines the angle span (centered around target vector + AngleOffset)	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AngleSpan = 0.f;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UProjectileSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnProjectiles(const FProjectileSpawnDefinition& SpawnDefinition, FVector Direction, FVector Offset);

	void InternalSpawnProjectile(const FVector Direction, const FVector Offset, int Count);

	FTimerHandle CurrentAttackTimerHandle;

	TOptional<FProjectileSpawnDefinition> CurrentAttack;
public:	
	// Sets default values for this component's properties
	UProjectileSpawnComponent();

	//Spawn projectiles at owner location
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FProjectileSpawnDefinition& SpawnDefinition, FVector Direction, FVector Offset);
};
