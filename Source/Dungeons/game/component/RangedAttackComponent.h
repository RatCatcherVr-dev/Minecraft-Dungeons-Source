#pragma once

#include "game/component/AttackComponent.h"
#include "DrawDebugHelpers.h"
#include "game/actor/item/Arrow.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/item/stats/ItemStatsTypes.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"
#include "game/actor/item/ThrowablePropActor.h"
#include "RangedAttackComponent.generated.h"

class UItemSlot;
class APlayerCharacter;
class ARangedWeaponGearItemInstance;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerRangedAttack, const APlayerCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerBeginRangedAttack, const APlayerCharacter*);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginRangedAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoSlotChanged, UItemSlot*, ammoSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRangedWeaponChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAvailableAmmoSlotsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRangedAttackInsuffientArrows);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoRangedWeaponWasEquipped);
DECLARE_MULTICAST_DELEGATE(FOnDurationPassed)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChargeCompleted, const float);
DECLARE_MULTICAST_DELEGATE(FOnAttackStopped);

USTRUCT()
struct DUNGEONS_API FProjectileSpawnData {
	GENERATED_USTRUCT_BODY()

	FProjectileSpawnData() :rangedAttackSpeedMultiplier(1.f), attackTarget(nullptr), ammoType(game::item::type::Arrow.getId()) {};
	FProjectileSpawnData(float speed, AActor* target, const FItemId&, const FVector& location, const FVector& direction, uint8 count, int32 inSeed);

	enum FLAGS {
		AttackSpeedFlag = 0x1,
		AttackTargetFlag = 0x2,
		AmmoTypeFlag = 0x4
	};

	float rangedAttackSpeedMultiplier;
	AActor* attackTarget;
	FSerializableItemId ammoType;
	FVector_NetQuantize10 spawnLocation;
	FVector_NetQuantizeNormal spawnDirection;
	uint8 arrowCountAtStart;
	int32 seed;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FProjectileSpawnData> : public TStructOpsTypeTraitsBase2<FProjectileSpawnData>
{
	enum
	{
		WithNetSerializer = true
	};
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FRangedAttackProjectileSpawnDescription {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABaseProjectile> Projectile = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int NumberOfProjectiles = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelaySeconds = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DelaySecondsPerProjectile = 0.f;

	//Defines the angle span (centered around target vector + AngleOffset)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AngleSpan = 0.f;

	//Angle offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AngleOffset = 0.f;

	//Max angle offset factor for each projectile
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxRandomizedAngleFactor = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool NonRandomFirstArrow = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector ProjectileOffset = FVector(0, 0, 0);

	float GetTotalDelay() const {
		return DelaySeconds + Math::max(0, (NumberOfProjectiles - 1)) * DelaySecondsPerProjectile;
	};
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FRangedAttack {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Range = 750.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UAnimSequenceBase* Animation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Slot = FName(TEXT("UpperBody"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName SpawnSocketLocation = NAME_None;

	/**
	The sequence of projectiles to spawn. Will be executed in order with the delays specified INBETWEEN entries.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FRangedAttackProjectileSpawnDescription> ProjectileDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FRangedAttackProjectileSpawnDescription> ChargedProjectileDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool Interruptable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float AttackRateSeconds = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ChargeTimeSecond = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ChargeVelocityMultiplier = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ChargeDamageMultiplier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ActiveCancelPorintFactor = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProjectileVelocityFactor = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ProjectileDamageFactor = 1.f;

	const TArray<FRangedAttackProjectileSpawnDescription>& GetCurrentProjectileDefinitions(bool isCharged) const {
		if (isCharged && ChargedProjectileDefinition.Num() > 0) {
			return ChargedProjectileDefinition;
		}
		
		return ProjectileDefinition;
	}

	bool CanChargeUp() const {
		return ChargeTimeSecond > 0.0f;
	}

	bool IsAnyProjectileHoming(bool isCharged) const;

	void PreCacheProjectiles(UWorld* world)const;
};

UENUM()
enum class EPitchMode : uint8 {	
	Normal,
	Parallel,
	Adjust,
	Random,
	ParallelSpawn,
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API URangedAttackComponent : public UAttackComponent {
	GENERATED_BODY()

public:
	URangedAttackComponent();

	float GetAttackRange() const override;
	float GetAttackTimeTotal() const;
	float GetBiggestAttackAngleSpan() const;

	bool HasAmmo() const;

	bool CanAttack(AActor* attackTarget = nullptr) const override;

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void OnRangedDamageDealt(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation);

	FPredictionKey AttackLocal(AActor* attackTarget = nullptr) override;

	bool BeginAttack(const FAttackAimProvider& targetProvider) override;
	void BeginAttackAnimation(FSharedPredictionContext);
	void EndAttack(AActor* attackTarget = nullptr) override;

	bool IsAttacking() const override { return IsContinuousAttack; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBeginAttack(FRotator attackRotation, FPredictionKey predictionKey);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndAttack(FPredictionKey predictionKey);

	UFUNCTION(BlueprintCallable)
	TArray<UItemSlot*> GetAvailableAmmoSlots() const;
	void ResetAttackState();

	void BeginChargeUp(FSharedPredictionContext);
	void ChargeComplete(FSharedPredictionContext);

	void StopAttack() override;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopAttack() ;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopAttackAnimation(FPredictionKey predictionKey);

	void StopAttackAnimation(FSharedPredictionContext);

	void SetRangeAttackVariant(int attackIndex);
	int GetRangeAttackVariant() { return AttackIndex; }
	void ResetRangeAttackVariant();
	int GetNumAttackVariants() { return AlternateRangedAttacks.Num(); }

	const FRangedAttack& AttackDefinition() const { return RangedAttackDefinition;  }
	FRangedAttack& AttackDefinitionByRef() { return RangedAttackDefinition;  }

	const FRangedAttack& GetConfiguredAttackDefinition() const { return DefaultRangedAttack; }

	FOnPlayerRangedAttack OnPlayerRangedAttack;
	FOnPlayerBeginRangedAttack OnPlayerBeginRangedAttack;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnBeginRangedAttack OnBeginRangedAttack;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|RangedAttackComponent")
	EPitchMode pitchMode = EPitchMode::Normal;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|RangedAttackComponent")
	FFloatRange pitchRange = FFloatRange { 5.f, 40.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent")
	TArray<ESlotType> AmmoTypes;

	bool IsAttackInProgress() const override;
	
	bool InAttackOrientation(AActor* attackTarget) const override;

	int IssuedAttackCounter = 0;

	bool EndAttackIsAttackExecution() const;

	float GetEstimatedProjectileSpeed(const FItemId& ammoType) const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnRangedAttackInsuffientArrows OnRangedAttackInsuffientArrows;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNoRangedWeaponWasEquipped OnNoRangedWeaponWasEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnRangedWeaponChanged OnRangedWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAvailableAmmoSlotsChanged OnAvailableAmmoSlotsChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAmmoSlotChanged OnAmmoSlotChanged;

	FOnDurationPassed OnDurationPassed;

	FOnChargeCompleted OnChargeComplete;

	FOnAttackStopped OnAttackStopped;

	void OnInsuffientArrows() const;
	void OnNoRangedWeaponEquipped() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasRangedWeaponEquipped() const;

	UFUNCTION()
	void AmmoSlotChangedCount(const UItemSlot* slot);
	void AmmoSlotChangedCount(UItemSlot* slot);

	ARangedWeaponGearItemInstance* GetCurrentWeapon() const { return RangedWeapon; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UItemSlot* GetCurrentAmmoSlot() const;

	// D11.DB - Had to make this public to fix autotagetting with crossbows.
	bool ShouldUseChargeUpAttack() const;

	void ResetProjectile(ABaseProjectile* projectile);
	ABaseProjectile* GetProjectileForSpawnId(uint8 id);
	TOptional<uint8> GetProjectileId(const ABaseProjectile* projectile) const;

	static void TrySetDistanceBasedVelocity(ABaseProjectile* projectile, const FVector& from, const FVector& to);
	static void SetRandomizedPitchBasedVelocity(ABaseProjectile* projectile, const FFloatRange& pitchRange, const FRandomStream& randStream);

	void ToggleAlwaysAttack(bool on) { AlwaysAttack = on; }
	void ToggleShouldConsumeAmmo(bool shouldConsume) { bShouldConsumeAmmo = shouldConsume; }
	void ToggleCharged(bool charged) { IsCharged = charged; }

	bool bRemoveInvisibilityAfterAttack = true;

	UFUNCTION(BlueprintCallable)
	bool AddThrowable(AThrowablePropActor* throwable);
	UFUNCTION(BlueprintCallable)
	bool CanAddThrowable(EThrowableType type);
	UFUNCTION(BlueprintCallable)
	void RemoveThrowable(AThrowablePropActor* throwable);
	UFUNCTION(BlueprintCallable)
	AThrowablePropActor* GetAndRemoveTopThrowable();

	const TArray<AThrowablePropActor*>& GetHeldThrowables() const { return HeldThrowables; }

	void ConsumeAmmo(TSubclassOf<ABaseProjectile> projectileDefinition);

	void OverrideOrigin(FVector);
	void ResetOrigin();

	UFUNCTION(BlueprintCallable)
	float GetWeaponChargeTime();

protected:
	void EndAttack_Internal(FSharedPredictionContext predictionContext);
	virtual void PerformAttack(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, FItemId ammoType, float rangedAttackSpeedMultiplier, int32 seed);

	UFUNCTION()
	void OnWeaponSlotUpdated(UItemSlot* slot);
	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnProjectileMulticast(const FProjectileSpawnData& spawnData, bool isCharged);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons|RangedAttackComponent")
	FRangedAttack DefaultRangedAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons|RangedAttackComponent")
	TArray<FRangedAttack> AlternateRangedAttacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|RangedAttackComponent")
	bool RecalculatePerProjectile = false;

	virtual FVector CalculateHomingTargetOffset(const FVector& emitLocation, const AActor* target, TSharedPtr<FRandomStream> randStream) const;
	virtual FVector CalculateAttackLocation(ABaseCharacter* attacker, const FVector& emitLocation, const FVector& emitDirection, AActor* attackTarget, const FRangedAttackProjectileSpawnDescription& definition, const FItemId& ammoType) const;
	virtual FVector GetAttackerBaseEmitVector(ABaseCharacter* attacker) const;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	TArray<AThrowablePropActor*> HeldThrowables;

	bool IsThrowableSlot(ESlotType SlotTypeID);

	uint8 AuthorativeArrowCount = 0;

	UPROPERTY(Replicated)
	bool IsCharged = false;

	TOptional<FVector> overrideOrigin;

	ARangedWeaponGearItemInstance* RangedWeapon = nullptr;
	
private:
	bool AlwaysAttack = false;
	bool bShouldConsumeAmmo = true;

	void AttackEnded(bool completed, TSubclassOf<ABaseProjectile> projectileDefinition);

	void LaunchHeldThrowablePlayerForwardDir();

	TMap<ABaseProjectile*, uint8> SpawnMap;

	void OnAutoChargeChanged(FGameplayTag tag, int32 count) { AutoCharge = count > 0; };

	void SetAmmoSlot(TWeakObjectPtr<UItemSlot> slot);
	void DetermineAmmoSlot();
	void PreCacheProjectileClasses();

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UItemSlot>> AvailableAmmoSlots;

	UPROPERTY(Transient)
	TWeakObjectPtr<UItemSlot> AmmoSlot = nullptr;

	float GetProjectileDefinitionSpeed(int index, const FItemId& ammoType) const;
	
	uint8 TemporaryAttackArrowCount = 0;

	FTimerHandle AttackStateResetHandle;
	FTimerHandle DelayedEndAttackHandle;
	FTimerHandle DelayBeginChargeHandle;
	FTimerHandle ProjectileSpawnHandle;	
	FTimerHandle ProjectileSpawnArcDelayHandle;		
	FTimerHandle AttackDurationTimerHandle;

	void OnAttackDurationPassed();

	bool IsContinuousAttack = false;


	UPROPERTY(Replicated)
	bool bCanShoot = true;
	bool AutoCharge = false;

	UFUNCTION()
	void OnRep_AttackIndex();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_AttackIndex)
	int AttackIndex = -1;

	bool bHasSpawnedProjectilesThisAttack = false;

	UPROPERTY(Replicated)
	FRangedAttack RangedAttackDefinition;

	FAttackAimProvider CachedTargetProvider;
	
	void SpawnProjectileForIndex(int index, int attackIndex, AActor* attackTarget, FVector spawnLocation, FVector spawnDirection, bool isCharged, TSubclassOf<ABaseProjectile> projectileOverride, float attackSpeedMultiplier, FPredictionKey key, FItemId ammoType, TSharedPtr<FRandomStream> randstream);
	
	void ExecuteProjectileDefintion(int index, int projectileIndex, AActor* attackTarget, FVector spawnLocation, FVector spawnDirection, bool isCharged, TSubclassOf<ABaseProjectile> projectileOverride, float attackSpeedMultiplier, FPredictionKey key, FItemId ammoType, TSharedPtr<FRandomStream> randStream);
	
	void ExecuteProjectileDefinitionIndex(TSubclassOf<ABaseProjectile> projectileDefintion, const FVector& spawnLocation, bool isCharged, const FRotator& spawnRotation, ABaseCharacter* attacker, AActor* attackTarget, FVector attackLocation, TSharedPtr<FRandomStream> randStream);	

	void SetRangedAttackDefinitionVariant(int attackIndex);

public:
	static ABaseProjectile* SpawnProjectileDeferred(TSubclassOf<ABaseProjectile> projectileDefintion, float ItemPower, const FTransform& transform, ABaseCharacter* attacker, const FRangedAttack& rangedAttackDefinition, bool IsCharged, bool canTriggerEnchantments, TOptional<float> sourceItemPower = {});
	static float GetRangedAttackStat(const FRangedAttack& AttackDefinition, TSubclassOf<ABaseProjectile> ProjectileClass, EItemStats stat);
};
