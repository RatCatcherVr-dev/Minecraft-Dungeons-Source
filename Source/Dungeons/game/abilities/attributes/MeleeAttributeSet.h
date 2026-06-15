#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "MeleeAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UMeleeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	/** The base damage increase contributed by the item power of the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackItemPowerFactor, Category = "Dungeons")
	float MeleeAttackItemPowerFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeHealItemPowerFactor, Category = "Dungeons")
	float MeleeHealItemPowerFactor = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackDamageMultiplier, Category = "Dungeons")
	float MeleeAttackDamageMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackSpeedMultiplier, Category = "Dungeons")
	float MeleeAttackSpeedMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackAngleMultiplier, Category = "Dungeons")
	float MeleeAttackAngleMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackRangeMultiplier, Category = "Dungeons")
	float MeleeAttackRangeMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackPushbackMultiplier, Category = "Dungeons")
	float MeleeAttackPushbackMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackLifeStealAmount, Category = "Dungeons")
	float MeleeAttackLifeStealAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackHitChanceMultiplier, Category = "Dungeons")
	float MeleeAttackHitChanceMultiplier = 1.f;
	
	//Adds a flat amount of damage to base damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MeleeAttackDamageIncrease, Category = "Dungeons")
	float MeleeAttackDamageIncrease = 0.f;

	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeHealItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackSpeedMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackAngleMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackRangeMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackPushbackMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackLifeStealAmount);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackHitChanceMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(MeleeAttackDamageIncrease);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_MeleeAttackItemPowerFactor();

	UFUNCTION()
	void OnRep_MeleeHealItemPowerFactor();
	
	UFUNCTION()
	void OnRep_MeleeAttackSpeedMultiplier();
	
	UFUNCTION()
	void OnRep_MeleeAttackDamageMultiplier();
	
	UFUNCTION()
	void OnRep_MeleeAttackAngleMultiplier();
	
	UFUNCTION()
	void OnRep_MeleeAttackRangeMultiplier();
	
	UFUNCTION()
	void OnRep_MeleeAttackPushbackMultiplier();
	
	UFUNCTION()
	void OnRep_MeleeAttackLifeStealAmount();
	
	UFUNCTION()
	void OnRep_MeleeAttackHitChanceMultiplier();

	UFUNCTION()
	void OnRep_MeleeAttackDamageIncrease();
};
