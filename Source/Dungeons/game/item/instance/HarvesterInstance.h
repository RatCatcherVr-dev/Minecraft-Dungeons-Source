#pragma once

#include "AItemInstance.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "HarvesterInstance.generated.h"

class AMobCharacter;
class APlayerCharacter;
UCLASS()
class DUNGEONS_API UHarvesterDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UHarvesterDamageGameplayEffect();
};

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API AHarvesterInstance : public AItemInstance {
	GENERATED_BODY()
		
public:
	AHarvesterInstance();
	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const;

private:
	void ExecuteHarvesterGameplayCue(const APlayerCharacter* player);
	void ApplyHarvesterEffectToMobs(const APlayerCharacter* player);

protected:
	virtual void OnPostMobDeath(AMobCharacter* mob);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGameplayTag ExplosionTriggerTag;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int ExplosionDamageRange = 800;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int DamagePerSoul = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FPushback OnKillPushback;
};
