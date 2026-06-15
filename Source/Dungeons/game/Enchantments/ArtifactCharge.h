#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "ArtifactCharge.generated.h"


UCLASS()
class DUNGEONS_API UArtifactChargeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UArtifactChargeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UArtifactCharge : public UEnchantment
{
	GENERATED_BODY()

public:
	UArtifactCharge();

	void OnItemActivated(const AItemInstance*, FPredictionKey key) override;
	void OnRangedAttackEnded(bool completed, bool didSpawnProjectiles) override;
	void OnEnd() override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<TSubclassOf<UArtifactChargeGameplayEffect>> EffectPerLevel;
};