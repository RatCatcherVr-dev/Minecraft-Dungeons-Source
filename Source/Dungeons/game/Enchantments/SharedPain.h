#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "SharedPain.generated.h"

UCLASS()
class DUNGEONS_API USharedPainDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USharedPainDamageGameplayEffect();
};


UCLASS()
class DUNGEONS_API USharedPain : public UEnchantment
{
	GENERATED_BODY()

public:
	USharedPain();

		float DamageOverflow = 0.0f;
		bool bCanTriggerSharedPain = false;
	UPROPERTY(EditDefaultsOnly)
		float MobsRange = 3000.0f;
	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) override;
	void OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext window) override;
	void OnBeforeMeleeAttack(AActor* attackTarget, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;

};