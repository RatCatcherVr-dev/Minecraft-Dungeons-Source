#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "FrozenSolidIceCubeAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UFrozenSolidIceCubeAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_OwnerInteractionMagnitude, Category = "Dungeons")
	float OwnerInteractionMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_OtherInteractionMagnitude, Category = "Dungeons")
	float OtherInteractionMagnitude = 1.f;

	DECLARE_ATTRIBUTE_FUNCTION(OwnerInteractionMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(OtherInteractionMagnitude);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:

	UFUNCTION()
	void OnRep_OwnerInteractionMagnitude();

	UFUNCTION()
	void OnRep_OtherInteractionMagnitude();
};
