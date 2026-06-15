#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include <GameplayEffectExtension.h>
#include "ItemAttributeSet.generated.h"

DECLARE_DELEGATE_OneParam(FOnAttemptChangeSouls, const FGameplayEffectModCallbackData&);

UCLASS()
class DUNGEONS_API UItemAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ItemCooldownMultiplier, Category = "Dungeons")
	float ItemCooldownMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ItemDamageMultiplier, Category = "Dungeons")
	float ItemDamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SoulGathering, Category = "Dungeons")
	float SoulGathering = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SoulGatheringMultiplier, Category = "Dungeons")
	float SoulGatheringMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SoulSpawnMultiplier, Category = "Dungeons")
	float SoulSpawnMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_ItemLifeStealAmount, Category = "Dungeons")
	float ItemLifeStealAmount = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PotionCooldownMultiplier, Category = "Dungeons")
	float PotionCooldownMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Souls, Category = "Dungeons")
	float Souls = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxSouls, Category = "Dungeons")
	float MaxSouls = 300.f;

	DECLARE_ATTRIBUTE_FUNCTION(ItemCooldownMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(ItemDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(SoulGathering);
	DECLARE_ATTRIBUTE_FUNCTION(SoulGatheringMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(SoulSpawnMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(ItemLifeStealAmount);
	DECLARE_ATTRIBUTE_FUNCTION(PotionCooldownMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(Souls);
	DECLARE_ATTRIBUTE_FUNCTION(MaxSouls);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOnAttemptChangeSouls OnAttemptChangeSouls;

protected:

	UFUNCTION()
	void OnRep_ItemCooldownMultiplier();
	UFUNCTION()
	void OnRep_ItemDamageMultiplier();
	UFUNCTION()
	void OnRep_SoulGathering();
	UFUNCTION()
	void OnRep_SoulGatheringMultiplier();
	UFUNCTION()
	void OnRep_SoulSpawnMultiplier();
	UFUNCTION()
	void OnRep_ItemLifeStealAmount();
	UFUNCTION()
	void OnRep_PotionCooldownMultiplier();
	UFUNCTION()
	void OnRep_Souls();
	UFUNCTION()
	void OnRep_MaxSouls();
};
