// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DrawDebugHelpers.h"
#include "game/Conversion.h"
#include "world/level/ChunkBlockPos.h"
#include "MobCharacterMovementComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/attributes/MovementAttributeSet.h"



void UMobCharacterMovementComponent::BeginPlay() {
	Super::BeginPlay();

	if (AMobCharacter* mob = Cast<AMobCharacter>(GetOwner())) {
		if (UAbilitySystemComponent* abilitySystem = mob->GetAbilitySystemComponent()) {
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GravityAttribute()).AddUObject(this, &UMobCharacterMovementComponent::OnGravityAttributeChanged);
		}
	}

	DefaultRotationRateCache = RotationRate;
	RefreshRotationRate();
	mCachedFloorDelay = 0;
}

UMobCharacterMovementComponent::UMobCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	:
	Super(ObjectInitializer)
{
	bAlwaysCheckFloor = false; //disable always floor check, only do it when absolutely required
}

void UMobCharacterMovementComponent::TogglePathFollowing(bool following) {
	if (following != IsPathFollowing) {		
		IsPathFollowing = following;
		RefreshRotationRate();
	}
}

void UMobCharacterMovementComponent::ToggleIsInCooldown(bool inCooldown) {
	if (inCooldown != IsInCooldown) {
		IsInCooldown = inCooldown;
		RefreshRotationRate();
	}
}

void UMobCharacterMovementComponent::RefreshRotationRate() {
	if (!bBlockRotationRateChange)
	{
		if (IsPathFollowing || !IsInCooldown) {
			RotationRate = PathFollowingRotationRate;
		}
		else {
			RotationRate = DefaultRotationRateCache;
		}
	}
}

void UMobCharacterMovementComponent::SetBlockRotationRateChange(bool bBlock)
{
	bBlockRotationRateChange = bBlock;
}

void UMobCharacterMovementComponent::PhysWalking(float DeltaTime, int32 Iterations)
{
	if(!mBlockedByMobCountDown)
		Super::PhysWalking(DeltaTime,Iterations);
}

void UMobCharacterMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice /*= 0.f*/, const FVector& MoveDelta /*= FVector::ZeroVector*/)
{
	Super::HandleImpact(Hit,TimeSlice,MoveDelta);

	if (AMobCharacter* pBlockingMob = Cast<AMobCharacter>(Hit.GetActor()))
	{
		//we are being blocked by another mob, lets pause our movement a little to let it get out of the way
		mBlockedByMobCountDown = FMath::RandRange(2,4);
	}

}

void UMobCharacterMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	Super::RequestDirectMove(MoveVelocity, bForceMaxSpeed);
	TogglePathFollowing(true);
}

void UMobCharacterMovementComponent::RequestPathMove(const FVector& MoveInput)
{
	Super::RequestPathMove(MoveInput);
	TogglePathFollowing(true);
}

void UMobCharacterMovementComponent::StopActiveMovement()
{
	Super::StopActiveMovement();
	TogglePathFollowing(false);
}

void UMobCharacterMovementComponent::OnRegister() {
	ENetworkSmoothingMode tmpMode = NetworkSmoothingMode;
	Super::OnRegister();
	NetworkSmoothingMode = tmpMode;
}

void UMobCharacterMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult /*= NULL*/) const
{
	if (IsWalking())
	{
		if (mCachedFloorDelay == 0)
		{
			mCachedFloorDelay = 2 +Math::fastRandom() % 4;			
			
			const BlockPos CurBlock = conversion::ueToBlock(CapsuleLocation + FVector(Math::PE_TO_UE_UNITS*0.5f));
			
			if (bForceNextFloorCheck || !CurrentFloor.IsWalkableFloor() || mCachedFloorX != CurBlock.x || mCachedFloorY != CurBlock.z)
			{
				mCachedFloorX = CurBlock.x;
				mCachedFloorY = CurBlock.z;
				Super::FindFloor(CapsuleLocation, OutFloorResult, bCanUseCachedLocation, DownwardSweepResult);
			}
			else
			{
				OutFloorResult = CurrentFloor;
			}
		}
		else
		{
			OutFloorResult = CurrentFloor;
		}
	}
	else
	{
		Super::FindFloor(CapsuleLocation, OutFloorResult, bCanUseCachedLocation, DownwardSweepResult);
	}
}

void UMobCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (mCachedFloorDelay > 0)
	{
		--mCachedFloorDelay;
	}
	if (mBlockedByMobCountDown > 0)
	{
		--mBlockedByMobCountDown;
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMobCharacterMovementComponent::OnGravityAttributeChanged(const FOnAttributeChangeData& data)
{
	GravityScale = data.NewValue;
}