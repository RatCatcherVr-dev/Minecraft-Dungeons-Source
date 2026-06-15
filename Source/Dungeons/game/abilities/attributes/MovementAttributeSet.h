#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "MovementAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_SpeedMultiplier, Category = "Dungeons")
	float SpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_FrictionMultiplier, Category = "Dungeons")
	float FrictionMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_RotationMultiplier, Category = "Dungeons")
	float RotationMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DodgeCooldown, Category = "Dungeons")
	float DodgeCooldown = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DodgeSpeed, Category = "Dungeons")
	float DodgeSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DodgeCharges, Category = "Dungeons")
	float DodgeCharges = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxDodgeCharges, Category = "Dungeons")
	float MaxDodgeCharges = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Gravity, Category = "Dungeons")
	float Gravity = 1.0f;

	DECLARE_ATTRIBUTE_FUNCTION(SpeedMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(FrictionMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(RotationMultiplier);
	DECLARE_ATTRIBUTE_FUNCTION(DodgeCooldown);
	DECLARE_ATTRIBUTE_FUNCTION(DodgeSpeed);
	DECLARE_ATTRIBUTE_FUNCTION(DodgeCharges);
	DECLARE_ATTRIBUTE_FUNCTION(MaxDodgeCharges);
	DECLARE_ATTRIBUTE_FUNCTION(Gravity);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:

	UFUNCTION()
	void OnRep_SpeedMultiplier();

	UFUNCTION()
	void OnRep_FrictionMultiplier();

	UFUNCTION()
	void OnRep_RotationMultiplier();

	UFUNCTION()
	void OnRep_DodgeCooldown();

	UFUNCTION()
	void OnRep_DodgeSpeed();

	UFUNCTION()
	void OnRep_DodgeCharges();

	UFUNCTION()
	void OnRep_MaxDodgeCharges();

	UFUNCTION()
	void OnRep_Gravity();
};
