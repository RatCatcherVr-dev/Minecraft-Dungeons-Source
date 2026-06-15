#pragma once

#include "CoreMinimal.h"
#include "game/actor/character/CharacterAnimInstance.h"
#include "PerfectFormAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EPerfectFormAnimLocomotion: uint8 {
	Init,
	Idle,
	WalkRun,
	WalkReverse,
	Charge,
	Bombspew,
	Scatter,
	Feast,
	Submerge,
	Voidpull,
	Laser,
	DownedDead,
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UPerfectFormAnimInstance : public UCharacterAnimInstance
{
	GENERATED_BODY()

public:
	void NativeUpdateAnimation(float) override;

	UPROPERTY(transient, BlueprintReadWrite, Category = "Dungeons|Animation")
	EPerfectFormAnimLocomotion PerfectFormLocomotionBlend = EPerfectFormAnimLocomotion::Init;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons|Animation")
	float laserBodyRotationRate = 1.0f;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons|Animation")
	float turnAngle = 0.f;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons|Animation")
	float rotationTime = 0.f;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	bool IsTurning() const;

private:
	void UpdateTurnAngle();
	void UpdateRotationTime(float);
	float GetTurnAngleDelta() const;
	bool ShouldUpdateTurnAngle()const;

	float lastUpdateYaw = 0.f;

	TWeakObjectPtr<class AMobCharacter> owningMobCharacter;
	TWeakObjectPtr<class UMobCharacterMovementComponent> mobMovementComponent;
};
