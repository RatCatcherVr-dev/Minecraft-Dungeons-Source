#include "DungeonsProjectileMovementComp.h"
#include "Vector.h"
#include "NoExportTypes.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY_STATIC(LogDungeonsProjectileMovement, Log, All);

void UDungeonsProjectileMovementComp::SetUseUnderWaterPhysics(bool useUnderWaterPhysics)
{
	bUseUnderWaterPhysics = useUnderWaterPhysics;
}

void UDungeonsProjectileMovementComp::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	TimeSinceLaunched += DeltaTime;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsSinking && IsSinking())
	{
		SinkStartTime = TimeSinceLaunched;
		bIsSinking = true;
		OnProjectileStartsSinking.Broadcast();
	}
}

void UDungeonsProjectileMovementComp::StoreInitialVelocity()
{
	InitialLaunchVector = Velocity.GetUnsafeNormal();
	InitialLaunchSpeed = Velocity.Size();
}

FVector UDungeonsProjectileMovementComp::ComputeVelocity(FVector InitialVelocity, float DeltaTime) const
{
	if (bUseUnderWaterPhysics && TimeSinceLaunched >= TimeToKickDragIn)
	{
		const float TimeSinceDragStart = TimeSinceLaunched - TimeToKickDragIn;

		//v = v0/ (1 + (t * 0.5 * A * C * rho * v0) ).
		const float NewSpeed = InitialLaunchSpeed / (1 + (TimeSinceDragStart * 0.5f * 0.01f * Drag * InitialLaunchSpeed));

		FVector NewVelocity = NewSpeed * InitialLaunchVector;		
		if (bIsSinking) 
		{
			const float fVelocityDrop = TimeSinceLaunched - SinkStartTime;
			NewVelocity.Z -= (FallRate * fVelocityDrop);
		}

		return NewVelocity;
	}

	return Super::ComputeVelocity(InitialVelocity, DeltaTime);
}

void UDungeonsProjectileMovementComp::ResetComponent()
{
	bIsSinking = false;
	TimeSinceLaunched = 0.0f;	
	SinkStartTime = 0.0f;
}

bool UDungeonsProjectileMovementComp::IsSinking() const
{
	float value = Velocity.SizeSquared2D();
	return  value <= SpeedToleranceToSink;
}
