// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "MobCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UMobCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	void BeginPlay() override;
public:

	UMobCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	/** Change in rotation per second when pathfollowing. Used when UseControllerDesiredRotation or OrientRotationToMovement are true. Set a negative value for infinite rotation rate and instant turns. */
	UPROPERTY(Category = "Character Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	FRotator PathFollowingRotationRate = FRotator(0, 360.f, 0);

	void TogglePathFollowing(bool following);
	void ToggleIsInCooldown(bool inCooldown);
	void OnRegister() override;


	/**
	 * Sweeps a vertical trace to find the floor for the capsule at the given location. Will attempt to perch if ShouldComputePerchResult() returns true for the downward sweep result.
	 * No floor will be found if collision is disabled on the capsule!
	 *
	 * @param CapsuleLocation		Location where the capsule sweep should originate
	 * @param OutFloorResult		[Out] Contains the result of the floor check. The HitResult will contain the valid sweep or line test upon success, or the result of the sweep upon failure.
	 * @param bCanUseCachedLocation If true, may use a cached value (can be used to avoid unnecessary floor tests, if for example the capsule was not moving since the last test).
	 * @param DownwardSweepResult	If non-null and it contains valid blocking hit info, this will be used as the result of a downward sweep test instead of doing it as part of the update.
	 */
	virtual void FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult = NULL) const override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void SetBlockRotationRateChange(bool bBlock);

	virtual void PhysWalking(float DeltaTime, int32 Iterations) override;

	/** Handle a blocking impact. Calls ApplyImpactPhysicsForces for the hit, if bEnablePhysicsInteraction is true. */
	virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

private:

	mutable int		mCachedFloorDelay = 0;
	mutable int		mCachedFloorX = -1;
	mutable int		mCachedFloorY = -1;

	int				mBlockedByMobCountDown = 0;

	FRotator DefaultRotationRateCache;
	bool IsPathFollowing = false;
	bool IsInCooldown = false;
	bool bBlockRotationRateChange = false;
	void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	void RequestPathMove(const FVector& MoveInput) override;
	void StopActiveMovement() override;
	void RefreshRotationRate();

	void OnGravityAttributeChanged(const FOnAttributeChangeData& data);
};