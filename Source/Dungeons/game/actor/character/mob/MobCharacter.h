#pragma once

#include "game/actor/character/BaseCharacter.h"
#include "MobParams.h"
#include "world/entity/EntityTypes.h"
#include "game/util/ActorQuery.h"
#include "game/ReactsToHurt.h"
#include "game/level/sound/MusicSetTypes.h"
#include "AI/Navigation/NavAgentInterface.h"
#include "MobCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnpossessed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnObjectiveActivated);

class USphereComponent;

static const float MobDefaultResurrectTime = 1.8f;

UENUM(Meta = (Bitflags))
enum class EMobSpawnProperties : uint8 {
	 EM_Revealed = 1 << 0
	,EM_Underling = 1 << 1
	,EM_BlockMusicOverride = 1 << 2
};
ENUM_CLASS_FLAGS(EMobSpawnProperties)

UCLASS()
class DUNGEONS_API AMobCharacter : public ABaseCharacter, public IReactsToHurt {
	GENERATED_BODY()

public:
	AMobCharacter(const FObjectInitializer& ObjectInitializer);

	void PreRegisterAllComponents() override;
	void PreInitializeComponents() override;

	void Falling() override;
	void Landed(const FHitResult& hit) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& outLifetimeProps) const override;
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	virtual void Tick(float DeltaSecs) override;

	void UpdateMobHurt();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void UnPossessed() override;
	bool IsFriendlyTowardsPlayers() const;
	bool IsHostileTowardsPlayers() const;

	void SetSharedCooldown(float activeCooldown, float passiveCooldown) override;
	void ResetSharedCooldown() override;

	float CalculateAbsoluteMoveSpeed(float absoluteSpeed) const;
	float CalculateRelativeMoveSpeed(float relativeFraction) const;

	void RespawnAt(const FVector& location, bool vfx = false) const;

	virtual class USphereComponent* GetNearMissComponent() const override { return  NearMissTargetSelectCollider; };

	UPROPERTY(Category = "Dungeons|Mob", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* BlobShadow;
	
	UPROPERTY(Category = "Dungeons|Mob", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DebugSphereInner;
	UPROPERTY(Category = "Dungeons|Mob", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* DebugSphereOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Mob")
	class USphereComponent* NearMissTargetSelectCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* stepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* hurtSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* blockSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* deathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* idleSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* preSummonSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* shieldBreakSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	float idleSoundDelay = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Mob")
	float DeathSaturation = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Mob")
	float DeathSaturationSecondsLapse = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Behavior")
	bool NoClip = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Behavior")
	bool NoXP = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Behavior")
	bool NoSouls = false;
	
	UPROPERTY(BlueprintReadWrite, Transient, Category = Audio)
	class UAncientMobAudioComponent* AncientMobAudioComponent = nullptr;


	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnpossessed OnUnpossessedByController;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnObjectiveActivated OnObjectiveActivated;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	class AAIController* AiController() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasTag(const FString& tag) const;

	bool HasTag(size_t tag_hash) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void TrySpawnXPOrbs(AActor* OrbTarget, float Damage);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons|Mob")
	EntityType EntityType;

	FMobParams MobParams;

	void IssueWarning(AActor* actor);

	void HandleShieldChange(bool hasShield);

	bool IsEnchanted() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons|Mob Ranges Hint")
	float OffensiveRange = 1500;

	UPROPERTY()
	class UBehaviorComponent* Behavior;

	UPROPERTY()
	class UAudioComponent* HurtAudioComponent;

	UPROPERTY()
	class UAudioComponent* DeathAudioComponent;

	UPROPERTY()
	class UAudioComponent* IdleAudioComponent;

	UPROPERTY()
	class UGlobalHealthBarComponent* GlobalHealthBarComponent;

	class UGlobalHealthBarComponent* GetGlobalHealthBarComponent() const;	

	void HandleDeath() override;
	void HandleRevive() override;

	void Resurrect(); // D11.DB

	bool IsMoving() const override;

	virtual void RemoveInstanceTracking() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsEventMob() const;
	bool IsAncient() const;
	void SetEventMobRevealed(bool reveal);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsRaidCaptain() const;

	bool HasMusicOverride() const;
	bool IsRevealed() const;
	bool IsUnderling() const;

	UFUNCTION(BlueprintNativeEvent)
	void PlayerEnteredVisibleRange();
	virtual void PlayerEnteredVisibleRange_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void PlayerLeftVisibleRange();
	virtual void PlayerLeftVisibleRange_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void PlayerInVisibleRangeChanged(bool Visible);

	UFUNCTION(BlueprintNativeEvent)
	void MobAtExtremePlayerRangeChanged(bool IsAtExtremeRange);
	virtual void MobAtExtremePlayerRangeChanged_Implementation(bool IsAtExtremeRange);
	

	virtual bool CanDamageTarget(const ABaseCharacter* target) const override;
	virtual bool IsHostileTowards(const ABaseCharacter* target) const override;


	UPROPERTY(VisibleAnywhere, Category = "Dungeons")
	bool mPlayerVisible = true;

	TArray<AActor*, TInlineAllocator<4>> mPlayersVisible;

	//Do player distance Check
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool mPlayerDistanceCheck = true;

	bool mExtremeRangeFromPlayers = false;

	//These are the players targetting us.
	TArray<TWeakObjectPtr<APlayerCharacter>> cachedTargetees;

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeons")
	EMusicSequenceState GetMusicSequenceState() const;

	void SetIsUnderlingMob(bool isUnderling);
	void SetBlockMusicOverride(bool blockMusicOverride);

	// E.g. mission end or per player tracked stats.
	bool ShouldBeIncludedInStats_ServerOnly = true;

	void SetResurrectTime(float time);
	void SetForceResurrection(bool res);
	void SetIsRequiredForObjective(bool);
	bool IsRequiredForObjective() const;

	void CancelResurrection();

	UFUNCTION()
	void OnUnderwaterTagChange(const FGameplayTag Tag, int32 Count);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool HideOnDeath = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool AllMobMeshesUseAttachParentBounds = true;

	FVector GetMoveGoalOffset(const AActor* MovingActor) const override;
	void GetMoveGoalReachTest(const AActor* MovingActor, const FVector& MoveOffset, FVector& GoalOffset, float& GoalRadius, float& GoalHalfHeight) const override;

private:
	void PlayIdleSound(float DeltaSecs);
	void RefreshMovementCooldownState() const;

	void ReactToHurt_Implementation(AActor* byWhom, float magnitude) override;

	bool IsAnyCooldownActive() const;

	UStaticMeshComponent* GetStaticMeshComponent() const;

	TOptional<class UMusicOverrideComponent*> MusicOverridePtr;

	FTimerHandle CooldownStateRefreshHandle;

	bool CanResurrect();
	void OnResurrection();
	FTimerHandle ResurrectionHandle;

	ECollisionEnabled::Type CachedCapsuleCollisionType = ECollisionEnabled::QueryAndPhysics;

	TArray<TWeakObjectPtr<UActorComponent>> mExtremeRangedTickingComponents;

	float mIdleAudioCountDown = 0.0f;

	bool mExtremeRangeWasActorTickEnabled = false;
	bool mExtremeRangeWasControllerActorTickEnabled = false;

	bool bIsRequiredForObjective = false;

	UPROPERTY(Replicated, meta=(Bitmask, BitmaskEnum="EMobSpawnProperties"))
	uint8 mMobReplicatedFlags;

	bool bMobHurt = false;

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Resurrection")
	float ResurrectTime = MobDefaultResurrectTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Resurrection", DisplayName = "Force Resurrection")
	bool bForceResurrection = false;

	//unchecked respawns at death location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Resurrection")
	bool RespawnAtStartLocation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Resurrection")
	bool RespawnUnenchanted = false;
};
