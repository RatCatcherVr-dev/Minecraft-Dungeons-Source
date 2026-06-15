#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/OnLandingExecutionEnchantment.h"
#include <GameplayEffectTypes.h>
#include "world/level/BlockPos.h"
#include <GameplayEffect.h>
#include "game/actor/ContinousDamageActor.h"
#include "FireTrail.generated.h"

UCLASS()
class DUNGEONS_API UFireTrailDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFireTrailDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UFireTrailSpeedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFireTrailSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS(BlueprintType)
class DUNGEONS_API AFireBlock : public AContinousDamageActor {
	GENERATED_BODY()
public:
	AFireBlock();
};

UCLASS()
class DUNGEONS_API UFireTrail : public UEnchantment {
	GENERATED_BODY()
	
public:
	UFireTrail();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	FText CreateDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<AFireBlock> PlayerFireBlockClass;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<AFireBlock> MobFireBlockClass;

	void OnStart() override;
	void OnEnd() override;
	
	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;
	void OnDodgeRollEnd(FPredictionKey) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamagePerSecond = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DurationAfterDodgeEnd = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeons")
	float FireBlockDuration = 4.0f;

private:
	void OnExitedBlock(const BlockPos& blockPos) const;
	
	void StartFireSpawning();

	void StopFireSpawning();

	void SpawnFireBlock(const BlockPos& blockPos) const;

	void ApplySpeedEffect();

	void RemoveSpeedEffect() const;

	FActiveGameplayEffectHandle SpeedEffectHandle;
	
	FTimerHandle TrailTimerHandle;

	bool bShouldSpawnFire = false;

	bool bIsOwnerPlayer = false;

	BlockPos lastBlockPos;
};
