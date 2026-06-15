#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "ShockPowderInstance.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API AShockPowderInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AShockPowderInstance();

	int GetDisplayCount() const override;
	void ApplyStunEffectToMobs(const APlayerCharacter* player, const FPredictionKey& predictionKey) const;

	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UStaggerGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	int32 StunRange = 600;
private:
	FGameplayEffectSpec CreateStunEffectSpec(const UAbilitySystemComponent* abilitySystem) const;
	float CalculateDuration() const;

	TArray<AMobCharacter*> GetMobsToTarget(const APlayerCharacter* player) const;
	static bool IsValidStunTarget(const APlayerCharacter* player, const AMobCharacter* mob);

	void ExecuteStunGameplayCue(const APlayerCharacter* player);
};
