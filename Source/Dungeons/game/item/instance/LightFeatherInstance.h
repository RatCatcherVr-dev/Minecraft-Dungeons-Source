#pragma once

#include "game/item/instance/AItemInstance.h"
#include <GameplayPrediction.h>
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/actor/item/CustomMoverItemInstance.h"
#include "LightFeatherInstance.generated.h"

class ABaseCharacter;
class AMobCharacter;

UCLASS()
class DUNGEONS_API ALightFeatherInstance : public ACustomMoverItemInstance {
	GENERATED_BODY()

	ALightFeatherInstance();

	int GetDisplayCount() const override;
	void Activate(const FPredictionKey& predictionKey) override;

	bool CanActivate() const override;

	UFUNCTION()
	void OnPlayerLanded();

	float GetStats(EItemStats stat) const;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UStaggerGameplayEffect> Effect = UStaggerGameplayEffect::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	UParticleSystem *LandingEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	USoundCue *LandingSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FPushback MobPush;;

	UPROPERTY(EditDefaultsOnly)
	float StunDurationSeconds = 2.f;

private:
	mutable FPredictionKey CurrentPrediction;
	
	void ExecuteLightFeatherJumpGameplayCue(const ABaseCharacter* owner) const;

	void ApplyStunEffectToMobs(const ABaseCharacter* owner) const;
	TArray<AMobCharacter*> GetMobsToTarget(const ABaseCharacter* player) const;
	FGameplayEffectSpec CreateStunEffectSpec(const UAbilitySystemComponent* abilitySystem) const;
	float CalculateStunDuration() const;

	void ExecuteLightFeatherLandingGameplayCue(const ABaseCharacter* owner) const;

	FDelegateHandle onLandedHandle;
};
