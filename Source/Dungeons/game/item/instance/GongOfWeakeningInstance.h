#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/WeakenGameplayEffect.h"
#include "GongOfWeakeningInstance.generated.h"

UCLASS()
class DUNGEONS_API AGongOfWeakeningInstance : public AItemInstance
{
	GENERATED_BODY()

	AGongOfWeakeningInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float EnemyDealDamageMultiplier = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float EnemyTakeDamageMultiplier = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FGameplayTag TriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	int32 WeakenRange = 400;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UWeakenGameplayEffect> Effect;
private:
	void ExecuteWeakeningGameplayCue(const ABaseCharacter* ownerCharacter, float calculatedRange) const;
	
	void ApplyEffectToMobs(const ABaseCharacter* ownerCharacter, float calculatedRange, const FPredictionKey& predictionKey) const;
	FGameplayEffectSpec CreateWeakenEffectSpec(UAbilitySystemComponent* abilitySystem) const;
	TArray<class AMobCharacter*> GetMobsToTarget(const ABaseCharacter* ownerCharacter, int32 range) const;
};
