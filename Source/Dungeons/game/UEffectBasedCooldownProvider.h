

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CooldownProvider.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "AbilitySystemInterface.h"
#include "UEffectBasedCooldownProvider.generated.h"

/**
 * 
 */
UCLASS(DefaultToInstanced, BlueprintType)
class DUNGEONS_API UEffectBasedCooldownProvider : public UObject, public ICooldownProvider
{
	GENERATED_BODY()

	IAbilitySystemInterface* AbilitySystemProvider = nullptr;

	UAbilitySystemComponent* GetAbilitySystem() const;
	
	void PostRename(UObject* OldOuter, const FName OldName) override;
	void PostInitProperties() override;
public:
	UEffectBasedCooldownProvider(const FObjectInitializer& ObjectInitializer);

	
	bool IsOnCooldown() const override;

	UFUNCTION()
	float GetCooldownSecondsRemaining() const  override;
	UFUNCTION()
	float GetCooldownFractionRemaining() const  override;
	UFUNCTION()
	float GetCooldownSecondsDuration() const  override;

	TPair<float, float> GetCooldownSecondsRemainingAndDuration() const override;

	void TriggerCooldown(float seconds, FPredictionKey key = FPredictionKey()) override;
	//Modifies the start time of the cooldown
	void ModifyCooldown(float offsetSeconds) override;
	void ResetCooldown() override;

	void TriggerCooldownWithSpec(FGameplayEffectSpec& spec,  FPredictionKey key = FPredictionKey());

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CooldownTag;
};


UCLASS()
class DUNGEONS_API UCooldownGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCooldownGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

