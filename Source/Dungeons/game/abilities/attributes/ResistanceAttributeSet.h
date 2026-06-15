#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "ResistanceAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UResistanceAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SlowResistanceMagnitude, Category = "Dungeons")
	float SlowResistanceMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_WindResistanceMagnitude, Category = "Dungeons")
	float WindResistanceMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentResistanceMagnitude, Category = "Dungeons")
	float CurrentResistanceMagnitude = 1.f;
	// bad OnRep update function? from when?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PushbackResistanceMagnitude, Category = "Dungeons")
	float PushbackResistanceMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_EnvironmentalProtectionMagnitude, Category = "Dungeons")
	float EnvironmentalProtectionMagnitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_PushbackZClampMagnitude, Category = "Dungeons")
	float PushbackZClampMagnitude = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_FallResistanceMagnitude, Category = "Dungeons")
	float FallResistanceMagnitude = 1.f;

	DECLARE_ATTRIBUTE_FUNCTION(SlowResistanceMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(WindResistanceMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(CurrentResistanceMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(PushbackResistanceMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(EnvironmentalProtectionMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(PushbackZClampMagnitude);
	DECLARE_ATTRIBUTE_FUNCTION(FallResistanceMagnitude);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:

	UFUNCTION()
	void OnRep_SlowResistanceMagnitude();

	UFUNCTION()
	void OnRep_WindResistanceMagnitude();

	UFUNCTION()
	void OnRep_CurrentResistanceMagnitude();

	UFUNCTION()
	void OnRep_PushbackResistanceMagnitude();

	UFUNCTION()
	void OnRep_EnvironmentalProtectionMagnitude();

	UFUNCTION()
	void OnRep_PushbackZClampMagnitude();
	UFUNCTION()
	void OnRep_FallResistanceMagnitude();
};
