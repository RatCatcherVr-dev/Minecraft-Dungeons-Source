
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/MovementComponent.h"
#include "SimpleMovementComponent.generated.h"

/**
* Simple MovementComponent for synching network movement with specified velocities and server position.
*
* @see UMovementComponent
*/

UCLASS(ClassGroup = Movement, meta = (BlueprintSpawnableComponent), ShowCategories = (Velocity))
class DUNGEONS_API USimpleMovementComponent : public UMovementComponent
{
	GENERATED_UCLASS_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileStopDelegate, const FHitResult&, ImpactResult);


	/** If true, simple bounces will be simulated. Set this to false to stop simulating on contact. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileBounces)
	uint32 bShouldBounce : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldSlide = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShouldAccelerate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSweepOnMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bShouldAccelerate"))
	float AccelerationMagnitude = 2;

	virtual void BeginPlay() override;

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	//End UActorComponent Interface


	/**
	* This will check to see if the projectile is still in the world.  It will check things like
	* the KillZ, outside world bounds, etc. and handle the situation.
	*/
	virtual bool CheckStillInWorld();

	
	/** Clears the reference to UpdatedComponent, fires stop event (OnProjectileStop), and stops ticking (if bAutoUpdateTickRegistration is true). */
	UFUNCTION(BlueprintCallable, Category = "Game|Components|ProjectileMovement")
	virtual void StopSimulating(const FHitResult& HitResult);

	bool HasStoppedSimulation() { return (UpdatedComponent == nullptr) || (bIsActive == false); }


	virtual bool ShouldSkipUpdate(float DeltaTime) const override;

	/** Called when projectile has come to a stop (velocity is below simulation threshold, bounces are disabled, or it is forcibly stopped). */
	UPROPERTY(BlueprintAssignable)
	FOnProjectileStopDelegate OnProjectileStop;

	void SetWaitTime(float time) { if (time > 0) m_waitTime = time; }

	UFUNCTION()
	void OnRep_ServerPosition(FVector old);

	UFUNCTION()
	void SetRotationSpeed(float rotationSpeed, float pendelSpan = HALF_PI) {
		RotationSpeed = rotationSpeed;
		PendelSpan = pendelSpan;
		Rotation = !FMath::IsNearlyZero(RotationSpeed);
	}

	void UpdateComponentVelocity() override;

	// #D11.CM - Returns if the movement component is actually moving or not.
	// I.E. If movement was blocked or if we were successful last frame.
	UFUNCTION(BlueprintCallable)
	bool IsComponentMoving() { return m_isMoving; };

protected:
	virtual FVector ComputeMoveDelta(const FVector& velocity, float DeltaTime);

protected:
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ServerPosition)
	FVector ServerPosition;

	bool FinishedWaiting() const { return m_waitCount > m_waitTime; }

	FVector lastPosition = FVector::ZeroVector;
	bool m_isMoving = false;

	float m_t;
	float m_waitCount;
	float m_waitTime = 0.0f;
	float m_NetUpdateRate;

	float RotationSpeed;
	bool Rotation = false;
	bool RotationPendel = true;
	float PendelSpan = HALF_PI;

	/** Minimum delta time considered when ticking. Delta times below this are not considered. This is a very small non-zero positive value to avoid potential divide-by-zero in simulation code. */
	static const float MIN_TICK_TIME;

private:
	float spawnTime;

	FVector CurrentVelocity;
};

