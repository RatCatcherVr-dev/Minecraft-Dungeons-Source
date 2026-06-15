#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "Power.generated.h"

UCLASS()
class DUNGEONS_API UPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPowerGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UPower : public UEnchantment
{
	GENERATED_BODY()

	UPower();

	void OnStart() override;

	void OnEnd() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPowerGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;

	UPROPERTY(EditDefaultsOnly)
	float MobDamageMultiplier = 2.0f;

};
