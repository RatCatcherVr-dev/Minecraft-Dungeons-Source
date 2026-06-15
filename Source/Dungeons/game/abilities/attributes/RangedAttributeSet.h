#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "RangedAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API URangedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackItemPowerFactor, Category = "Dungeons")
	float RangedAttackItemPowerFactor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedHealItemPowerFactor, Category = "Dungeons")
	float RangedHealItemPowerFactor = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackDamageMultipler, Category = "Dungeons")
	float RangedAttackDamageMultipler = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackSpeedMultiplier, Category = "Dungeons")
	float RangedAttackSpeedMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackPushbackMultiplier, Category = "Dungeons")
	float RangedAttackPushbackMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackLifeStealAmount, Category = "Dungeons")
	float RangedAttackLifeStealAmount = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackHitChanceMultiplier, Category = "Dungeons")
	float RangedAttackHitChanceMultiplier = 1.f;
	//Adds a flat amount of damage to base damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RangedAttackDamageIncrease, Category = "Dungeons")
	float RangedAttackDamageIncrease = 0.f;

	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(RangedHealItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackDamageMultipler);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackSpeedMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackPushbackMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackLifeStealAmount);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackHitChanceMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(RangedAttackDamageIncrease);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_RangedAttackItemPowerFactor();
	UFUNCTION()
	void OnRep_RangedHealItemPowerFactor();
	UFUNCTION()
	void OnRep_RangedAttackDamageMultipler();
	UFUNCTION()
	void OnRep_RangedAttackSpeedMultiplier();
	UFUNCTION()
	void OnRep_RangedAttackPushbackMultiplier();
	UFUNCTION()
	void OnRep_RangedAttackLifeStealAmount();
	UFUNCTION()
	void OnRep_RangedAttackHitChanceMultiplier();
	UFUNCTION()
	void OnRep_RangedAttackDamageIncrease();
};
