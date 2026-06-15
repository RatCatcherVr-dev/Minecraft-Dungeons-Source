// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "DamageAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDamageAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DamageMultiplier, Category = "Dungeons")
	float DamageMultiplier = 1.f;

	DECLARE_ATTRIBUTE_FUNCTION(DamageMultiplier);
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_DamageMultiplier();
};
