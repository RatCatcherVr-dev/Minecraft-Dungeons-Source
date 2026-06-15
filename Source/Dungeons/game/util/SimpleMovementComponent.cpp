
#include "Dungeons.h"

#include "SimpleMovementComponent.h"
#include "EngineDefines.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "UnrealNetwork.h"


USimpleMovementComponent::USimpleMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_t(0.0f)
	, m_waitTime(0.0f)
	, m_waitCount(0.0f)
	, m_NetUpdateRate(7.5f)
	, ServerPosition(FVector::ZeroVector)
	, RotationSpeed(0.0f)
{
	Velocity = FVector::ZeroVector;
	CurrentVelocity = FVector::ZeroVector;
}


FVector USimpleMovementComponent::ComputeMoveDelta(const FVector& velocity, float DeltaTime)
{
	// no need to check? disable and look for skippy graphics
	// use incoming velocity?
	FVector dx = velocity * DeltaTime;
	
	float erSq = 0.01f; // property? on walkpickupcomponent?
	FVector diff = ServerPosition != FVector::ZeroVector ? ServerPosition - UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
	
	// if moving, shouldn't correct back!
	if (diff.SizeSquared() > erSq && velocity.SizeSquared() < 1)
	{
		FVector pos = UpdatedComponent->GetComponentLocation();
		
		// scaling like VInterpTo:
		float flerp = FMath::Clamp(m_t * m_NetUpdateRate, 0.0f, 1.0f);
		dx += diff * flerp;
	}
	
	return dx;

}

bool USimpleMovementComponent::ShouldSkipUpdate(float DeltaTime) const
{
	if (FinishedWaiting())
		return Super::ShouldSkipUpdate(DeltaTime);
	else
		return true;
}

void USimpleMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USimpleMovementComponent, ServerPosition);
}

void USimpleMovementComponent::BeginPlay() {
	Super::BeginPlay();

	spawnTime = GetWorld()->GetTimeSeconds();
}

void USimpleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	ServerPosition = UpdatedComponent->GetComponentLocation();
	m_waitCount += DeltaTime;

	// skip if don't want component updated when not rendered or updated component can't move
	if (HasStoppedSimulation() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(UpdatedComponent))
	{
		return;
	}

	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner || !CheckStillInWorld())
	{
		return;
	}

	m_t += DeltaTime;
	float TimeTick = DeltaTime;

	//const float TimeTick = ShouldUseSubStepping() ? GetSimulationTimeStep(RemainingTime, Iterations) : RemainingTime;
	//RemainingTime -= TimeTick;

	FHitResult Hit(1.f);
	Hit.Time = 1.f;
	FVector OldVelocity = Velocity;
	
	if (bShouldAccelerate) {
		if (Velocity.IsZero()) {
			CurrentVelocity = Velocity;
		}
		else {
			

			float targetLength;
			FVector targetDirection;
			Velocity.ToDirectionAndLength(targetDirection, targetLength);

			if( !(CurrentVelocity - Velocity).IsNearlyZero() ) {
				const float currentLength = FMath::Min(targetLength, CurrentVelocity.Size() + (AccelerationMagnitude * DeltaTime * targetLength));
				CurrentVelocity = targetDirection * currentLength;
			}

			OldVelocity = CurrentVelocity;
		}
	}
	else {
		CurrentVelocity = Velocity;
	}

	const FVector MoveDelta = ComputeMoveDelta(OldVelocity, TimeTick);

	FQuat NewRotation = [&] {
		if (!Rotation) {
			return UpdatedComponent->GetComponentQuat();
		}
		
		if (RotationPendel) {
			return FQuat(FVector::UpVector, FMath::Sin(RotationSpeed * (GetWorld()->GetTimeSeconds() - spawnTime)) * PendelSpan * .5f);
		} else {
			return FQuat(FVector::UpVector, RotationSpeed / 180 * DeltaTime) * UpdatedComponent->GetComponentQuat();
		}
	}();	
		
	//const FQuat NewRotation = (bRotationFollowsVelocity && !OldVelocity.IsNearlyZero(0.01f)) ? OldVelocity.ToOrientationQuat() : UpdatedComponent->GetComponentQuat();

	SafeMoveUpdatedComponent(MoveDelta, NewRotation, bSweepOnMove, Hit);

	if (bShouldSlide) {		
		if (Hit.IsValidBlockingHit())
		{
			FVector Delta = CurrentVelocity * DeltaTime;
			HandleImpact(Hit, DeltaTime, Delta);
			// Try to slide the remaining distance along the surface.
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}
	}

	UpdateComponentVelocity();

	if (!lastPosition.IsZero()) {
		m_isMoving = !lastPosition.Equals(UpdatedComponent->GetComponentLocation(), 0.1f);
	}

	lastPosition = UpdatedComponent->GetComponentLocation();

}

void USimpleMovementComponent::OnRep_ServerPosition(FVector old)
{
	if (m_t != 0)
	{
		m_NetUpdateRate = 1 / m_t;
	}
	m_t = 0.0f;
}

void USimpleMovementComponent::UpdateComponentVelocity() {
	if (bShouldAccelerate) {
		if (UpdatedComponent) {
			UpdatedComponent->ComponentVelocity = CurrentVelocity;
		}
	}
	else {
		Super::UpdateComponentVelocity();
	}
}

void USimpleMovementComponent::StopSimulating(const FHitResult& HitResult)
{
	SetUpdatedComponent(NULL);
	Velocity = FVector::ZeroVector;
	CurrentVelocity = FVector::ZeroVector;
	OnProjectileStop.Broadcast(HitResult);
}

// todo: implement/test (from projectileMovementComponent)
bool USimpleMovementComponent::CheckStillInWorld()
{
	if (!UpdatedComponent)
	{
		return false;
	}

	const UWorld* MyWorld = GetWorld();
	if (!MyWorld)
	{
		return false;
	}

	// check the variations of KillZ
	AWorldSettings* WorldSettings = MyWorld->GetWorldSettings(true);
	if (!WorldSettings->bEnableWorldBoundsChecks)
	{
		return true;
	}
	AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!IsValid(ActorOwner))
	{
		return false;
	}
	if (ActorOwner->GetActorLocation().Z < WorldSettings->KillZ)
	{
		UDamageType const* DmgType = WorldSettings->KillZDamageType ? WorldSettings->KillZDamageType->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
		ActorOwner->FellOutOfWorld(*DmgType);
		return false;
	}
	// Check if box has poked outside the world
	else if (UpdatedComponent && UpdatedComponent->IsRegistered())
	{
		const FBox&	Box = UpdatedComponent->Bounds.GetBox();
		if (Box.Min.X < -HALF_WORLD_MAX || Box.Max.X > HALF_WORLD_MAX ||
			Box.Min.Y < -HALF_WORLD_MAX || Box.Max.Y > HALF_WORLD_MAX ||
			Box.Min.Z < -HALF_WORLD_MAX || Box.Max.Z > HALF_WORLD_MAX)
		{
			UE_LOG(LogDungeons, Warning, TEXT("%s is outside the world bounds!"), *ActorOwner->GetName());
			ActorOwner->OutsideWorldBounds();
			// not safe to use physics or collision at this point
			ActorOwner->SetActorEnableCollision(false);
			FHitResult Hit(1.f);
			StopSimulating(Hit);
			return false;
		}
	}
	return true;
}
