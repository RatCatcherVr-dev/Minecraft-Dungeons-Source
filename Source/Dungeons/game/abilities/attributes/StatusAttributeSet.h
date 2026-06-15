#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "StatusAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UStatusAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PositiveStatusDurationMagnitude, Category = "Dungeons")
	float PositiveStatusDurationMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_NegativeStatusDurationMagnitude, Category = "Dungeons")
	float NegativeStatusDurationMagnitude = 1.f;

	DECLARE_ATTRIBUTE_FUNCTION(PositiveStatusDurationMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(NegativeStatusDurationMagnitude);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:

	UFUNCTION()
	void OnRep_PositiveStatusDurationMagnitude();

	UFUNCTION()
	void OnRep_NegativeStatusDurationMagnitude();
};
