// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "lovika/world/level/ChunkBlockSource.h"
#include "MoveToTargetMovementComponent.generated.h"

USTRUCT()
struct DUNGEONS_API FArcMovementInfo {
	GENERATED_BODY()

	FArcMovementInfo();
	FArcMovementInfo(float duration, float maxZ, FVector startPos, FVector endPos);

	float Duration;
	float MaxZOffset;

	FVector StartPosition;
	FVector EndPosition;
};

UCLASS()
class DUNGEONS_API UArcMovement : public UObject {
	GENERATED_BODY()
public:
	UArcMovement();
	void TryStart(FArcMovementInfo info);

	const FVector GetCurrentPosition() const;
private:
	bool bIsStarted = false;
	float StartTime;
	FArcMovementInfo Info;
};

UCLASS()
class DUNGEONS_API UMoveToTargetMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
public:
	UMoveToTargetMovementComponent();


	void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	bool bArcMovement = false;

	UPROPERTY(EditAnywhere)
	bool bGridMovement = false;

	UPROPERTY(EditAnywhere)
	FVector mTargetOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere)
	float TargetTolerance = 0.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bArcMovement"))
	float Acceleration = 100.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bArcMovement"))
	float MaxSpeed = 100.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "!bArcMovement"))
	float MinSpeed = 0.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bArcMovement"))
	float ArcDuration = 2.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bArcMovement"))
	float ArcMaxZOffset = 200.0f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bGridMovement"))
	float GridTolerance = 20.f;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bGridMovement"))
	float LerpStrength = 8.0f;

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* target);
	UFUNCTION(BlueprintCallable)
	void Reset();
	
	float GetMaxSpeed() const override;
	float GetMinSpeed() const;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
private:
	void ApplyNormalMovement(float deltaTime);
	void ApplyGridMovement(float deltaTime);
	void ApplyArcedMovement();

	bool CanChangeDirection(FVector position);

	TWeakObjectPtr<AActor> Target;

	UPROPERTY()
	UArcMovement* ArcMovement;

	FVector mStoredDelta;
	ChunkBlockSource* blockSource;

	enum class Axis{
		x,
		y,
		z
	};

	Axis currentAxis();
};
