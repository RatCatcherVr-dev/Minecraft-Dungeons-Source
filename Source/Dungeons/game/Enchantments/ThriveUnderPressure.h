#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "ThriveUnderPressure.generated.h"


UCLASS()
class DUNGEONS_API UThriveUnderPressureGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UThriveUnderPressureGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UThriveUnderPressure : public UEnchantment
{
	GENERATED_BODY()

	UThriveUnderPressure();

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UThriveUnderPressureGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;

	UPROPERTY(EditDefaultsOnly)
	float TriggerRange = 400.0f;

	UPROPERTY(EditDefaultsOnly)
	int MobThreshold = 4;

private:
	bool effectOn = false;
};
