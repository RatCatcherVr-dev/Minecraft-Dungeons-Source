// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EAnimLocomotion: uint8 {
	Idle,
	WalkRun,
	Flying,
	BlendSpace,
	Falling,
	DownedDead,
	Gliding,
	Diving
};

USTRUCT(BlueprintType)
struct FAnimGraphStateConditions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(transient, BlueprintReadWrite, Category = "Dungeons")
	EAnimLocomotion FallBackLocomotionBlend;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	FVector LocomotionVelocity;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	float LocomotionWalkRatio;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	float RelativeMoveSpeed;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	float LocomotionWalkVariedPlayRate;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	FVector LookAtPoint;

	UPROPERTY(transient, BlueprintReadOnly, Category = "Dungeons")
	float LookAtAlpha;
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float) override;
	
	void NativePostEvaluateAnimation() override;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	class ABaseCharacter* GetBaseCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	class AMobCharacter* GetMobCharacter() const;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	class APlayerCharacter* GetPlayerCharacter() const;
	
	void SetLookAtActor(AActor*);
	
	void ClearLookAtActor();

	UPROPERTY(transient, BlueprintReadWrite, Category = "Dungeons|Animation")
	FAnimGraphStateConditions StateMachineConditions;

private:
	float LocomotionAnimSpeedMultiplier;
	TWeakObjectPtr<AActor> LookAtActor;
};
