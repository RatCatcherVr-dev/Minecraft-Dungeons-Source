// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "MoveToTargetMovementComponent.h"
#include "world/level/BlockPos.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "lovika/Interpolators2.h"

UMoveToTargetMovementComponent::UMoveToTargetMovementComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = false;
}

void UMoveToTargetMovementComponent::BeginPlay() {
	Super::BeginPlay();

	if (bArcMovement) {
		ArcMovement = NewObject<UArcMovement>(this);
	}
	if (bGridMovement)
	{
		auto Game = actorquery::getFirstActor<AGameBP>(GetWorld());
		blockSource = Game->BlockSource();
	}
}

void UMoveToTargetMovementComponent::SetTarget(AActor* target) {
	Target = target;
	Activate();
}

void UMoveToTargetMovementComponent::Reset() {
	Target.Reset();
	Velocity = FVector::ZeroVector;
	Deactivate();
}

float UMoveToTargetMovementComponent::GetMinSpeed() const {
	return MinSpeed;
}


float UMoveToTargetMovementComponent::GetMaxSpeed() const {
	return MaxSpeed;
}

void UMoveToTargetMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	if (!Target.IsValid()) {
		Reset();
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bArcMovement) {
		ApplyArcedMovement();
	}
	else {
		if (bGridMovement)
		{
			ApplyGridMovement(DeltaTime);
		}
		else
		{
			ApplyNormalMovement(DeltaTime);
		}
	}
}

void UMoveToTargetMovementComponent::ApplyNormalMovement(float deltaTime) {
	const auto& targetLocation = Target->GetActorLocation() + mTargetOffset;
	const auto& soulLocation = UpdatedComponent->GetComponentLocation();

	const auto delta = targetLocation - soulLocation;

	if (delta.IsNearlyZero(TargetTolerance)) {
		return;
	}

	float scalarVelocity = FMath::Max(GetMinSpeed(), Velocity.Size());

	if (scalarVelocity < GetMaxSpeed()) {
		scalarVelocity = FMath::Min(scalarVelocity + Acceleration * deltaTime, GetMaxSpeed());
	}
	const auto scalarDelta = delta.Size();
	auto normalizedDelta = delta;
	normalizedDelta.Normalize();

	Velocity = normalizedDelta * scalarVelocity;

	FHitResult Hit(1.f);
	Hit.Time = 1.f;
	SafeMoveUpdatedComponent(Velocity * deltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);
	UpdateComponentVelocity();
}

void UMoveToTargetMovementComponent::ApplyGridMovement(float deltaTime) {
	const auto& targetLocation = Target->GetActorLocation() + mTargetOffset;
	const auto& soulLocation = UpdatedComponent->GetComponentLocation();

	if (mStoredDelta.IsNearlyZero(TargetTolerance))
	{
		mStoredDelta = targetLocation - soulLocation;
	}

	if (CanChangeDirection(soulLocation))
	{
		Axis currentDirection = currentAxis();
		bool firstPass = true;

		const auto delta = targetLocation - soulLocation;

		float absX = abs(delta.X);
		float absY = abs(delta.Y);
		float absZ = abs(delta.Z);

		for (int i = 0; i < 4; i++)
		{
			FVector offset(0, 0, 0);
			if (firstPass) //prefer to continue in current direction, so we'll check that first
			{
				firstPass = false;
				switch (currentDirection)
				{
				case UMoveToTargetMovementComponent::Axis::x:
					if ((mStoredDelta.X > 0 && delta.X < 0) || (mStoredDelta.X < 0 && delta.X > 0)) //block u-turns
					{
						absX = 0;
						break;
					}
					if (abs(mStoredDelta.X - delta.X) > 100)
					{
						offset.X += delta.X;
					}
					break;
				case UMoveToTargetMovementComponent::Axis::y:
					if ((mStoredDelta.Y > 0 && delta.Y < 0) || (mStoredDelta.Y < 0 && delta.Y > 0)) //block u-turns
					{
						absY = 0;
						break;
					}
					if (abs(mStoredDelta.Y - delta.Y) > 100)
					{
						offset.Y += delta.Y;
					}
					break;
				case UMoveToTargetMovementComponent::Axis::z:
					if ((mStoredDelta.Z > 0 && delta.Z < 0) || (mStoredDelta.Z < 0 && delta.Z > 0)) //block u-turns
					{
						absZ = 0;
						break;
					}
					if (abs(mStoredDelta.Z - delta.Z) > 100)
					{
						offset.Z += delta.Z;
					}
					break;
				default:
					break;
				}
				if (offset.IsNearlyZero())
				{
					continue;
				}
			}
			else
			if (absX > absY && absX > absZ)
			{
				offset.X += delta.X;
				absX = 0;
			}
			else
			if (absY > absX && absY > absZ)
			{
				offset.Y += delta.Y;
				absY = 0;
			}
			else
			if (absZ > absY && absZ > absX)
			{
				offset.Z += delta.Z;
				absZ = 0;
			}
			offset.Normalize();

			const BlockPos nextBlockPos = conversion::ueToBlock(soulLocation + (offset * 100));

			const FullBlock nextBlock = blockSource->getBlockAndData(nextBlockPos);

			if (nextBlock == FullBlock::AIR)
			{
				mStoredDelta = offset;
				break;
			}
		}
	}

	if (mStoredDelta.IsNearlyZero(TargetTolerance)) {
		return;
	}

	float scalarVelocity = FMath::Max(GetMinSpeed(), Velocity.Size());

	if (scalarVelocity < GetMaxSpeed()) {
		scalarVelocity = FMath::Min(scalarVelocity + Acceleration * deltaTime, GetMaxSpeed());
	}
	const auto scalarDelta = mStoredDelta.Size();
	auto normalizedDelta = mStoredDelta;
	normalizedDelta.Normalize();
	auto normalizedVelocity = Velocity;
	normalizedVelocity.Normalize();

	Velocity = lerp(normalizedVelocity, normalizedDelta, LerpStrength * deltaTime) * scalarVelocity;

	FHitResult Hit(1.f);
	Hit.Time = 1.f;
	SafeMoveUpdatedComponent(Velocity * deltaTime, UpdatedComponent->GetComponentQuat(), true, Hit);
	UpdateComponentVelocity();
}

void UMoveToTargetMovementComponent::ApplyArcedMovement() {
	if (ArcMovement) {
		ArcMovement->TryStart({ ArcDuration, ArcMaxZOffset, UpdatedComponent->GetComponentLocation(), Target->GetActorLocation() + mTargetOffset });

		auto newPosition = ArcMovement->GetCurrentPosition();
		auto delta = newPosition - UpdatedComponent->GetComponentLocation();

		FHitResult Hit(1.f);
		Hit.Time = 1.f;
		SafeMoveUpdatedComponent(delta, UpdatedComponent->GetComponentQuat(), true, Hit);
	}
}

bool UMoveToTargetMovementComponent::CanChangeDirection(FVector position)
{
	Axis direction = currentAxis();

	if (direction == Axis::x && ((int)(position.X+50) % 100) < GridTolerance)
	{
		return true;
	}
	if (direction == Axis::y && ((int)(position.Y+50) % 100) < GridTolerance)
	{
		return true;
	}
	if (direction == Axis::z && ((int)(position.Z+50) % 100) < GridTolerance)
	{
		return true;
	}
	return false;
}

UMoveToTargetMovementComponent::Axis UMoveToTargetMovementComponent::currentAxis()
{
	float absX = abs(mStoredDelta.X);
	float absY = abs(mStoredDelta.Y);
	float absZ = abs(mStoredDelta.Z);

	if (absX > absY && absX > absZ)
	{
		return Axis::x;
	}
	if (absY > absX && absY > absZ)
	{
		return Axis::y;
	}
	if (absZ > absY && absZ > absX)
	{
		return Axis::z;
	}
	return Axis::x;
}

FArcMovementInfo::FArcMovementInfo() : Duration(2.0f) {
}

FArcMovementInfo::FArcMovementInfo(float duration, float maxZ, FVector startPos, FVector endPos) : Duration(duration), MaxZOffset(maxZ), StartPosition(startPos), EndPosition(endPos) {
}

UArcMovement::UArcMovement() {
}

void UArcMovement::TryStart(FArcMovementInfo info) {
	if (bIsStarted) {
		return;
	}
	StartTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	
	Info = std::move(info);

	bIsStarted = true;
}

const FVector UArcMovement::GetCurrentPosition() const {
	float progressedTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - StartTime;

	auto progress = FMath::Clamp(progressedTime / Info.Duration, 0.0f, 1.0f);

	auto X = FMath::Lerp(Info.StartPosition.X, Info.EndPosition.X, progress);
	auto Y = FMath::Lerp(Info.StartPosition.Y, Info.EndPosition.Y, progress);

	auto maxZPos = Info.StartPosition.Z > Info.EndPosition.Z ? Info.StartPosition.Z + Info.MaxZOffset : Info.EndPosition.Z + Info.MaxZOffset;

	auto sin = FMath::Sin(FMath::Lerp(0.0f, PI, progress));

	auto Z = progress < 0.5f ? FMath::Lerp(Info.StartPosition.Z, maxZPos, sin)
		                     : FMath::Lerp(maxZPos, Info.EndPosition.Z, 1.0f - sin);

	return FVector(X, Y, Z);
}

