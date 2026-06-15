// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PlayerCharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "IConsoleManager.h"
#include "TeleportComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/DodgeRechargingGameplayEffect.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/OxygenComponent.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "game/Conversion.h"
#include "game/UEffectBasedCooldownProvider.h"

#include "AbilitySystemGlobals.h"
#include "ClientEventHub.h"
#include "StatTracker.h"
#include "PlayerExperienceComponent.h"
#include "TimerManager.h"
#include "GameplayEffect.h"
#include <GameFramework/GameNetworkManager.h>
#include <GameFramework/PlayerState.h>
#include "TimerManager.h"
#include "GameplayEffect.h"
#include "LovikaSpringArmComponent.h"
#include "game/component/ElytraComponent.h"
#include "game/GameTypes.h"

FORCEINLINE uint32 operator&(uint32 a, ECustomMoverSlot b) { return a & static_cast<uint32>(b); }

namespace playermovement {
	FSavedMove_Character::CompressedFlags SlotToFlag(ECustomMoverSlot slot) {
		switch (slot) {
			case ECustomMoverSlot::Slot1: return FSavedMove_Character::FLAG_Reserved_1;
			case ECustomMoverSlot::Slot2: return FSavedMove_Character::FLAG_Reserved_2;
			case ECustomMoverSlot::Slot3: return FSavedMove_Character::FLAG_Custom_2;
		}

		ensureMsgf(false, TEXT("Trying to resolve movement flag for invalid custom mover slot."));
		return FSavedMove_Character::FLAG_Custom_3;
	}

	ECustomMoverSlot FlagToSlot(FSavedMove_Character::CompressedFlags flag) {
		switch(flag) {
			case FSavedMove_Character::FLAG_Reserved_1: return ECustomMoverSlot::Slot1;
			case FSavedMove_Character::FLAG_Reserved_2: return ECustomMoverSlot::Slot2;
			case FSavedMove_Character::FLAG_Custom_2: return ECustomMoverSlot::Slot3;
		default: ;
		}

		ensureMsgf(false, TEXT("Trying to resolve slot for invalid flag."));
		return ECustomMoverSlot::INVALID;
	}

	ECustomMoverSlot IndexToSlot(uint8 index) {
		switch(index) {
			case 0: return ECustomMoverSlot::Slot1;
			case 1: return ECustomMoverSlot::Slot2;
			case 2: return ECustomMoverSlot::Slot3;
			default : return ECustomMoverSlot::INVALID;
		}
	}
	
	uint8 SlotToIndex(ECustomMoverSlot slot) {
		switch (slot) {
			case ECustomMoverSlot::Slot1: return 0;
			case ECustomMoverSlot::Slot2: return 1;
			case ECustomMoverSlot::Slot3: return 2;
		}

		ensureMsgf(false, TEXT("Trying to resolve index for invalid custom mover slot."));
		return 0xff;
	}

	uint32 ConvertToFlag(uint8 bWantsToCustomMove, ECustomMoverSlot slot) {
		if(bWantsToCustomMove & slot) return SlotToFlag(slot);

		return 0;
	}

	uint8 ConvertToMoverSlot(uint32 flags, FSavedMove_Character::CompressedFlags flag) {
		if(flags & flag) return static_cast<uint8>(FlagToSlot(flag));

		return 0;
	}
}

void UPlayerCharacterMovementComponent::MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) {
	if (!HasValidData())
	{
		return;
	}

	UpdateFromCompressedFlags(CompressedFlags);
	CharacterOwner->CheckJumpInput(DeltaTime);

	Acceleration = ConstrainInputAcceleration(NewAccel);
	Acceleration = Acceleration.GetClampedToMaxSize(GetMaxAcceleration());
	AnalogInputModifier = ComputeAnalogInputModifier();

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FQuat OldRotation = UpdatedComponent->GetComponentQuat();

	PerformMovement(DeltaTime);

	// Check if data is valid as PerformMovement can mark character for pending kill
	if (!HasValidData())
	{
		return;
	}

	bool ShouldTickPose = !(CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy && IsNetMode(NM_ListenServer));

	// If not playing root motion, tick animations after physics. We do this here to keep events, notifies, states and transitions in sync with client updates.
	if (CharacterOwner && !CharacterOwner->bClientUpdating && !CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh() && ShouldTickPose)
	{
		TickCharacterPose(DeltaTime);
		// TODO: SaveBaseLocation() in case tick moves us?

		// Trigger Events right away, as we could be receiving multiple ServerMoves per frame.
		CharacterOwner->GetMesh()->ConditionallyDispatchQueuedAnimEvents();
	}

	if (CharacterOwner && UpdatedComponent)
	{
		// Smooth local view of remote clients on listen servers
		if (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy && IsNetMode(NM_ListenServer))
		{
			SmoothCorrection(OldLocation, OldRotation, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat());
		}
	}
}

bool UPlayerCharacterMovementComponent::HandlePendingLaunch()
{
	const bool wasBlastingOff = IsBlastingOff();
	const bool didLaunch = Super::HandlePendingLaunch();

	if(didLaunch && wasBlastingOff)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementType::BlastingOff);
	}

	return didLaunch;
}

void UPlayerCharacterMovementComponent::Server_BlastOff_Implementation(const FVector& blastOffVelocity)
{
	if (APlayerCharacter* owner = GetPlayerCharacter())
	{
		auto elytraComponent = owner->GetElytraComponent();
		if (elytraComponent->IsEquipped())
		{
			SetMovementMode(MOVE_Walking);
			StopMovementImmediately();
			elytraComponent->SetIsFlying(true);
			owner->TeleportTo(owner->GetActorLocation() + FVector(0, 0, 30), owner->GetActorRotation());
			SetMovementMode(MOVE_Custom, (uint8)ECustomMovementType::BlastingOff);
			Launch(blastOffVelocity);
		}
	}
}

bool UPlayerCharacterMovementComponent::Server_BlastOff_Validate(const FVector& blastOffVelocity) {
	return true;
}

TAutoConsoleVariable<int32> CVarListenServerInterpolation(
	TEXT("Dungeons.ListenServerInterpolationEnabled"),
	1,
	TEXT("Toggling custom interpolation on listen server.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

void UPlayerCharacterMovementComponent::SmoothClientPosition(float DeltaSeconds) {
	if (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy && IsNetMode(NM_ListenServer) && CVarListenServerInterpolation.GetValueOnGameThread()) {

		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			
			// Smooth interpolation of mesh translation to avoid popping of other client pawns unless under a low tick rate.
			// Faster interpolation if stopped.
			const float ScalarVelocity = Velocity.Size();
			const float SmoothLocationTime = Velocity.IsZero() ? 0.5f*ClientData->SmoothNetUpdateTime : ClientData->SmoothNetUpdateTime;
			if (!Velocity.IsZero()) {
				const float currentOffsetSize = ClientData->MeshTranslationOffset.Size();
				const float alpha = FMath::Clamp((currentOffsetSize - DeltaSeconds * ScalarVelocity) / currentOffsetSize, 0.f, 1.f);
				ClientData->MeshTranslationOffset = ClientData->MeshTranslationOffset * alpha;
				UE_LOG(LogTemp, Verbose, TEXT("Custom lerping offset with alpha: %.2f"), alpha);
			}
			else
			{
				ClientData->MeshTranslationOffset = FVector::ZeroVector;
			}

			// Smooth rotation
			const FQuat MeshRotationTarget = ClientData->MeshRotationTarget;
			if (DeltaSeconds < ClientData->SmoothNetUpdateRotationTime)
			{
				// Slowly decay rotation offset
				ClientData->MeshRotationOffset = FQuat::FastLerp(ClientData->MeshRotationOffset, MeshRotationTarget, DeltaSeconds / ClientData->SmoothNetUpdateRotationTime).GetNormalized();
			}
			else
			{
				ClientData->MeshRotationOffset = MeshRotationTarget;
			}

			// Check if lerp is complete
			if (ClientData->MeshTranslationOffset.IsNearlyZero(1e-2f) && ClientData->MeshRotationOffset.Equals(MeshRotationTarget, 1e-5f))
			{
				bNetworkSmoothingComplete = true;
				// Make sure to snap exactly to target values.
				ClientData->MeshTranslationOffset = FVector::ZeroVector;
				ClientData->MeshRotationOffset = MeshRotationTarget;
			}
		}

		SmoothClientPosition_UpdateVisuals();
	}
	else {
		Super::SmoothClientPosition(DeltaSeconds);
	}
}

void UPlayerCharacterMovementComponent::BeginPlay(){
	Super::BeginPlay();
	bForceMaxAccel = false;
	DefaultRotationRateCache = RotationRate;
	DefaultWalkSpeed = MaxWalkSpeed;
	DefaultBrakingFriction = BrakingDecelerationWalking;
	DefaultGroundFriction = GroundFriction;
	DefaultAcceleration = MaxAcceleration;
	MovementCurrent = MovementDefault;

	if (auto player = GetPlayerCharacter()) {
		if (auto abilitySystem = player->GetAbilitySystemComponent()) {
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::SpeedMultiplierAttribute()).AddUObject(this, &UPlayerCharacterMovementComponent::OnSpeedAttributeChanged);
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::FrictionMultiplierAttribute()).AddUObject(this, &UPlayerCharacterMovementComponent::OnSpeedAttributeChanged);
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::RotationMultiplierAttribute()).AddUObject(this, &UPlayerCharacterMovementComponent::OnRotationAttributeChanged);
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UMovementAttributeSet::GravityAttribute()).AddUObject(this, &UPlayerCharacterMovementComponent::OnGravityAttributeChanged);

			if (GetOwnerRole() == ROLE_Authority) {
				abilitySystem->SetNumericAttributeBase(UMovementAttributeSet::DodgeCooldownAttribute(), DodgeCooldown);
			}
		}
		player->OnModifierSurfaceChanged.AddUObject(this, &UPlayerCharacterMovementComponent::ModifierSurfaceChanged);
	}

	RefreshRotationRate();
	RefreshSpeed();

	if (PushbackDodgeAnimations.Contains(EDodgeMode::Swim)) {
		PushbackDodgeAnimations[EDodgeMode::Swim].Pushback.pushbackZFactor = 0.15f;
	}
}

void UPlayerCharacterMovementComponent::StopMovementImmediately()
{
	Super::StopMovementImmediately();
	bUseDodgeGravity = false;
}

void UPlayerCharacterMovementComponent::SetPresumedTeleportToLocationAndRotation(const FVector& Location, const FRotator& Rotation){
	TeleportToLocation = Location;
	TeleportToRotation = Rotation;
	
	/*
	Begin hack.
		vrak: The server really shouldn't NEED to presumably teleport the clients since 
		the clients should sync these moves back to the server when they execute them.
		However, for some unknown reason, the initial teleport moves seem to just get lost
		resulting in the clients getting corrected into void.
	*/
	ExecuteTeleportTo(Location, Rotation);
	/*End hack*/
}

void UPlayerCharacterMovementComponent::SetWantsTeleportTo(const FVector& Location, const FRotator& Rotation){
	bWantsToTeleport = true;
	TeleportToLocation = Location;
	TeleportToRotation = Rotation;	
}

void UPlayerCharacterMovementComponent::SetWantsDodgeTo(){
	bWantsToDodge = true;
	bDodgeWithIntent = true;
}

void UPlayerCharacterMovementComponent::SetWantsDive(bool wantsToDive) {
	bWantsToDive = wantsToDive;
}

float UPlayerCharacterMovementComponent::GetDodgeCooldownFraction() const {
	return CooldownProvider->GetCooldownFractionRemaining();
}

UEffectBasedCooldownProvider* UPlayerCharacterMovementComponent::GetDodgeCooldownProvider() const {
	return CooldownProvider;
}

void UPlayerCharacterMovementComponent::SetDefaultMovementMode() {
	auto player = Cast<APlayerCharacter>(GetOwner());
	if (player->GetWorldState() == ECharacterWorldState::InWorld) {
		// We don't want any movement when we're not InWorld
		Super::SetDefaultMovementMode();
	}
}

void UPlayerCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {

	if (mCachedFloorDelay > 0)
	{
		--mCachedFloorDelay;
	}

	const FVector InputVector = ConsumeInputVector();
	bHasInputVector = !InputVector.IsNearlyZero();
	if (!HasValidData() || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	UPawnMovementComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Super tick may destroy/invalidate CharacterOwner or UpdatedComponent, so we need to re-check.
	if (!HasValidData())
	{
		return;
	}

	// See if we fell out of the world.
	const bool bIsSimulatingPhysics = UpdatedComponent->IsSimulatingPhysics();
	if (CharacterOwner->Role == ROLE_Authority && (!bCheatFlying || bIsSimulatingPhysics) && !CharacterOwner->CheckStillInWorld())
	{
		return;
	}

	// We don't update if simulating physics (eg ragdolls).
	if (bIsSimulatingPhysics)
	{
		// Update camera to ensure client gets updates even when physics move him far away from point where simulation started
		if (CharacterOwner->Role == ROLE_AutonomousProxy && IsNetMode(NM_Client))
		{
			APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
			APlayerCameraManager* PlayerCameraManager = (PC ? PC->PlayerCameraManager : NULL);
			if (PlayerCameraManager != NULL && PlayerCameraManager->bUseClientSideCameraUpdates)
			{
				PlayerCameraManager->bShouldSendClientSideCameraUpdate = true;
			}
		}

		ClearAccumulatedForces();
		return;
	}

	AvoidanceLockTimer -= DeltaTime;

	if (CharacterOwner->Role > ROLE_SimulatedProxy)
	{
		// If we are a client we might have received an update from the server.
		const bool bIsClient = (CharacterOwner->Role == ROLE_AutonomousProxy && IsNetMode(NM_Client));
		if (bIsClient)
		{
			ClientUpdatePositionAfterServerUpdate();
		}

		// Allow root motion to move characters that have no controller.
		if (CharacterOwner->IsLocallyControlled() || (!CharacterOwner->Controller && bRunPhysicsWithNoController) || (!CharacterOwner->Controller && CharacterOwner->IsPlayingRootMotion()))
		{
			{
				// We need to check the jump state before adjusting input acceleration, to minimize latency
				// and to make sure acceleration respects our potentially new falling state.
				CharacterOwner->CheckJumpInput(DeltaTime);

				// apply input to acceleration
				Acceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
				AnalogInputModifier = ComputeAnalogInputModifier();
			}

			if (CharacterOwner->Role == ROLE_Authority)
			{
				PerformMovement(DeltaTime);
			}
			else if (bIsClient)
			{
				ReplicateMoveToServer(DeltaTime, Acceleration);
			}
		}
		else if (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy)
		{
			// Server ticking for remote client.
			// Between net updates from the client we need to update position if based on another object,
			// otherwise the object will move on intermediate frames and we won't follow it.
			MaybeUpdateBasedMovement(DeltaTime);
			MaybeSaveBaseLocation();

			// Smooth on listen server for local view of remote clients. We may receive updates at a rate different than our own tick rate.
			if (!bNetworkSmoothingComplete && IsNetMode(NM_ListenServer))
			{
				SmoothClientPosition(DeltaTime);
			}

			TickCharacterPose(DeltaTime);
		}
	}
	else if (CharacterOwner->Role == ROLE_SimulatedProxy)
	{
	if (bShrinkProxyCapsule)
	{
		AdjustProxyCapsuleSize();
	}
	SimulatedTick(DeltaTime);
	}

	if (bUseRVOAvoidance)
	{
		UpdateDefaultAvoidance();
	}

	if (bEnablePhysicsInteraction)
	{
		ApplyDownwardForce(DeltaTime);
		ApplyRepulsionForce(DeltaTime);
	}

	// D11.DB - Required for the new slippy ice gameplay effect.
	//			Forces the player character to decelerate if they've built up extra speed
	//			somehow (e.g. after rolling).
	if (IsWalking()) {
		if (IsExceedingMaxSpeed(MaxWalkSpeed)) {
			ApplyVelocityBraking(DeltaTime, 100.0f, 100.0f);
		}
	}

	/*
	#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		const bool bVisualizeMovement = CharacterMovementCVars::VisualizeMovement > 0;
		if (bVisualizeMovement)
		{
			VisualizeMovement();
		}
	#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	*/

	if (HasTickedAutonomousAnimations) HasTickedAutonomousAnimations--;

	if (isMoving && FMath::IsNearlyZero(Velocity.Size2D())) {
		isMoving = false;
		OnMovingChange.Broadcast(false);
	}
	else if (!isMoving && !FMath::IsNearlyZero(Velocity.Size2D())) {
		isMoving = true;
		OnMovingChange.Broadcast(true);
	}
}

void UPlayerCharacterMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bCanUseCachedLocation, const FHitResult* DownwardSweepResult /*= NULL*/) const
{
	if (IsWalking())
	{
		if (mCachedFloorDelay == 0)
		{
			mCachedFloorDelay = 2 + Math::fastRandom() % 4;

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

void UPlayerCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Friction = (IsFalling() && UnderwaterDecelerate) ? UnderwaterFallingLateralFriction : Friction;
	Super::CalcVelocity(DeltaTime, Friction, bFluid || UnderwaterDecelerate, BrakingDeceleration);
}

void UPlayerCharacterMovementComponent::SetWantsToCustomMove(ECustomMoverSlot slot) {
	if(ReplicatedCustomMovers.IsValidIndex(playermovement::SlotToIndex(slot)) && ReplicatedCustomMovers[playermovement::SlotToIndex(slot)].GetDefaultObject()) {
		bWantsToCustomMove |= static_cast<uint8>(slot);	
	}
}
//Set input flags on character from saved inputs
void UPlayerCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)//Client only
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToTeleport = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsToDodge = (Flags&FSavedMove_Character::FLAG_Custom_1) != 0;
	bWantsToDive = (Flags&FSavedMove_Character::FLAG_Custom_2) != 0;

	bWantsToCustomMove = 0;
	bWantsToCustomMove |= playermovement::ConvertToMoverSlot(Flags, playermovement::SlotToFlag(ECustomMoverSlot::Slot1));
	bWantsToCustomMove |= playermovement::ConvertToMoverSlot(Flags, playermovement::SlotToFlag(ECustomMoverSlot::Slot2));
	bWantsToCustomMove |= playermovement::ConvertToMoverSlot(Flags, playermovement::SlotToFlag(ECustomMoverSlot::Slot3));
}

uint8 FSavedMove_PlayerCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToTeleport){
		Result |= FLAG_Custom_0;
	}
	if (bSavedWantsToDodge) {
		Result |= FLAG_Custom_1;
	}
	if (bSavedWantsToDive) {
		Result |= FLAG_Custom_2;
	}

	Result |= playermovement::ConvertToFlag(bSavedWantsToCustomMove, ECustomMoverSlot::Slot1);
	Result |= playermovement::ConvertToFlag(bSavedWantsToCustomMove, ECustomMoverSlot::Slot2);
	Result |= playermovement::ConvertToFlag(bSavedWantsToCustomMove, ECustomMoverSlot::Slot3);
	
	return Result;
}




class FNetworkPredictionData_Client* UPlayerCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UPlayerCharacterMovementComponent* MutableThis = const_cast<UPlayerCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_PlayerMovement(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

EDodgeDirection UPlayerCharacterMovementComponent::GetDodgeAnimationDirection(const FVector& DodgeDirection) const {
	FVector playerForward = CharacterOwner->GetActorForwardVector();
	FVector playerRight = FVector::CrossProduct(FVector::UpVector, playerForward);
	float forwardProduct = FVector::DotProduct(playerForward, DodgeDirection);
	float rightProduct = FVector::DotProduct(playerRight, DodgeDirection);
	bool isForwardBackwards = FMath::Abs(forwardProduct) >= FMath::Abs(rightProduct);
	if (isForwardBackwards) {
		return forwardProduct >= 0 ? EDodgeDirection::Forwards : EDodgeDirection::Backwards;
	} else {
		return rightProduct >= 0 ? EDodgeDirection::Right : EDodgeDirection::Left;
	}
	return EDodgeDirection::Invalid;
}

void UPlayerCharacterMovementComponent::StartLeniencyWindow(ELeniencySource source) {
	auto existing = LeniencyToggles.Find(source);

	//We need to start a timer if we either have no leniency for this source, or if we do and it is not toggled on (i.e we have a valid timer)
	if (!existing || (existing && existing->IsValid())) {
		ToggleLeniencyForSource(true, source);
		ToggleLeniencyForSource(false, source);
	}
}

float UPlayerCharacterMovementComponent::GetAllowedPositionError() const {
	const AGameNetworkManager* GameNetworkManager = (const AGameNetworkManager*)(AGameNetworkManager::StaticClass()->GetDefaultObject());
	return LeniencyToggles.Num() ? LenientMaxPositionError * LenientMaxPositionError : GameNetworkManager->MAXPOSITIONERRORSQUARED;
}

void UPlayerCharacterMovementComponent::ToggleLeniencyForSource(bool on, ELeniencySource source) {
	if (GetOwnerRole() != ENetRole::ROLE_Authority) return;

	if (on) {
		UE_LOG(LogDungeons, Verbose, TEXT("Setting leniency to true for source %s"), *GetEnumValueToString(source));
		auto entry = LeniencyToggles.FindOrAdd(source);
		if (entry.IsValid()) {
			GetWorld()->GetTimerManager().ClearTimer(entry);
			entry.Invalidate();
		}
	}
	else if (auto entry = LeniencyToggles.Find(source)) {
		if (const auto* state = GetPlayerCharacter()->GetPlayerState()) {
			const float OffToggleDelay = (state->Ping * 4 + LeniencyOffToggleDelayBiasMs ) / 1000.f;

			UE_LOG(LogDungeons, Verbose, TEXT("Disabling leniency to false for source %s in %0.3f"), *GetEnumValueToString(source), OffToggleDelay);

			TWeakObjectPtr<UPlayerCharacterMovementComponent> weakThis = this;
			GetWorld()->GetTimerManager().SetTimer(*entry, FTimerDelegate::CreateLambda([weakThis, source]() {
				if (auto actualThis = weakThis.Get()) {
					actualThis->LeniencyToggles.Remove(source);
					UE_LOG(LogDungeons, Verbose, TEXT("Setting leniency to false for source %s"), *GetEnumValueToString(source));
				}
			}), OffToggleDelay, false);
		}
		else {
			LeniencyToggles.Remove(source);
		}
	}
}

EDodgeMode UPlayerCharacterMovementComponent::GetDodgeMode() const
{
	if (APlayerCharacter* player = GetPlayerCharacter()) {
		if (auto abilitySystem = player->GetAbilitySystemComponent()) {
			if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.SuppressRollMode")))) {
				return EDodgeMode::Normal;
			}

			if (player->IsUnderwater()) {
				return EDodgeMode::Swim;
			}
		}
		
		if (auto elytraComponent = player->GetElytraComponent()) {
			if (elytraComponent->IsEquipped()) {
				return EDodgeMode::Elytra;
			}
		}
	}
	return EDodgeMode::Normal;
}

void UPlayerCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	APlayerCharacter* playerOwner = GetPlayerCharacter();
	if (!playerOwner)
	{
		return;
	}

	UnderwaterDecelerate = playerOwner->IsUnderwater();

	//Update dodge movement
	if (bWantsToTeleport){
		ExecuteTeleportTo(TeleportToLocation, TeleportToRotation);
		bWantsToTeleport = false;
	}

	if (CurrentDodgeMode == EDodgeMode::Unset) {
		CurrentDodgeMode = GetDodgeMode();
	}

	if (CharacterOwner->HasAuthority()) {
		if (bWantsToDive && IsGliding()) {
			SetMovementMode(MOVE_Custom, (uint8)ECustomMovementType::Diving);
		}
		else if (!bWantsToDive && IsDiving())
		{
			SetMovementMode(MOVE_Custom, (uint8)ECustomMovementType::Gliding);
		}
	}

	if (bWantsToDodge) {
		auto player = GetPlayerCharacter();
		auto controller = CharacterOwner->GetController();
		if(controller && player) {
			FVector DodgeDirection = controller->GetControlRotation().Vector();
			const auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
			EDodgeMode DodgeModeToUse = GetDodgeMode();
			DodgeDirection.Z = player->IsUnderwater() ? DodgeDirection.Z + 0.15f : DodgeDirection.Z;

			if(CharacterOwner->HasAuthority()) {
				StartLeniencyWindow(ELeniencySource::Dodge);
				MulticastOnDodgeBegun(GetDodgeAnimationDirection(DodgeDirection));
			} else {
				OnDodgeBegun(GetDodgeAnimationDirection(DodgeDirection));
				if (bDodgeWithIntent) {
					CurrentPrediction = FPredictionKey::CreateNewPredictionKey(abilitySystem);
					ServerBeginDodgePrediction(CurrentPrediction);
				}
			}
			
			if (bDodgeWithIntent || CharacterOwner->HasAuthority()) {
				if (DodgeOnLandedHandle.IsValid()) {
					player->OnCharacterWalking.Remove(DodgeOnLandedHandle);
					DodgeOnLandedHandle.Reset();
				}
				DodgeOnLandedHandle = player->OnCharacterWalking.AddUObject(this, &UPlayerCharacterMovementComponent::OnDodgeLanded);

				if (auto springArm = player->GetCameraSpringArm()) {
					springArm->SetXYSnapAmount(DodgeCameraXYSnapAmount, DodgeCameraXYSnapRestoreDurationSeconds);
				}

				player->OnDodgeRollStart(DodgeDirection, CurrentPrediction);
			}

			float dodgeSpeedMultiplier = 1.0f;
			dodgeSpeedMultiplier = abilitySystem->GetNumericAttribute(UMovementAttributeSet::DodgeSpeedAttribute());

			DoDodgeModeLogicOnStart(DodgeModeToUse);

			CurrentDodgeMode = DodgeModeToUse;

			pushback::pushback(PushbackDodgeAnimations[DodgeModeToUse].Pushback, DodgeDirection, *CharacterOwner, dodgeSpeedMultiplier, false);

			//D11.PC Remove one charge
			if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiDodge")))
			{
				float NumCharges = abilitySystem->GetNumericAttributeBase(UMovementAttributeSet::DodgeChargesAttribute());
				abilitySystem->SetNumericAttributeBase(UMovementAttributeSet::DodgeChargesAttribute(), NumCharges - 1);
				NumCharges = abilitySystem->GetNumericAttributeBase(UMovementAttributeSet::DodgeChargesAttribute());
			}
		}
		
		bWantsToDodge = false;
		bDodgeWithIntent = false;
	}

	//If we are not replaying moves CurrentCustomMovers will be empty and we should use the replicated movers
	if (CurrentCustomMovers.Num() == 0) CurrentCustomMovers = ReplicatedCustomMovers;
	
	TryCustomMoverForSlot(ECustomMoverSlot::Slot1);
	TryCustomMoverForSlot(ECustomMoverSlot::Slot2);
	TryCustomMoverForSlot(ECustomMoverSlot::Slot3);

	
	CurrentCustomMovers.Empty();
	bWantsToCustomMove = 0;

	RefreshRotationRate();
	RefreshSpeed();

	if (const auto character = GetPlayerCharacter()) {
		character->GetClientEventHubComponent()->Moved(DeltaSeconds, OldLocation, OldVelocity);
	}
}

void UPlayerCharacterMovementComponent::TryCustomMoverForSlot(ECustomMoverSlot slot) {
	if((bWantsToCustomMove & slot) && CurrentCustomMovers.IsValidIndex(playermovement::SlotToIndex(slot))) {
		if(const auto mover = CurrentCustomMovers[playermovement::SlotToIndex(slot)].GetDefaultObject()) {
			mover->ExecuteCustomMove(this);
			if (mover->ShouldTriggerLeniencyWindow()) {
				StartLeniencyWindow(ELeniencySource::CustomMover);
			}
			OnCustomMoveExecuted.Broadcast(slot);
		}
	}
}


UAbilitySystemComponent* UPlayerCharacterMovementComponent::GetAbilitySystemComponent() const
{
	if(auto player = GetPlayerCharacter()) {
		return player->GetAbilitySystemComponent();
	}

	return nullptr;
}

void UPlayerCharacterMovementComponent::ModifierSurfaceChanged(EMaterialTypeEnum material) {
	switch (material) {
		case EMaterialTypeEnum::SlippyFloor: SetMovementModifiers({ 1.0f, 0.00008f, 0.0f, 0.6f, 1.0f }); break;
		default: ResetMovementModifiers(); break;
	}
}

bool UPlayerCharacterMovementComponent::IsDodging() const {
	return bIsDodging;
}

ECustomMoverSlot UPlayerCharacterMovementComponent::RegisterCustomMover(TSubclassOf<UCustomMover> mover) {
	check(GetOwnerRole() == ROLE_Authority && "Only server should register custom movers.")
	auto index = ReplicatedCustomMovers.IndexOfByKey(nullptr);
	if(index != INDEX_NONE) {
		ReplicatedCustomMovers[index] = mover;

		return playermovement::IndexToSlot(index);
	} else if(!ReplicatedCustomMovers.IsValidIndex(playermovement::SlotToIndex(ECustomMoverSlot::Slot3))) {
		return playermovement::IndexToSlot(ReplicatedCustomMovers.Emplace(MoveTemp(mover)));
	}

	return ECustomMoverSlot::INVALID;
}

void UPlayerCharacterMovementComponent::RemoveMoverFromSlot(ECustomMoverSlot slot) {
	check(GetOwnerRole() == ROLE_Authority && "Only server should remove custom movers.")
	
	auto index = playermovement::SlotToIndex(slot);
	if(ReplicatedCustomMovers.IsValidIndex(index)) {
		ReplicatedCustomMovers[index] = nullptr;
	}
}

bool UPlayerCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate() {
	if(auto character = GetPlayerCharacter()){		
		//Save current state of UPlayerCharacterMovementComponent
		FSavedMove_PlayerCharacter CurrentStateOfCharacterMovement = {};
		const float DeltaTime = 0.0f; //Only used to re-simulate root-motion which we don't use. Also - we can't really know the future ticks delta time.
		const FVector InputVector = GetPendingInputVector();
		const FVector currentInputAcceleration = ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
		CurrentStateOfCharacterMovement.SetMoveFor(character, DeltaTime, currentInputAcceleration, *GetPredictionData_Client_Character());
		
		const bool result = Super::ClientUpdatePositionAfterServerUpdate();
		if(result){
			//Restore state of UPlayerCharacterMovementComponent
			CurrentStateOfCharacterMovement.PrepMoveFor(character);
			UpdateFromCompressedFlags(CurrentStateOfCharacterMovement.GetCompressedFlags());
		}
		return result;		
	}
	return Super::ClientUpdatePositionAfterServerUpdate();
}

void UPlayerCharacterMovementComponent::OnDodgeLanded(){
	
	bIsDodging = false;

	if (auto player = GetPlayerCharacter()) {
		bUseDodgeGravity = false;

		const auto abilitySystem = player->GetAbilitySystemComponent();
		//This is where we close the prediction and replicate the key back to client.
		FScopedPredictionWindow predictionWindow(abilitySystem, CurrentPrediction);
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		
		if (DodgeOnLandedHandle.IsValid()) {
			player->OnCharacterWalking.Remove(DodgeOnLandedHandle);
			DodgeOnLandedHandle.Reset();
		}

		ApplyDodgeCooldown(abilitySystem);

		
		player->OnDodgeRollEnd(CurrentPrediction);

		RefreshRotationRate();
		RefreshSpeed();

		DoDodgeModeLogicOnLanded(CurrentDodgeMode);

		CurrentPrediction = FPredictionKey();
	}
}

void UPlayerCharacterMovementComponent::OnGravityAttributeChanged(const FOnAttributeChangeData& data)
{
	GravityScale = data.NewValue;
	OldGravityScale = GravityScale;
}

void UPlayerCharacterMovementComponent::ApplyDodgeCooldown(const UAbilitySystemComponent * abilitySystem)
{	
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UDodgeRechargingGameplayEffect>(abilitySystem, 1);
	spec.SetSetByCallerMagnitude(UDodgeRechargingGameplayEffect::DurationName, DodgeCooldown);
	spec.SetSetByCallerMagnitude(UDodgeRechargingGameplayEffect::SpeedFactorName, DodgeEndSlowSpeedFactor);
	spec.SetSetByCallerMagnitude(UDodgeRechargingGameplayEffect::RotationFactorName, DodgeEndSlowRotationRateFactor);
	spec.SetSetByCallerMagnitude(UDodgeRechargingGameplayEffect::FrictionFactorName, DodgeEndSlowBrakingFactor);
	spec.CapturedSourceTags = FTagContainerAggregator();

	if (!abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.MultiDodge")) || abilitySystem->GetNumericAttribute(UMovementAttributeSet::DodgeChargesAttribute()) < abilitySystem->GetNumericAttribute(UMovementAttributeSet::MaxDodgeChargesAttribute()))
	{
		if (!CooldownProvider->IsOnCooldown()) {
			CooldownProvider->TriggerCooldownWithSpec(spec, CurrentPrediction);
		}
	}
}

void UPlayerCharacterMovementComponent::OnSpeedAttributeChanged(const FOnAttributeChangeData& data) {
	RefreshSpeed();

	if (data.Attribute == UMovementAttributeSet::SpeedMultiplierAttribute()) {
		if (data.NewValue >= LeniencySpeedMultiplierToggleLimit && data.OldValue < LeniencySpeedMultiplierToggleLimit) {
			ToggleLeniencyForSource(true, ELeniencySource::MovementSpeed);
		}
		else if (data.NewValue < LeniencySpeedMultiplierToggleLimit && data.OldValue >= LeniencySpeedMultiplierToggleLimit) {
			ToggleLeniencyForSource(false, ELeniencySource::MovementSpeed);
		}
		else {
			StartLeniencyWindow(ELeniencySource::MovementSpeed);
		}
	}
}

void UPlayerCharacterMovementComponent::OnRotationAttributeChanged(const FOnAttributeChangeData& data) {
	RefreshRotationRate();
}

void UPlayerCharacterMovementComponent::SetBase(UPrimitiveComponent * NewBase, const FName BoneName, bool bNotifyActor) {
	const UPrimitiveComponent* OldBase = GetMovementBase();
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	
	if (OldBase == NewBase) return;

	if ((OldBase && OldBase->Mobility == EComponentMobility::Movable) || (NewBase && NewBase->Mobility == EComponentMobility::Movable)) {
		StartLeniencyWindow(ELeniencySource::BaseChange);
	}
}

void UPlayerCharacterMovementComponent::ExecuteTeleportTo(const FVector& Location, const FRotator& Rotation){
	auto* rootComponent = CharacterOwner->GetRootComponent();
	if (rootComponent == nullptr) {
		return;
	}

	CurrentFloor.Clear();

	UPrimitiveComponent* ActorPrimComp = Cast<UPrimitiveComponent>(rootComponent);
	if (ActorPrimComp)
	{
		auto StartingPosition = ActorPrimComp->GetComponentLocation();
		FVector const Delta = Location - StartingPosition;
		ActorPrimComp->MoveComponent(Delta, Rotation, false, nullptr, MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		if (auto player = GetPlayerCharacter()) {
			player->RotatePlayer(Rotation);
		}
		//For some reason child worldtocomponent transforms are not updated after this.
		//We need all other children to have updates positions after a teleport.
		ActorPrimComp->UpdateChildTransforms(EUpdateTransformFlags::PropagateFromParent, ETeleportType::TeleportPhysics);
		CharacterOwner->TeleportSucceeded(false);
		auto PostTeleportPosition = ActorPrimComp->GetComponentLocation();
	}
	else
	{
		// not a primitivecomponent, just set directly
		rootComponent->SetWorldLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	if (auto teleportcomponent = CharacterOwner->FindComponentByClass<UTeleportComponent>()) {
		teleportcomponent->OnTeleported.Broadcast();
	}	
}


void UPlayerCharacterMovementComponent::OnDodgeBegun(EDodgeDirection animDirection)
{
	bIsDodging = true;
	if (auto player = GetPlayerCharacter())
	{
		bUseDodgeGravity = true;

		EDodgeMode DodgeModeToUse = GetDodgeMode();
		if (PushbackDodgeAnimations[DodgeModeToUse].DodgeAnimations.Contains(animDirection)) {
			player->PlayAnimationAsDynamicMontage(PushbackDodgeAnimations[DodgeModeToUse].DodgeAnimations[animDirection], DodgeAnimationSlot);
			if (DodgeModeToUse == EDodgeMode::Elytra) player->GetElytraComponent()->PerformRollAnimation();

			if (PushbackDodgeAnimations[DodgeModeToUse].Pushback.pushbackTimeToCallFinish >= PushbackDodgeAnimations[DodgeModeToUse].DodgeAnimations[animDirection]->GetPlayLength())
			{
				player->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(this, &UPlayerCharacterMovementComponent::OnDodgeAnimationEnded);
			}
			else
			{
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPlayerCharacterMovementComponent::OnDodgeAnimationEndedTimer, PushbackDodgeAnimations[DodgeModeToUse].Pushback.pushbackTimeToCallFinish, false);
			}
		}
	}
}

void UPlayerCharacterMovementComponent::MulticastOnDodgeBegun_Implementation(EDodgeDirection animDirection)
{
	auto owner = GetOwner();
	if(owner->GetLocalRole() != ROLE_AutonomousProxy || owner->HasAuthority()) {
		OnDodgeBegun(animDirection);
	}
}

void UPlayerCharacterMovementComponent::ServerBeginDodgePrediction_Implementation(FPredictionKey key)
{
	CurrentPrediction = key;
}

bool UPlayerCharacterMovementComponent::ServerBeginDodgePrediction_Validate(FPredictionKey key)
{
	return true;
}

APlayerCharacter* UPlayerCharacterMovementComponent::GetPlayerCharacter() const{
	return Cast<APlayerCharacter>(CharacterOwner);
}

void UPlayerCharacterMovementComponent::RefreshRotationRate() {
	if (auto player = GetPlayerCharacter()) {
		auto abilitySystem = player->GetAbilitySystemComponent();
		float rotationMultiplier = abilitySystem->GetNumericAttribute(UMovementAttributeSet::RotationMultiplierAttribute());
		float worldtime = GetWorld()->GetTimeSeconds();

		FRotator newRotationRate = (IsFalling() && bIsDodging) ? DodgeAirRotationRate : DefaultRotationRateCache;
		
		RotationRate = newRotationRate * rotationMultiplier;
	}
}

void UPlayerCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations) {
	Super::PhysCustom(deltaTime, Iterations);

	switch(CustomMovementMode) {
		case (uint8)ECustomMovementType::Gliding:
			PhysGliding(deltaTime, Iterations);
			break;

		case (uint8)ECustomMovementType::Diving:
			PhysDiving(deltaTime, Iterations);
			break;

		case (uint8)ECustomMovementType::BlastingOff:
			PhysBlastingOff(deltaTime, Iterations);
			break;

		default:
			break;
	}
}

bool UPlayerCharacterMovementComponent::IsFalling() const {
	return ((MovementMode == MOVE_Falling) && UpdatedComponent) || IsMovingWithElytra();
}

bool UPlayerCharacterMovementComponent::IsGliding() const {
	return ((MovementMode == MOVE_Custom) && CustomMovementMode == (uint8)ECustomMovementType::Gliding) && UpdatedComponent;
}

bool UPlayerCharacterMovementComponent::IsDiving() const {
	return ((MovementMode == MOVE_Custom) && CustomMovementMode == (uint8)ECustomMovementType::Diving) && UpdatedComponent;
}

bool UPlayerCharacterMovementComponent::IsBlastingOff() const {
	return ((MovementMode == MOVE_Custom) && CustomMovementMode == (uint8)ECustomMovementType::BlastingOff) && UpdatedComponent;
}

bool UPlayerCharacterMovementComponent::IsMovingWithElytra() const {
	return ((MovementMode == MOVE_Custom) && CustomMovementMode >= (uint8)ECustomMovementType::Gliding && CustomMovementMode <= (uint8)ECustomMovementType::BlastingOff) && UpdatedComponent;
}	

float UPlayerCharacterMovementComponent::GetGravityZ() const {

	float dodgeGravityScale = 1.0f;
	if(bUseDodgeGravity)
	{
		switch (GetDodgeMode())
		{
		case EDodgeMode::Unset:
			break;
		case EDodgeMode::Normal:
			break;
		case EDodgeMode::Elytra:
			dodgeGravityScale = WingDashGravityScale;
			break;
		case EDodgeMode::Swim:
			dodgeGravityScale = 0.5f;
			break;
		}
	}

	return Super::GetGravityZ() * (IsGliding() ? GlideGravityMultiplier : 1.0f) * (IsDiving() ? DiveGravityMultiplier : 1.0f) * dodgeGravityScale;
}

FVector UPlayerCharacterMovementComponent::GetAirControl(float DeltaTime, float TickAirControl, const FVector& FallAcceleration) {
	if (MovementMode == EMovementMode::MOVE_Custom) {
		switch (CustomMovementMode) {
		case (uint8)ECustomMovementType::Gliding:
			return Super::GetAirControl(DeltaTime, GlideAirControl, FallAcceleration);
		case (uint8)ECustomMovementType::Diving:
			return Super::GetAirControl(DeltaTime, DiveAirControl, FallAcceleration);
		}
	}

	return Super::GetAirControl(DeltaTime, TickAirControl, FallAcceleration);
}

void UPlayerCharacterMovementComponent::ServerSetMovementMode_Implementation(EMovementMode NewMovementMode, ECustomMovementType NewCustomMode) {
	// jryden, this looks pretty horrible -- is CustomMode even SUPPOSED to be an enum?   have to go for now to get compiling working
	SetMovementMode(NewMovementMode, (uint8)NewCustomMode);
}


bool UPlayerCharacterMovementComponent::ServerSetMovementMode_Validate(EMovementMode NewMovementMode, ECustomMovementType NewCustomMode) {
	return true;
}

void UPlayerCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) {
	const auto character = GetPlayerCharacter();

	if(MovementMode == EMovementMode::MOVE_Custom && PreviousCustomMode != CustomMovementMode){
		switch ((ECustomMovementType)CustomMovementMode) {
		case ECustomMovementType::Gliding:
			if ((ECustomMovementType)PreviousCustomMode == ECustomMovementType::Diving)
			{
				if (character->HasAuthority())
				{
					const float zVelocity = Velocity.Z;
					Velocity = character->GetActorForwardVector() * FMath::Clamp(-zVelocity * DiveToGlideDiveBoostMultiplier, 0.0f, DiveToGlideMaxBoostVelocity);
					Velocity.Z = zVelocity * GlideInitialVerticalVelocityMultiplier;
				}

				character->GetElytraComponent()->TriggerAudioStateChange(EElytraAudioState::DiveRecovery);
			}
			else
			{
				character->GetElytraComponent()->TriggerAudioStateChange(EElytraAudioState::Glide);
			}
			break;
		case ECustomMovementType::Diving:
			character->GetElytraComponent()->TriggerAudioStateChange(EElytraAudioState::Dive);
			break;
		}
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UPlayerCharacterMovementComponent::PhysFalling(float deltaTime, int32 Iterations) {
	Super::PhysFalling(deltaTime, Iterations);
}

void UPlayerCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) {
	
	auto elytraComponent = GetPlayerCharacter()->GetElytraComponent();
	if (IsDiving()) {
		elytraComponent->DoDiveImpact();
	}

	Super::ProcessLanded(Hit, remainingTime, Iterations);
	elytraComponent->SetIsFlying(false);
}

void UPlayerCharacterMovementComponent::PhysGliding(float deltaTime, int32 Iterations) {
	Super::PhysFalling(deltaTime, Iterations);
}

void UPlayerCharacterMovementComponent::PhysBlastingOff(float deltaTime, int32 Iterations) {
	Super::PhysFalling(deltaTime, Iterations);
	if (Velocity.Z < 0.0f) {
		SetMovementMode(MOVE_Custom, (uint8)ECustomMovementType::Gliding);
	}
}

void UPlayerCharacterMovementComponent::PhysDiving(float deltaTime, int32 Iterations) {
	Super::PhysFalling(deltaTime, Iterations);
}

void UPlayerCharacterMovementComponent::ApplySpeedModifiers(const MovementModifiers& modifiers) {
	if (auto playerCharacter = GetPlayerCharacter()) {
		if (auto abilitySystem = playerCharacter->GetAbilitySystemComponent()) {
			const float speedMultiplier = abilitySystem->GetNumericAttribute(UMovementAttributeSet::SpeedMultiplierAttribute());
			const float frictionMultiplier = abilitySystem->GetNumericAttribute(UMovementAttributeSet::FrictionMultiplierAttribute());

			MaxWalkSpeed = DefaultWalkSpeed * modifiers.MaxWalkSpeed * speedMultiplier;
			GroundFriction = DefaultGroundFriction * modifiers.GroundFriction;
			MaxAcceleration = DefaultAcceleration * modifiers.MaxAcceleration;
			BrakingDecelerationWalking = DefaultBrakingFriction * modifiers.BrakingDecelerationWalking * frictionMultiplier;
			MinAnalogWalkSpeed = MaxWalkSpeed * modifiers.MinAnalogWalkSpeed;

			BrakingDecelerationWalking = FMath::Max(MinBrakingDeceleration, DefaultBrakingFriction * modifiers.BrakingDecelerationWalking * frictionMultiplier);
		}
	}
}

void UPlayerCharacterMovementComponent::RefreshSpeed(){	
	if(auto playerCharacter = GetPlayerCharacter()){
		auto abilitySystem = playerCharacter->GetAbilitySystemComponent();

		ApplySpeedModifiers(MovementCurrent);

		bool sliding = false;

		// D11.DB - Extra processing required if we're in a low traction state.
		if (ABasePlayerController* PC = Cast<ABasePlayerController>(CharacterOwner->GetController())) {
			if (GroundFriction < 1.0f) { 
				if (PC->GetPathFollowingComponent()->HasValidPath()) {
					// D11.DB - As player movement is handled via pathfinding we have to handle an edge case here
					//			when the player is on a slippery surface.
					//			It is possible for the low friction to cause the player to start orbiting the path
					//			finding destination if they change direction particularly quickly.
					//			The workaround is to abort pathfinding if the pathfinding destination is 
					//			near to the player and they are also facing away from it.
					const auto playerLocation = GetOwner()->GetActorLocation();
					const auto playerForward = GetOwner()->GetActorForwardVector();
					const auto destination = PC->GetPathFollowingComponent()->GetPathDestination();
					const float speedMultiplier = abilitySystem->GetNumericAttribute(UMovementAttributeSet::SpeedMultiplierAttribute());

					const auto vecToDestination = destination - playerLocation;
					const auto distanceSquared = FVector::DistSquared2D(playerLocation, destination);
					const float distanceThreshold = 200.0f * speedMultiplier;

					const bool close = distanceSquared < distanceThreshold * distanceThreshold;
					const bool facing = FVector::DotProduct(playerForward, vecToDestination) > 0;

					if (close && !facing) {
						PC->GetPathFollowingComponent()->AbortMove(*this, FPathFollowingResultFlags::AlreadyAtGoal, false);
					}
				}
			}
			sliding = PC->IsLocalController() && !bHasInputVector;
		}

		playerCharacter->setIntendedAnimationSpeed(sliding ? 0.0f : MaxWalkSpeed);
	}
}

float UPlayerCharacterMovementComponent::GetMaxAcceleration() const {
	if (MovementMode == EMovementMode::MOVE_Custom) {
		switch (CustomMovementMode) {
		case (uint8)ECustomMovementType::Gliding:
			return GlideMaxAcceleration;
		case (uint8)ECustomMovementType::Diving:
			return DiveMaxAcceleration;
		}
	}

	return Super::GetMaxAcceleration();
}

float UPlayerCharacterMovementComponent::GetMaxSpeed() const {
	if(MovementMode == EMovementMode::MOVE_Custom) {
		switch ((ECustomMovementType)CustomMovementMode) {
		case ECustomMovementType::Gliding:
			return IsGlidingNearGround() ? GlideNearGroundMaxSpeed : GlideMaxSpeed;
		case ECustomMovementType::Diving:
			return DiveMaxSpeed;
		}
	}

	return Super::GetMaxSpeed();
}

float UPlayerCharacterMovementComponent::GetMaxBrakingDeceleration() const {
	if (MovementMode == EMovementMode::MOVE_Custom) {
		switch ((ECustomMovementType)CustomMovementMode) {
		case ECustomMovementType::Gliding:
			return IsGlidingNearGround() ? GlideNearGroundSpeedDeceleration : GlideBrakingDeceleration;
		case ECustomMovementType::Diving:
			return DiveBrakingDeceleration;
		}
	}

	return Super::GetMaxBrakingDeceleration();
}

void UPlayerCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerCharacterMovementComponent, ReplicatedCustomMovers);
	DOREPLIFETIME(UPlayerCharacterMovementComponent, bUseDodgeGravity);
}

UPlayerCharacterMovementComponent::UPlayerCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAlwaysCheckFloor = false; //disable always floor check, only do it when absolutely required
	CooldownProvider = CreateDefaultSubobject<UEffectBasedCooldownProvider>(TEXT("CooldownProvider"));
	CooldownProvider->CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Dodge"));
	MovementDefault = { 1.0f, 1.0f, 1.0f, 1.0f, 0.0f };
	MovementModifiers MovementSlippery = { 1.0f, 0.00008f, 0.0f, 0.6f, 1.0f };

	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> playerWingDashRightAnim(*(game::PrefabPath("Actors/Items/Elytra/Animations/PlayerAnimations/RollRight")));
	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> playerWingDashLeftAnim(*(game::PrefabPath("Actors/Items/Elytra/Animations/PlayerAnimations/RollLeft")));
	static ConstructorHelpers::FObjectFinder<UAnimSequenceBase> playerWingDashFrontAnim(*(game::PrefabPath("Actors/Items/Elytra/Animations/PlayerAnimations/RollFront")));

	auto& elytraDodge = PushbackDodgeAnimations.Add(EDodgeMode::Elytra, FDodgeAnimationDirection());
	elytraDodge.DodgeAnimations.Add(EDodgeDirection::Forwards, playerWingDashFrontAnim.Object);
	elytraDodge.DodgeAnimations.Add(EDodgeDirection::Backwards, playerWingDashFrontAnim.Object);
	elytraDodge.DodgeAnimations.Add(EDodgeDirection::Right, playerWingDashRightAnim.Object);
	elytraDodge.DodgeAnimations.Add(EDodgeDirection::Left, playerWingDashLeftAnim.Object);
	elytraDodge.Pushback.enablePushback = true;
	elytraDodge.Pushback.pushbackStrength = 3.f;
	elytraDodge.Pushback.pushbackZFactor = 0.2f;
	elytraDodge.Pushback.useForwardDirection = true;
	elytraDodge.Pushback.pushbackMultiplierRespectFactor = 0.0f;
}


bool UPlayerCharacterMovementComponent::ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	// Check location difference against global setting
	if (!bIgnoreClientMovementErrorChecksAndCorrection)
	{
		const FVector LocDiff = UpdatedComponent->GetComponentLocation() - ClientWorldLocation;
		const AGameNetworkManager* GameNetworkManager = (const AGameNetworkManager*)(AGameNetworkManager::StaticClass()->GetDefaultObject());

		if (LocDiff.SizeSquared() > GetAllowedPositionError())
		{
			bNetworkLargeClientCorrection = (LocDiff.SizeSquared() > FMath::Square(NetworkLargeClientCorrectionDistance));
			return true;
		}
	}

	// Check for disagreement in movement mode
	const uint8 CurrentPackedMovementMode = PackNetworkMovementMode();
	if (CurrentPackedMovementMode != ClientMovementMode)
	{
		return true;
	}

	return false;
}

void FSavedMove_PlayerCharacter::Clear()
{
	Super::Clear();

	bSavedWantsToTeleport = false;
	SavedTeleportToLocation = FVector::ZeroVector;
	SavedTeleportToRotator = FRotator::ZeroRotator;

	bSavedWantsToDive = false;
	bSavedWantsToDodge = false;
	bSavedWantsToCustomMove = 0;
	SavedCustomMovers.Empty();
}



bool FSavedMove_PlayerCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (auto playerCharacterMove = ((FSavedMove_PlayerCharacter*)&NewMove)) {
		if (bSavedWantsToTeleport != playerCharacterMove->bSavedWantsToTeleport) {
			return false;
		}
		if (!SavedTeleportToLocation.Equals(playerCharacterMove->SavedTeleportToLocation)) {
			return false;
		}
		if (!SavedTeleportToRotator.Equals(playerCharacterMove->SavedTeleportToRotator)) {
			return false;
		}
		if (bSavedWantsToDodge != playerCharacterMove->bSavedWantsToDodge) {
			return false;
		} else if(SavedControlRotation != playerCharacterMove->SavedControlRotation) {
			return false;
		}
		if (bSavedWantsToDive != playerCharacterMove->bSavedWantsToDive) {
			return false;
		}
		if(bSavedWantsToCustomMove != playerCharacterMove->bSavedWantsToCustomMove) {
			return false;
		}
	}
	
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_PlayerCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	UPlayerCharacterMovementComponent* CharMov = Cast<UPlayerCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		bSavedWantsToTeleport = CharMov->bWantsToTeleport;
		SavedTeleportToLocation = CharMov->TeleportToLocation;
		SavedTeleportToRotator = CharMov->TeleportToRotation;

		bSavedWantsToDive = CharMov->bWantsToDive;
		bSavedWantsToDodge = CharMov->bWantsToDodge;

		SavedCustomMovers = CharMov->ReplicatedCustomMovers;
		bSavedWantsToCustomMove = CharMov->bWantsToCustomMove;
	}
}

void FSavedMove_PlayerCharacter::PrepMoveFor(class ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UPlayerCharacterMovementComponent* CharMov = Cast<UPlayerCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		CharMov->TeleportToLocation = SavedTeleportToLocation;
		CharMov->TeleportToRotation = SavedTeleportToRotator;
		CharMov->CurrentCustomMovers = SavedCustomMovers;
	}
}


void UPlayerCharacterMovementComponent::DoDodgeModeLogicOnStart(EDodgeMode DodgeMode)
{
	const auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
	if (abilitySystem && PushbackDodgeAnimations[DodgeMode].GameplayEffectToApply)
	{
		FGameplayEffectSpec Spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, PushbackDodgeAnimations[DodgeMode].GameplayEffectToApply);
		Spec.SetSetByCallerMagnitude("Duration", PushbackDodgeAnimations[DodgeMode].Pushback.pushbackTimeToCallFinish);
		PushbackDodgeAnimations[DodgeMode].DodgeSpec = Spec;
		abilitySystem->ApplyGameplayEffectSpecToSelf(Spec);
	}
}

void UPlayerCharacterMovementComponent::DoDodgeModeLogicOnMontageEnded(EDodgeMode DodgeMode)
{
	if (auto player = GetPlayerCharacter()) 
	{
		player->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &UPlayerCharacterMovementComponent::OnDodgeAnimationEnded);
		bUseDodgeGravity = false;

		switch (DodgeMode)
		{
		default:
			break;
		case EDodgeMode::Unset:
			break;
		case EDodgeMode::Normal:
			ReplenishGravity();
			break;
		case EDodgeMode::Elytra:
			player->GetElytraComponent()->TriggerAudioStateChange(EElytraAudioState::None);
			player->ResetFallFromLocation();
		case EDodgeMode::Swim:
			if (PushbackDodgeAnimations[DodgeMode].bStopMovementAfterDodge)
			{
				StopActiveMovement();
				Velocity = FVector::ZeroVector;
			}
			ReplenishGravity();
			break;
		}
	}
}

void UPlayerCharacterMovementComponent::DoDodgeModeLogicOnLanded(EDodgeMode DodgeMode)
{
	switch (DodgeMode)
	{
	default:
		break;
	case EDodgeMode::Unset:
		break;
	case EDodgeMode::Normal:
		break;
	case EDodgeMode::Swim:
		break;
	case EDodgeMode::Elytra:
		break;
	}
}

void UPlayerCharacterMovementComponent::ReplenishGravity()
{
	if (GravityScale != 1.0f) {
		GravityScale = OldGravityScale;
	}
}

void UPlayerCharacterMovementComponent::OnDodgeAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		OnDodgeAnimationEndedTimer();
	}
}

void UPlayerCharacterMovementComponent::OnDodgeAnimationEndedTimer()
{
	DoDodgeModeLogicOnMontageEnded(CurrentDodgeMode);
}

FNetworkPredictionData_Client_PlayerMovement::FNetworkPredictionData_Client_PlayerMovement(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_PlayerMovement::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_PlayerCharacter());
}

bool UPlayerCharacterMovementComponent::IsGlidingNearGround() const
{
	if(const APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner()))
	{
		return character->GetElytraComponent()->GetDistanceToLandMarker() < GlideNearGroundDistance;
	}

	return false;
}
