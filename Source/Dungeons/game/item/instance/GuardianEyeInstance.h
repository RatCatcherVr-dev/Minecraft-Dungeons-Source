#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/actor/item/InterruptableItemInstance.h"
#include "game/util/Pushback.h"
#include "GuardianEyeInstance.generated.h"

UCLASS()
class DUNGEONS_API UGuardianEyeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGuardianEyeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UGuardianEyeDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UGuardianEyeDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UGuardianEyeChargeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGuardianEyeChargeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API AGuardianEyeInstance : public AInterruptableItemInstance {
	GENERATED_BODY()

public:
	AGuardianEyeInstance();

	float GetStats(EItemStats stat) const;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Activate(const FPredictionKey& predictionKey) override;

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginCharge(const FPredictionKey predictionKey);

	UFUNCTION()
	void BeginBeam(const FPredictionKey predictionKey);

	UFUNCTION()
	void EndBeam(const FPredictionKey predictionKey);

	void Interrupt() override;

	UFUNCTION(Client, Reliable, WithValidation)
	void InterruptLocal();
	UFUNCTION(Server, Reliable, WithValidation)
	void InterruptServer();

	bool IsActive() const { return bIsActive; };

protected:

	TArray<AActor*> GetTargetsInBeam();
	void ApplyDamage(const ABaseCharacter* target, FGameplayEffectSpec damageSpec);
	void ApplyDamageToTargetsInBeam();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float baseDamage = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGuardianEyeChargeGameplayEffect> chargeEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGuardianEyeGameplayEffect> laserEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float aimingTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float laserTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FPushback onKillPushback;

	FTimerHandle attackDelayHandle;
	FTimerHandle attackEndDelayHandle;
	bool bIsActive = false;
};