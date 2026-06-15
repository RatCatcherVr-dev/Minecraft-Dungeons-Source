// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "game/util/Pushback.h"
#include <GameplayEffectTypes.h>
#include <GameplayPrediction.h>
#include "AbilitySystemInterface.h"
#include "lovika/LevelCommon.h"
#include "movement/CustomMover.h"
#include "movement/MovementFlyingCommon.h"
#include "GameplayEffect.h"
#include "PlayerCharacterMovementComponent.generated.h"

UENUM()
enum class ELeniencySource : uint8 {
	MovementSpeed,
	BaseChange,
	Dodge,
	CustomMover
};
ENUM_NAME(ELeniencySource)

UENUM(BlueprintType)
enum class ECustomMoverSlot : uint8 {
	INVALID = 0,
	Slot1 = 1,
	Slot2 = 0x2,
	Slot3 = 0x4,
};
ENUM_NAME(ECustomMoverSlot);

UENUM(BlueprintType)
enum class EDodgeDirection : uint8 {
	Invalid,
	Forwards,
	Backwards,
	Left,
	Right
};
ENUM_NAME(EDodgeDirection);

struct MovementModifiers {
	float MaxWalkSpeed = 1.0f;
	float BrakingDecelerationWalking = 1.0f;
	float GroundFriction = 1.0f;
	float MaxAcceleration = 1.0f;
	float MinAnalogWalkSpeed = 0.0f;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDodgeAnimationDirection 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	FPushback Pushback;

	UPROPERTY(EditDefaultsOnly)
	TMap<EDodgeDirection, UAnimSequenceBase*>  DodgeAnimations;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectToApply;

	FGameplayEffectSpec DodgeSpec;

	UPROPERTY(EditDefaultsOnly)
	bool bStopMovementAfterDodge = false;
};

UENUM(BlueprintType)
enum class EDodgeMode : uint8 {
	Unset,
	Normal,
	Swim,
	Elytra,
};
ENUM_NAME(EDodgeMode);

class APlayerCharacter;
class UAnimSequenceBase;
class UEffectBasedCooldownProvider; 

enum class ECustomMovementType : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCustomMoveExecuted, ECustomMoverSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovingChange, bool, isMoving);

/**
 * 
 */
UCLASS(config = Game)
class DUNGEONS_API UPlayerCharacterMovementComponent : public UCharacterMovementComponent, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	void GetLifetimeReplicatedProps(class TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	
	UPlayerCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void SetPresumedTeleportToLocationAndRotation(const FVector& Location, const FRotator& Rotation);
	void SetWantsTeleportTo(const FVector& Location, const FRotator& Rotation);	
	void SetWantsDodgeTo();
	void SetWantsDive(bool wantsToDive);
	float GetDodgeCooldownFraction() const;
	UEffectBasedCooldownProvider* GetDodgeCooldownProvider() const;

	void SetDefaultMovementMode() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void MoveAutonomous(float ClientTimeStamp, float DeltaTime, uint8 CompressedFlags, const FVector& NewAccel) override;
	void SmoothClientPosition(float DeltaSeconds) override;

	void SetMovementModifiers(const MovementModifiers& modifiers) { MovementCurrent = modifiers; }
	void ResetMovementModifiers() { MovementCurrent = MovementDefault; }

	void SetWantsToCustomMove(ECustomMoverSlot slot);

	friend class FSavedMove_PlayerCharacter;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void ModifierSurfaceChanged(EMaterialTypeEnum material); // D11.DB
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

	bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	UPROPERTY(Category = "Character Movement: Jumping / Falling", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float UnderwaterFallingLateralFriction = 0.8f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	FName DodgeAnimationSlot = "FullBody";

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	TMap<EDodgeMode, FDodgeAnimationDirection> PushbackDodgeAnimations;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	FRotator DodgeAirRotationRate = FRotator(0, 10.f, 0);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeEndSlowRotationRateFactor = 0.9f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeEndSlowBrakingFactor = 0.001f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeEndSlowSpeedFactor = 0.25f;	

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeCameraXYSnapAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeCameraXYSnapRestoreDurationSeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeCooldown = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float MinBrakingDeceleration = 800.0f;

	bool IsDodging() const;
	void ApplyDodgeCooldown(const  UAbilitySystemComponent * abilitySystem);
	void StopMovementImmediately() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|GlidingNearGround")
	float GlideNearGroundDistance  = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|GlidingNearGround")
	float GlideNearGroundSpeedDeceleration = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|GlidingNearGround")
	float GlideNearGroundMaxSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideInitialVerticalVelocityMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideGravityMultiplier = 0.025f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideAirControl = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideMaxSpeed = 1200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideMaxAcceleration = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Gliding")
	float GlideBrakingDeceleration = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Diving")
	float DiveGravityMultiplier = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Diving")
	float DiveAirControl = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Diving")
	float DiveMaxSpeed = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Diving")
	float DiveMaxAcceleration = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|Diving")
	float DiveBrakingDeceleration = 1000.0f;


	//Clamp max boost speed from transition from diving to gliding.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra")
	float DiveToGlideMaxBoostVelocity = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra")
	float DiveToGlideDiveBoostMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Elytra|WingDash")
	float WingDashGravityScale = 0.16f;

	bool IsFalling() const override;
	bool IsGliding() const;
	bool IsDiving() const;
	bool IsMovingWithElytra() const;

	bool HandlePendingLaunch() override;

	UFUNCTION(BlueprintPure)
	bool IsBlastingOff() const;

	void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	float GetGravityZ() const override;
	FVector GetAirControl(float DeltaTime, float TickAirControl, const FVector& FallAcceleration) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMovementMode(EMovementMode NewMovementMode, ECustomMovementType NewCustomMode);

	float GetMaxAcceleration() const override;
	float GetMaxSpeed() const override;
	float GetMaxBrakingDeceleration() const override;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_BlastOff(const FVector& blastOffVelocity);
protected:
	void PhysCustom(float deltaTime, int32 Iterations) override;
	void PhysFalling(float deltaTime, int32 Iterations) override;
	void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
	void PhysGliding(float deltaTime, int32 Iterations);
	void PhysDiving(float deltaTime, int32 Iterations);
	void PhysBlastingOff(float deltaTime, int32 Iterations);


public:
	// jryden AB#349936 -- end


	ECustomMoverSlot RegisterCustomMover(TSubclassOf<UCustomMover>);
	void RemoveMoverFromSlot(ECustomMoverSlot);


	FOnCustomMoveExecuted OnCustomMoveExecuted;
	FOnMovingChange OnMovingChange;

protected:
	//Speed at which we permanently set error checking to the lenient setting
	UPROPERTY(GlobalConfig)
	float LeniencySpeedMultiplierToggleLimit = 2.f;

	//The maximum allowed error in unreal units before the server will correct the client *when in lenient mode*
	UPROPERTY(GlobalConfig)
	float LenientMaxPositionError = 300.f;

	//The time in ms added to ping to toggle movement leniency off.
	UPROPERTY(GlobalConfig)
	int32 LeniencyOffToggleDelayBiasMs = 96;

	bool ClientUpdatePositionAfterServerUpdate() override;

private:
	void SetBase(UPrimitiveComponent* NewBase, const FName BoneName /* = NAME_None */, bool bNotifyActor/* =true */) override;

	UPROPERTY(Replicated, Transient)
	TArray<TSubclassOf<UCustomMover>> ReplicatedCustomMovers;

	TArray<TSubclassOf<UCustomMover>> CurrentCustomMovers;
	
	mutable int		mCachedFloorDelay = 0;
	mutable int		mCachedFloorX = -1;
	mutable int		mCachedFloorY = -1;	

	uint8 HasTickedAutonomousAnimations = 0;

	FVector TeleportToLocation;
	FRotator TeleportToRotation;
	uint8 bWantsToTeleport : 1;

	void OnGravityAttributeChanged(const FOnAttributeChangeData& data);
	void OnSpeedAttributeChanged(const FOnAttributeChangeData& data);
	void OnRotationAttributeChanged(const FOnAttributeChangeData& data);

	void ExecuteTeleportTo(const FVector& Location, const FRotator& Rotation);

	uint8 bWantsToCustomMove : 4;

	uint8 bWantsToDive : 1;
	uint8 bWantsToDodge : 1;
	uint8 bDodgeWithIntent : 1;
	FDelegateHandle DodgeOnLandedHandle;
	FRotator DefaultRotationRateCache;
	float DefaultWalkSpeed;
	float DefaultBrakingFriction;	
	float DefaultGroundFriction;
	float DefaultAcceleration;

	float OldGravityScale;

	UPROPERTY(Replicated, Transient)
	bool bUseDodgeGravity = false;

	bool bIsDodging = false;
	bool bHasInputVector = false;

	void OnDodgeBegun(EDodgeDirection);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnDodgeBegun(EDodgeDirection animDirection);
	
	UFUNCTION(Server, unreliable, WithValidation)
	void ServerBeginDodgePrediction(FPredictionKey key);

	APlayerCharacter* GetPlayerCharacter() const;

	void RefreshRotationRate();
	void RefreshSpeed();
	void ApplySpeedModifiers(const MovementModifiers& modifiers);

	void TryCustomMoverForSlot(ECustomMoverSlot slot);

	void OnDodgeLanded();

	EDodgeDirection GetDodgeAnimationDirection(const FVector&) const;

	FPredictionKey CurrentPrediction;

	UPROPERTY()
	UEffectBasedCooldownProvider* CooldownProvider = nullptr;

	MovementModifiers MovementDefault;
	MovementModifiers MovementCurrent;

	void StartLeniencyWindow(ELeniencySource);
	void ToggleLeniencyForSource(bool on, ELeniencySource);

	TOptional<float> OverrideAllowedPositionError;
	TMap<ELeniencySource, FTimerHandle> LeniencyToggles;

	float GetAllowedPositionError() const;
	FTimerHandle EnableCorrectionHandle;
	bool bLeniencyToggled;

	EDodgeMode CurrentDodgeMode;
	void DoDodgeModeLogicOnStart(EDodgeMode DodgeMode);
	void DoDodgeModeLogicOnMontageEnded(EDodgeMode DodgeMode);
	void DoDodgeModeLogicOnLanded(EDodgeMode DodgeMode);
	UFUNCTION()
	void ReplenishGravity();
	UFUNCTION()
	void OnDodgeAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnDodgeAnimationEndedTimer();

	bool UnderwaterDecelerate = false;

	bool isMoving = false;
	bool IsGlidingNearGround() const;

	EDodgeMode GetDodgeMode() const;
};



class FSavedMove_PlayerCharacter : public FSavedMove_Character
{
public:
	TArray<TSubclassOf<UCustomMover>, TInlineAllocator<3>> SavedCustomMovers;
	
	FVector SavedTeleportToLocation;
	FRotator SavedTeleportToRotator;
	uint8 bSavedWantsToTeleport : 1;

	uint8 bSavedWantsToDive : 1;

	uint8 bSavedWantsToDodge : 1;

	uint8 bSavedWantsToCustomMove : 4;

	typedef FSavedMove_Character Super;

	///@brief Resets all saved variables.
	void Clear() override;

	///@brief Store input commands in the compressed flags.
	uint8 GetCompressedFlags() const override;

	///@brief This is used to check whether or not two moves can be combined into one.
	///Basically you just check to make sure that the saved variables are the same.
	bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	///@brief Sets up the move before sending it to the server. 
	void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	///@brief Sets variables on character movement component before making a predictive correction.
	void PrepMoveFor(class ACharacter* Character) override;
};

class FNetworkPredictionData_Client_PlayerMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_PlayerMovement(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///@brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};
