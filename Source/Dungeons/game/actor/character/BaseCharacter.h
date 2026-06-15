#pragma once

#include "GameFramework/Character.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/component/HealthComponent.h"
#include "game/component/HealthBarComponent.h"
#include "game/abilities/effects/InvulnerableGameplayEffect.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "lovika/LevelCommon.h"
#include "CommonTypes.h"
#include "AbilitySystemInterface.h"
#include "CharacterAnimInstance.h"
#include "GameplayPrediction.h"
#include "game/team/TeamName.h"
#include "game/util/ActorQuery.h"
#include "game/actor/FrozenSolidIceCubeActor.h"
#include "game/component/PushVolumeReceiverComponent.h"
#include "game/level/GameTile.h"
#include "BaseCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnControllerReplicated);
DECLARE_MULTICAST_DELEGATE(FOnActualDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKilledOther, AActor*, byWhat, AActor*, byWhom, AActor*, other);

DECLARE_MULTICAST_DELEGATE(FOnCharacterWalking);
DECLARE_MULTICAST_DELEGATE(FCancelActions);

DECLARE_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE(FOnRevive);
DECLARE_MULTICAST_DELEGATE(FOnWorldStateChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStepMaterialChanged, EMaterialTypeEnum);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOverlapMaterialChanged, EMaterialTypeEnum);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifierSurfaceChanged, EMaterialTypeEnum);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinionAcquired, ABaseCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMinionReleased, ABaseCharacter*);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttackStateChanged, EAttackState);
DECLARE_MULTICAST_DELEGATE(FOnTeamChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamageImmunityChanged, bool, bHasDamageImmuninty);

UENUM()
enum class ESynchornizedCollisionResponse : uint8 {
	Default,
	WorldOnly,
};

namespace game {
class Game;
}

USTRUCT()
struct DUNGEONS_API FMontageData {
	GENERATED_USTRUCT_BODY()

	enum FLAGS {
		BlendInTimeFlag = 0x1,
		BlendOutTimeFlag = 0x2,
		InPlayRateFlag = 0x4,
		LoopCountFlag = 0x8,
		BlendOutTriggerTimeFlag = 0x10,
		InTimeToStartMontageAtFlag = 0x20,
		SectionPercentageFlag = 0x40,
		ReturnValueTypeFlag = 0x80,
		PredictionKeyFlag = 0x100
	};

	UAnimSequenceBase* asset;
	FName slotNodeName;
	float blendInTime = 0.f;
	float blendOutTime = 0.2f;
	float inPlayRate = 1.f;
	uint8 loopCount = 1;
	float blendOutTriggerTime = 0.f;
	float inTimeToStartMontageAt = 0.f;
	float sectionPercentage = 0.0f;
	EMontagePlayReturnType returnValueType = EMontagePlayReturnType::MontageLength;
	FPredictionKey key;

	uint16 gatherRepBits(FArchive&);
	void writeOptionals(FArchive&, uint16 RepBits);
	void readWriteFloat(FArchive&, float);
	bool NetSerialize(FArchive&, class UPackageMap*, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FMontageData> : public TStructOpsTypeTraitsBase2<FMontageData>
{
	enum
	{
		WithNetSerializer = true
	};
};


UCLASS()
class DUNGEONS_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface {
	GENERATED_BODY()
public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	void PreInitializeComponents() override;

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Searches components array and returns first encountered component of the specified class. */
	virtual UActorComponent* FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const override;

	template<class T>
	T* FindComponentByClass() const
	{
		return ACharacter::FindComponentByClass<T>();
	}

	virtual void Tick(float DeltaSecs) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	void Falling() override;	
	void Landed(const FHitResult& Hit) override;

	void OnAddedToLevel();

	UFUNCTION(Client, Reliable)
	void PullCharacter(FVector DeltaLocation, bool bSweep, FHitResult OutSweepHitResult, ETeleportType Teleport);

	void OnInvisibilityGained();
	void RemoveInvisibility() const;
	bool TryRemoveVoidAffliction(const FGameplayTagContainer& tags) const;

	float GetMaxSpeed() const;
	float GetAnimationWalkSpeed() const;
	void setIntendedAnimationSpeed(int16 speed);
	int16 getIntendedAnimationSpeed() const;
	virtual bool IsMoving() const;
	void RemoveActiveEffects() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual ABaseCharacter* GetMaster() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsFriendlyTowards(const ABaseCharacter* target) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool CanDamageTarget(const ABaseCharacter* target) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool CanHealTarget(const ABaseCharacter* target) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsHostileTowards(const ABaseCharacter* target) const;

	ETeamName GetCurrentTeam() const;


	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerApplyInvulnerability(float duration);

	void ApplyInvulnerability(float duration = 3.0f, bool bApplyCue = true) const;

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRemoveInvulnerability();

	void RemoveInvulnerability() const;

	bool HasCustomTargetableCapsules() const;
	UCapsuleComponent* GetClosestTargetableCapsule(const FVector &queryLocation) const;
	const TArray<TWeakObjectPtr<UCapsuleComponent>>& GetCachedTargetableCapsules() const;

	//////////////////////////////////////////////////////////////////////////

	FOnDeath OnDeath;
	
	FOnRevive OnRevive;
	

	//////////////////////////////////////////////////////////////////////////

	FOnStepMaterialChanged OnStepMaterialChanged;

	FOnModifierSurfaceChanged OnModifierSurfaceChanged; // D11.DB

	FOnOverlapMaterialChanged OnOverlapMaterialChanged;

	void SetStepMaterial(EMaterialTypeEnum material);

	void SetOverlapMaterial(EMaterialTypeEnum material);

	//////////////////////////////////////////////////////////////////////////

	virtual void HandleDeath();

	virtual void HandleRevive();

	virtual void HandleHeal(float amount);

	virtual void HandleDamageReceived(float amount);

	void HandleDamageTagReceived(float amount, const FGameplayTagContainer& tag);

	/** Called when the lifespan of an actor expires (if he has one). */
	virtual void LifeSpanExpired() override;

	virtual bool DissasembleCharacter();

	virtual void RemoveInstanceTracking();

	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterHeal(float amount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterDamageReceived(float amount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterDamageReceivedWithType(float amount, const FGameplayAttribute& type);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterDamageReceivedWithTag(float amount, const FGameplayTagContainer& tag);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterRevive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCharacterDeath();

	void OnFrozenSolid(const FGameplayTag tag, const int32 tagCount);

	void OnGhostWalk(const FGameplayTag tag, const int32 tagCount);
	void OnBubbled(const FGameplayTag tag, const int32 tagCount);
	virtual void OnLevitate(const FGameplayTag tag, const int32 tagCount);
	void OnDamageImmunity(const FGameplayTag tag, const int32 tagCount);

	UFUNCTION()
	void OnAttackedWhileBubbled(float damagedReceived);
	FDelegateHandle BubbledHandle;
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FORCEINLINE EAttackState GetAttackState() const { return AttackState; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetAttackState(EAttackState newState) {
		if (newState != AttackState) {
			AttackState = newState;
			OnRep_AttackState();
		}
	}

	UPROPERTY()
	FOnKilledOther OnKilledOther;
	
	FOnCharacterWalking OnCharacterWalking;

	FOnAttackStateChanged OnAttackStateChanged;

	FOnTeamChanged OnTeamChanged;

	FOnControllerReplicated OnControllerReplicated;

	UPROPERTY(BlueprintAssignable)
	FOnDamageImmunityChanged OnDamageImmunityChanged;

	//Delegate for *local* interruptions (i.e owning client).
	FCancelActions CancelActions;

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnEffectsAtLocation(UParticleSystem* emitter, class USoundCue* sound, bool attached);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Character")
	EMaterialTypeEnum GetStepMaterial();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Character")
	EMaterialTypeEnum GetOverlapMaterial();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Character")
	class USoundCue* OnLandedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Character")
	class USoundCue* OnFrozenLandedSound;

	UPROPERTY(Transient, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UDungeonsAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
	TSubclassOf<UInvulnerableGameplayEffect> InvunlerabilityEffect;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE class UEnchantmentComponent* GetEnchantmentComponent() const {
		return EnchantmentComponent;
	}

	TWeakObjectPtr<UCharacterAnimInstance> AnimInstance = nullptr; // @btlifecycle

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons|Character")
	class UBaseParticleAssetsComponent* Particles = nullptr;

	UFUNCTION(NetMulticast, Unreliable)
	void SpawnAttachedParticles(UParticleSystem* particle, FVector offset = FVector::ZeroVector, float duration = 0.f, bool fadeOut = true);

	virtual void SetCapsuleCollisionResponse(ESynchornizedCollisionResponse response);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetModifierSurface(EMaterialTypeEnum surface);

	TWeakObjectPtr<ABaseCharacter> GetLastInjuredBy() const;

	virtual void SteppedOnNewBlock(const FullBlock& block) { };
	virtual void SteppednNewBlock(const FullBlock& block) { };

	void SetLastInjuredBy(ABaseCharacter* lastInjuredBy);

	bool IsLocationReachable(const FVector& location) const;

	void MulticastPlayAnimationAsDynamicMontage(UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime = 0.f, float BlendOutTime = 0.2f, float InPlayRate = 1.f, int32 LoopCount = 1, float BlendOutTriggerTime = 0.f, float InTimeToStartMontageAt = 0.f, FPredictionKey key = FPredictionKey());
	void MulticastPlayMontage(UAnimMontage* MontageToPlay, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, float InTimeToStartMontageAt = 0.f, FPredictionKey predictionKey = FPredictionKey());
	void MulticastPlayMontageSectionWithBlendAtSectionPercentage(UAnimMontage* MontageToPlay, FName SectionName, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, float InTimeToStartMontageAt = 0.f, float BlendInTime = 0.0f, float SectionPercentage = 0.0f, FPredictionKey predictionKey = FPredictionKey());
	void MulticastJumpToSectionMontageWithPlayrate(FName SectionName, float InPlayRate = 1.f, UAnimMontage* Montage = NULL, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayAnimationAsDynamicMontagePacked(const FMontageData& data);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontagePacked(const FMontageData& data);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMontageSectionWithBlendAtSectionPercentagePacked(const FMontageData& data);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastJumpToSectionMontageWithPlayratePacked(const FMontageData& data);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopMontage(float InBlendOutTime, const UAnimMontage* Montage = NULL, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopAnimation(float InBlendOutTime, UAnimSequenceBase* Asset, FName SlotNodeName, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPauseMontage(const UAnimMontage* Montage = NULL, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastResumeMontage(const UAnimMontage* Montage = NULL, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastJumpToSectionMontage(FName SectionName, const UAnimMontage* Montage = NULL, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetPlayrateMontage(const UAnimMontage* Montage, float NewPlayRate = 1.f, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastStopSlotAnimation(FName slotName = NAME_None, float blendOutTime = 0.25f, FPredictionKey predictionKey = FPredictionKey());

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayMeleeAttackVariantAnimation(uint8 index, float animationPlayRate, FPredictionKey key);

	void PlayMontageSectionWithBlendAtSectionPercentage(UAnimMontage* MontageToPlay, FName SectionName, float InPlayRate, EMontagePlayReturnType ReturnValueType, float InTimeToStartMontageAt, float BlendInTime, float SectionPercentage);
	void StopSlotAnimation(FName slotName, float blendOutTime);
	class UAnimMontage* PlayAnimationAsDynamicMontage(UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime = 0.f, float BlendOutTime = 0.2f, float InPlayRate = 1.f, int32 LoopCount = 1, float BlendOutTriggerTime = 0.f, float InTimeToStartMontageAt = 0.f);	
	class UAnimMontage* PlayAnimationAsDynamicMontageLoopTo(UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime = 0.f, float BlendOutTime = 0.2f, float InPlayRate = 1.f, float BlendOutTriggerTime = 0.f, float InTimeToStartMontageAt = 0.f, float InTimeToLoopTo = 0.f);	
	float PlayMontage(UAnimMontage* MontageToPlay, float InPlayRate = 1.f, EMontagePlayReturnType ReturnValueType = EMontagePlayReturnType::MontageLength, float InTimeToStartMontageAt = 0.f);
	void StopMontage(float InBlendOutTime, const UAnimMontage* Montage);
	void StopAnimation(float InBlendOutTime, UAnimSequenceBase* Asset, FName SlotNodeName);
	void PauseMontage(const UAnimMontage* Montage);
	void ResumeMontage(const UAnimMontage* Montage);
	void JumpToSectionMontage(FName SectionName, const UAnimMontage* Montage);
	void SetPlayrateMontage(const UAnimMontage* Montage, float NewPlayRate);
	void PlayMeleeAttackVariantAnimation(uint8 index, float animationPlayRate);
	void JumpToSectionMontageWithPlayrate(FName SectionName, float InPlayRate, const UAnimMontage* Montage);

	UFUNCTION()
	void Kill() const;

	virtual void SetSharedCooldown(float activeCooldown, float passiveCooldown) {
		float now = GetWorld()->GetTimeSeconds();
		CanActFromSecondActive = now + activeCooldown;
		CanActFromSecondPassive = now + passiveCooldown;
		CanActSmallestCooldown = Math::min(activeCooldown, passiveCooldown);
	}

	virtual void ResetSharedCooldown() { CanActFromSecondActive = CanActFromSecondPassive = 0.f; }
	
	FORCEINLINE float GetCanActFromSecondActive() const { return CanActFromSecondActive; };
	FORCEINLINE float GetCanActFromSecondPassive() const { return CanActFromSecondPassive; };
	FORCEINLINE float GetCanActSmallestCooldown() const { return CanActSmallestCooldown; };	


	void SetLookAtActor(AActor* actor) {
		LookAtActor = actor;
		OnRep_LookAtActor();
	}

	void ResetLookAtActor() {
		LookAtActor.Reset();
		OnRep_LookAtActor();
	}
	 
	bool IsAlive() const;

	bool IsNotAlive() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool EnableOcclusionSilhouetting = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool EnableHealthBar = true;

	void SetNearMissSelection(const ECollisionResponse& nearMissResponse);

	void DisableNearMissSelection();
	void ResetNearMissSelection() const;	

	void ChangeTeam(ETeamName);

	void ChangeMaster(ABaseCharacter*);
	TArray<ABaseCharacter*> GetMinions() const;

	FOnMinionAcquired OnMinionAcquired;
	FOnMinionReleased OnMinionReleased;

	FOnWorldStateChanged OnWorldStateChanged;

	ECharacterWorldState GetWorldState() const;

	void SetWorldState(ECharacterWorldState);

	bool IsImmobile() const;

	UFUNCTION(BlueprintCallable)
	bool IsFrozenSolid() const;
	bool IsMeleeDisabled() const;

	AFrozenSolidIceCubeActor* GetCurrentIceCube() { return currentIceBlock; }

	UFUNCTION(BlueprintCallable)
	UPushVolumeReactiveComponent* GetPushVolumeReactiveComponent() const;

	UFUNCTION(BlueprintCallable)
	UPushVolumeReactiveComponent* GetOrCreatePushVolumeReactiveComponent(bool CreateEnabled = false);

	UHealthBarComponent* GetHealthBarComponent() const;

	UHealthComponent* GetHealthComponent() const;

	UFUNCTION(BlueprintCallable)
	class URagdollOnDeathComponent* GetRagdollOnDeathComponent() const;

	const FGuid& GetInstanceId_ServerOnly() const;

	void CancelAllActions() const;

	UFUNCTION(BlueprintCallable)
	bool IsTargetable() const;

	UFUNCTION(BlueprintCallable)
	void SetTargetable(bool targetable);
	
	UFUNCTION(BlueprintCallable)
	bool IsBlocking() const {
		return bIsBlocking;
	};

	UFUNCTION(BlueprintCallable)
	void SetIsBlocking(bool bBlocking) {
		bIsBlocking = bBlocking;
	};

	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	TOptional<FVector> GetFallFromLocation() const;
	void ResetFallFromLocation() { FallFromLocation.Reset(); };

	virtual void SetTimeToLive(float);
	virtual float GetTimeToLive() const;

	TWeakObjectPtr<class AGameBP> Game;

	UFUNCTION()
	void UpdatePushVolumeMovement(FVector pushForce);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetPushImmunity(bool bEnable);

	void EnablePushVolumes(bool bEnable);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	bool IsUnderwater() const; 

	virtual bool HitFlashOnCharacter(FLinearColor color, float duration, float startFraction /*= 1.f*/);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float CharacterVFXScale = 1.0f;

	UChildActorComponent* GetBackPackComponent()const {return backpackComponent;}
	
	void SetBackPackComponentAttached(bool AttachVal);

	void PostInitializeComponents() override;

protected:

	UPROPERTY()
	UChildActorComponent* backpackComponent;
	FName backpackSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintGetter = IsTargetable, BlueprintSetter = SetTargetable, Replicated, Category = "Dungeons")
	bool Targetable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter = IsBlocking, Category = "Dungeons")
	bool bIsBlocking = false;

	//If set to > 0, the character will call ::Kill after the specified time (in seconds)
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float InitialTimeToLive = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Team")
	ETeamName TeamName = ETeamName::World;

	UPROPERTY(ReplicatedUsing = OnRep_LookAtActor)
	TWeakObjectPtr<AActor> LookAtActor;

	UFUNCTION()
	void OnRep_LookAtActor();

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeons")
	void GetTargetableCapsules(TArray<UCapsuleComponent*> &outCapsules) const;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons", ReplicatedUsing = OnRep_AttackState)
	EAttackState AttackState;

	BlockID StepBlock = BlockID::AIR;
	EMaterialTypeEnum StepMaterial;
	EMaterialTypeEnum OverlapMaterial;

	TWeakObjectPtr<ABaseCharacter> LastInjuredBy;

	game::TilePtr LastValidTile = nullptr;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Dungeons")
	class UEnchantmentComponent *EnchantmentComponent;

	USceneComponent* GetDefaultAttachComponent() const override { return GetMesh(); }

	virtual TArray<ECustomTraceChannels> GetNearMissSelectionChannels();

	void AcquireMinion(ABaseCharacter*);
	void ReleaseMinion(ABaseCharacter*);
	
	void OnRep_Controller() override;

	void OnPushImmunityChanged(FGameplayTag, int32 tagCount);

	FCollisionResponseContainer DefaultCapsuleCollisionResponse;

	TOptional<FVector> FallFromLocation;

	struct BaseCharacterHitFlash
	{
		FLinearColor color;
		float duration = 0.0f;
		float startFraction = 0.0f;
	};

	TOptional<BaseCharacterHitFlash> NextHitFlash;

	void UpdateHitFlash();


private:
	UFUNCTION()
	void OnRep_AttackState();

	UFUNCTION()
	void OnRep_Master();

	UFUNCTION()
	void OnPushRecieverDeactivated();

	static bool AllowsNavigation(const AActor* actor);
	
	virtual USphereComponent* GetNearMissComponent() const;

	void BroadcastTeamChange(const ETeamName& previousTeam, const ETeamName& newTeam) const;

	FCollisionResponseContainer CachedCapsuleCollisionResponse;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float MaxSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float AnimationWalkSpeed = 250.0f;

	UPROPERTY(Replicated)
	int16 IntendedAnimationSpeed = 0;

	UPROPERTY(EditAnywhere, Category = "BaseCharacter | VoidTouched")
	FGameplayTagContainer tagsThatRemoveVoid;

	UPROPERTY()
	TArray<TWeakObjectPtr<USceneComponent>> componentsDisappeared;

	UFUNCTION()
	void OnRep_WorldState(ECharacterWorldState previousWorldState);

	UPROPERTY(ReplicatedUsing = OnRep_WorldState)
	ECharacterWorldState WorldState = ECharacterWorldState::InWorld;
	
	// Component added in blueprint
	TWeakObjectPtr<class UHealthComponent> CachedHealthComponent;
	TWeakObjectPtr<class URagdollOnDeathComponent> CachedRagdollOnDeathComponent;
	
	UPROPERTY()
	class UHealthBarComponent* HealthBarComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPushVolumeReactiveComponent* PushVolumeReactiveComponent;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient)
	class USimpleMovementComponent* PushVolumeMovementComponent;

	UPROPERTY()
	class UBlockTriggerComponent* BlockTriggerComponent;

	UPROPERTY()
	mutable TWeakObjectPtr<class USphereComponent> CachedNearMissComponent;

	UPROPERTY()
	TWeakObjectPtr<class UMassComponent> CachedMassComponent;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<AFrozenSolidIceCubeActor> FrozenSolidIceCubeActor;
	AFrozenSolidIceCubeActor* currentIceBlock;

	float CanActFromSecondActive = 0.f;
	float CanActFromSecondPassive = 0.f;
	float CanActSmallestCooldown = 0.f;
	
	FCollisionResponseContainer CachedNearMissCollisionResponse;

	UPROPERTY(ReplicatedUsing = OnRep_Master)
	TWeakObjectPtr<ABaseCharacter> Master;
	TArray<TWeakObjectPtr<ABaseCharacter>> Minions;

	// Let's see if this needs to be replicated or not
	FGuid InstanceGuid;

	FTimerHandle DelayedKillHandle;

	TArray<TWeakObjectPtr<UCapsuleComponent>> TargetableCapsules;
};
