#pragma once
#include "CoreMinimal.h"
#include "Enchantment.h"
#include "ChargingAcceleration.generated.h"

UCLASS()
class DUNGEONS_API UChargingAccelerationChargeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UChargingAccelerationChargeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UChargingAccelerationBaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UChargingAccelerationBaseGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class UChargingAcceleration : public UEnchantment {
	GENERATED_BODY()
public:
	UChargingAcceleration();

	void OnStart() override;
	void OnEnd() override;
protected:
	void OnBeforeRangedAttack(AActor* attackTarget, bool &attackDenied, FPredictionKey key) override;
	void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) override;

	// jryden: disabled for compile-OK after rebase
	//float GetCurrentCounterValue() override;
	//float GetMaxCounterValue() override;

	UPROPERTY(EditDefaultsOnly)
	float ChargeIntervalSeconds = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float AccelerationAmount = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UChargingAccelerationBaseGameplayEffect> Effect;
private:
	UFUNCTION()
	void OnCharge();
	void UnCharge();
	
	UFUNCTION()
	void OnAttackOver();

	FTimerHandle ChargeTimerHandle;
	FActiveGameplayEffectHandle AccelerationEffectHandle;
	FActiveGameplayEffectHandle ChargeEffectHandle;

	bool bInAttack = false;
};

