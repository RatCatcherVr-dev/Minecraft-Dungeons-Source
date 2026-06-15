// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "BurstBowstring.generated.h"

class UArrowItemSlot;

UCLASS()
class DUNGEONS_API URollingFortressGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URollingFortressGameplayEffect();

	static const FName DamageReductionKey;
};

UCLASS()
class DUNGEONS_API UBurstBowstring : public UEnchantment
{
	GENERATED_BODY()
public:
	UBurstBowstring();

protected:
	void OnStart() override;

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;
	void OnDodgeRollEnd(FPredictionKey);

	void OnBeforeRangedAttack(AActor * attackTarget, bool &attackDenied, FPredictionKey key) override;

	void TriggerAttack(int RemainingAttacks, TArray<TWeakObjectPtr<ABaseCharacter>> PotentialTargets);

	void EndAttack();

	FText CreateDescription() const override;

	void OnDeath();
	
	//Attacks gained per level past level 1
	UPROPERTY(EditDefaultsOnly)
	int32 AdditionalAttacksPerLevel = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 AttacksAtLevelOne = 1;

	//Damage dealt per attack
	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 0.4f;

	//The radius in which we look for targets
	UPROPERTY(EditDefaultsOnly)
	float TargetGatheringRadius = 1200.f;

	UPROPERTY(EditDefaultsOnly)
	float TimeBetweenAttacks = 0.05f;

	FTimerHandle AttackTriggerHandle;
	FActiveGameplayEffectHandle EffectHandle;

	bool bHasSuccesfullyAttacked = false;
	bool bShouldTriggerCue = false;
};
