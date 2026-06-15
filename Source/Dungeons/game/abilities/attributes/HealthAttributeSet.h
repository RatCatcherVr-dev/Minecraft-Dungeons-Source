#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "UnrealNetwork.h"
#include "game/difficulty/DifficultyStats.h"
#include "HealthAttributeSet.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Shield, Category = "Dungeons")
	float Shield = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Health, Category = "Dungeons")
	float Health = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxHealth, Category = "Dungeons")
	float MaxHealth = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ResistDeath, Category = "Dungeons")
	float ResistDeath = 0.f; // D11.DB
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TakeDamageMultiplier, Category = "Dungeons")
	float TakeDamageMultiplier = 1.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TakeMeleeDamageMultiplier, Category = "Dungeons")
	float TakeMeleeDamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TakeRangeDamageMultiplier, Category = "Dungeons")
	float TakeRangeDamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_TakeFallDamageMultiplier, Category = "Dungeons")
	float TakeFallDamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ArmorAttackItemPowerFactor, Category = "Dungeons")
	float ArmorAttackItemPowerFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ArmorHealingItemPowerFactor, Category = "Dungeons")
	float ArmorHealingItemPowerFactor = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ReceiveHealingMultiplier, Category = "Dungeons")
	float ReceiveHealingMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_HealthThreshold, Category = "Dungeons")
	float HealthThreshold = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float Endurance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float MaxEndurance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float EnduranceRecoveryPerSecond = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float StaggerMultiplier = 0.f;

	DECLARE_ATTRIBUTE_FUNCTION(Shield);
	DECLARE_ATTRIBUTE_FUNCTION(Health);
	DECLARE_ATTRIBUTE_FUNCTION(MaxHealth);
	DECLARE_ATTRIBUTE_FUNCTION(ResistDeath); // D11.DB
	DECLARE_ATTRIBUTE_FUNCTION(TakeDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(TakeMeleeDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(TakeRangeDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(TakeFallDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(ArmorAttackItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(ArmorHealingItemPowerFactor);
	DECLARE_ATTRIBUTE_FUNCTION(Endurance);
	DECLARE_ATTRIBUTE_FUNCTION(MaxEndurance);
	DECLARE_ATTRIBUTE_FUNCTION(EnduranceRecoveryPerSecond);
	DECLARE_ATTRIBUTE_FUNCTION(StaggerMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(ReceiveHealingMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(HealthThreshold);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	*	Called just before modifying the value of an attribute. AttributeSet can make additional modifications here. Return true to continue, or false to throw out the modification.
	*	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	*/
	bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;

	/**
	*	Called just before a GameplayEffect is executed to modify the base value of an attribute. No more changes can be made.
	*	Note this is only called during an 'execute'. E.g., a modification to the 'base value' of an attribute. It is not called during an application of a GameplayEffect, such as a 5 second +10 movement speed buff.
	*/
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;	

	/**
	*	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
	*	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
	*	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	*
	*	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	*/
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void ApplyThreshold(float& NewValue) const;

	/**
	*	This is called just before any modification happens to an attribute's base value when an attribute aggregator exists.
	*	This function should enforce clamping (presuming you wish to clamp the base value along with the final value in PreAttributeChange)
	*	This function should NOT invoke gameplay related events or callbacks. Do those in PreAttributeChange() which will be called prior to the
	*	final value of the attribute actually changing.
	*/
	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
protected:

	UFUNCTION()
	void OnRep_Shield();

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_MaxHealth();

	UFUNCTION()
	void OnRep_ResistDeath(); // D11.DB

	UFUNCTION()
	void OnRep_TakeDamageMultiplier();

	UFUNCTION()
	void OnRep_TakeMeleeDamageMultiplier();

	UFUNCTION()
	void OnRep_TakeRangeDamageMultiplier();

	UFUNCTION()
	void OnRep_TakeFallDamageMultiplier();

	UFUNCTION()
	void OnRep_ArmorAttackItemPowerFactor();

	UFUNCTION()
	void OnRep_ArmorHealingItemPowerFactor();

	UFUNCTION()
	void OnRep_ReceiveHealingMultiplier();

	UFUNCTION()
	void OnRep_HealthThreshold();

private:	
	bool IsDamageApplied(FGameplayEffectModCallbackData& data) const;
	float CalculateDamageDealt(FGameplayEffectModCallbackData& data) const;	
	
	static float GetDamageTypeMultiplier(FGameplayEffectModCallbackData& data, const ABaseCharacter* ownerCharacter);
	
	bool CanApplyDamage(struct FGameplayEffectModCallbackData &data) const;
	static bool CanDamageHurtFriends(const FGameplayEffectModCallbackData& data);
	bool CanApplyDamageToTarget(const FGameplayEffectModCallbackData& data) const;
	
	static bool IsAttackMissed(FGameplayEffectModCallbackData& data, const ABaseCharacter* ownerCharacter);
	
	bool ShouldSavePlayer(float damage) const;

	void ModifyDamageMagnitude(FGameplayEffectModCallbackData& data, float healthReduction) const;

	bool IsHealingApplied(FGameplayEffectModCallbackData& data) const;
	
	static bool HasReviveTag(FGameplayEffectModCallbackData& data);

	game::CachedDifficultyStats mDifficultyStats;
};
