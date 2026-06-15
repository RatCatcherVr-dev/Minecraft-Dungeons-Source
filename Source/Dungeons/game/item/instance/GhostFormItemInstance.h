#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/GhostFormGameplayEffect.h"
#include "GhostFormItemInstance.generated.h"

UCLASS()
class UGhostCloakGameplayEffect : public UGhostFormBaseGameplayEffect {
	GENERATED_BODY()
public:
	UGhostCloakGameplayEffect();

	static const FName GhostFormDurationKey;
	static const FName GhostFormSpeedKey;
};

UCLASS()
class DUNGEONS_API AGhostFormItemInstance : public AItemInstance
{
	GENERATED_BODY()

	AGhostFormItemInstance();

	void BeginPlay() override;

	int GetDisplayCount() const override;
	void Activate(const FPredictionKey& predictionKey) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UGhostCloakGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SpeedMultiplierBase = 1.3f;

private:
	void OnPlayerAttack(const APlayerCharacter* player);

	FActiveGameplayEffectHandle EffectHandle;
};