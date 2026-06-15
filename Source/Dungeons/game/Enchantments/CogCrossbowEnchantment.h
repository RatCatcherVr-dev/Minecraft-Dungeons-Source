#pragma once
#include "CoreMinimal.h"
#include "Enchantment.h"
#include "GameplayEffect.h"
#include "CogCrossbowEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UCogCrossBowChargeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCogCrossBowChargeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCogCrossbowEnchantment : public UEnchantment {
	GENERATED_BODY()
public:
	UCogCrossbowEnchantment();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type reason) override;

	void OnBeforeRangedAttack(AActor* attackTarget, bool& attackDenied, FPredictionKey key) override;
	void OnRangedAttackEnded(bool completed, bool didSpawnProjectiles) override;

	// jryden: disabled for compile-OK after rebase
	//float GetCurrentCounterValue() override;
	//float GetMaxCounterValue() override;
protected:
	UPROPERTY(EditDefaultsOnly)
	float ChargeDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCogCrossBowChargeGameplayEffect> EffectClass;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	void OnCharge();
	void UnCharge();

	UFUNCTION()
	void OnRep_CanAttack();

	FActiveGameplayEffectHandle EffectHandle;
	FTimerHandle ChargeHandle;
	TWeakObjectPtr<class URangedAttackComponent> RangedAttack;

	UPROPERTY(ReplicatedUsing=OnRep_CanAttack)
	bool bCanAttack = false;
};

