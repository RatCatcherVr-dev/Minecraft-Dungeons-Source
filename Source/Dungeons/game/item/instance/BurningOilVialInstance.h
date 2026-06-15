#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/item/BaseProjectile.h"
#include <GameplayEffect.h>
#include "BurningOilVialInstance.generated.h"

UCLASS()
class DUNGEONS_API UBurningOilVialGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBurningOilVialGameplayEffect(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|ABurningOilVialInstance")
		TSubclassOf<ABaseProjectile> ProjectileClass;
};


/**
 * 
 */
UCLASS()
class DUNGEONS_API ABurningOilVialInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	ABurningOilVialInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Dungeons")
   float ArrowPowerBoostAmount = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|ABurningOilVialInstance")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	TSubclassOf<UBurningOilVialGameplayEffect> Effect;
};
