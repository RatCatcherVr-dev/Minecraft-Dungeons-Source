#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "DifficultyAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UDifficultyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DifficultyDealDamageMultiplier, Category = "Dungeons")
	float DifficultyDealDamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DifficultyPerformHealingMultiplier, Category = "Dungeons")
	float DifficultyPerformHealingMultiplier = 1.f;
	
	DECLARE_ATTRIBUTE_FUNCTION(DifficultyDealDamageMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(DifficultyPerformHealingMultiplier);
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_DifficultyDealDamageMultiplier();

	UFUNCTION()
	void OnRep_DifficultyPerformHealingMultiplier();
};
