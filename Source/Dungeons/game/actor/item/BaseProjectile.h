#pragma once

#include "GameFramework/Actor.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <GameplayTagContainer.h>
#include <GameplayEffect.h>
#include "game/component/HealthComponent.h"
#include <Optional.h>
#include "game/item/ItemType.h"
#include "game/actor/item/Soul.h"
#include "Attachable.h"
#include "util/Algo.h"
#include "game/component/DungeonsProjectileMovementComp.h"
#include "BaseProjectile.generated.h"


class UHealthComponent;
class ABaseProjectile;
class ABaseProjectileProp;

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsProjectile, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOverlapEnd, AActor*, AActor*);

UENUM(BlueprintType)
enum class EArrowType : uint8
{
	Land,
	Water,
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FProjectileHitResponse 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Projectile")
	bool bounce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Projectile")
	FVector bounceDirection = FVector::ZeroVector;
};

UCLASS(Abstract)
class DUNGEONS_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
public:
	
	struct BaseProjectileDefaults
	{
		FPushback					pushback;
		FGameplayTag				DamageType;
	
		bool						IsHoming = false;
		bool						DestroyOnHit = true;
		bool						bShouldHitTerrain = true;
		bool						AffectTargetOnce = true;
		bool						IgnoreFriendlyFire = false;
		bool						enableDebugVisuals = false;
		bool						ProjectileMovementComponent_bRotationFollowsVelocity = true;
		bool						ProjectileMovementComponent_bShouldBounce = false;
		bool						IsCharged = false;
		bool						bCanTriggerEnchantments = false;

		float						CollisionPhysicsForce = 1500.f;
		float						TimeThreshold = 0.5f;
		float						HomingThreshold = -1.0f;
		float						VelocityFactor = 1.f;
		float						MaxProjectileLifeTime = 0.0f;
		float						MaxDist5Degrees = 400.f;
		float						MaxDist40Degrees = 600.f;
		float						damage = 100;
		float						stunMultiplier = 1.f;
		float						ProjectileMovementComponent_InitialSpeed = 3000.f;
		float						ProjectileMovementComponent_MaxSpeed = 3000.f;
		float						ProjectileMovementComponent_ProjectileGravityScale = 0.f;
		float						WeaponItemPower = 1.f;

		float DamageFactor = 1.f;
		float DamageFactorMultiplier = 1.f;
		float pushbackMultiplier = 1.f;
	};


	ABaseProjectile(const FObjectInitializer& ObjectInitializer);

	void Tick(float DeltaSecs) override;

	/** Called once this actor has been deleted */
	virtual void Destroyed() override;

	/** Called when the Actor is outside the hard limit on world bounds, we dont want this as they are cached */
	virtual void OutsideWorldBounds() override {};

	/** Called when the actor falls out of the world 'safely' (below KillZ and such), we dont want this as they are cached  */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override {};

	/**
	 * Called after the C++ constructor and after the properties have been initialized, including those loaded from config.
	 * mainly this is to emulate some behavior of when the constructor was called after the properties were initialized.
	 */
	virtual void PostInitProperties() override;

	UPROPERTY(Category = "Dungeons|Projectile", Transient, BlueprintReadOnly, Replicated)
	TWeakObjectPtr<AActor> Target;

	UPROPERTY(Category = "Dungeons|Projectile", Transient, BlueprintReadOnly)
	FVector TargetOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|Projectile")
	FPushback pushback;	

	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons", BlueprintReadWrite, Replicated)
	class UDungeonsProjectileMovementComp* ProjectileMovementComponent;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Blueprintcallable)
	bool GetInstigatorHadAuthority() const;

	bool InstigatorHadAuthority = false;

	bool IsEnchanted() const;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool IsHoming = false;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float CollisionPhysicsForce = 1500.f;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite, Replicated)
	bool DestroyOnHit = true;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool bShouldHitTerrain = true;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool AffectTargetOnce = true;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool IgnoreFriendlyFire = false;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool enableDebugVisuals = false;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float TimeThreshold = 0.5f;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float HomingThreshold = -1.0f;

	/** Whether or not the projectile should have its speed modulated by ranged weapon speed factor. Only applies if spawned by a ranged weapon. */
	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	bool RespectWeaponProjectileVelocityFactor = true;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ApplyEffectOnHit;

	UPROPERTY(Transient)
	TArray<FGameplayEffectSpec> GameplaySpecsToApply;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABaseProjectileProp> ProjectilePropClass;
	
	UPROPERTY(Category = "Dungeons|Projectile", BlueprintReadOnly, Replicated)
	float VelocityFactor = 1.f;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float MaxProjectileLifeTime = 0.0f;

	float CurrentProjectileLifetime = 0.0f;

	UPROPERTY(Category = "Dungeons|Projectile", BlueprintReadOnly, Replicated)
	bool IsCharged = false;

	UPROPERTY(Replicated)
	bool bCanTriggerEnchantments = false;

	UPROPERTY(Category = "Dungeons|Projectile | Physics", EditDefaultsOnly, BlueprintReadWrite)
	bool bAffectedByCustomPhysics = true;

	UPROPERTY(Category = "Dungeons|Projectile | Physics", EditDefaultsOnly, BlueprintReadWrite)
	EArrowType ArrowType = EArrowType::Land;

	UFUNCTION(BlueprintCallable)
	int GetNumHits() const;

	float GetEnvironmentSpeedMultiplier();

	/** The distance on the ground plane that the bullet traveled when launched at 5 degrees. Used only for gravity affected projectiles. */
	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float MaxDist5Degrees = 400.f;

	/** The distance on the ground plane that the bullet traveled when launched at 40 degrees. Used only for gravity affected projectiles. */
	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float MaxDist40Degrees = 600.f;

	void SetHomingTarget(TWeakObjectPtr<AActor> target, const FVector& targetOffset = FVector::ZeroVector)
	{
		Target = target;
		TargetOffset = targetOffset;
	}

	void SetVelocityFactor(float newVelocityFactor)
	{
		VelocityFactor = newVelocityFactor;
	}

	void SetDamageFactor(float newDamageFactor){
		DamageFactor = newDamageFactor;
	}

	void MultiplyDamageFactor(float DamageFactorMul) {
		DamageFactor *= DamageFactorMul;
	}


	void SetDamageFactorMultiplier(float newDamageFactorMultiplier) {
		DamageFactorMultiplier = newDamageFactorMultiplier;
	}


	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectile")
	float GetDamageFactor() const {
		return DamageFactor * DamageFactorMultiplier;
	}

	void SetPushbackMultiplier(float multiplier)
	{	
		pushbackMultiplier = multiplier;
	}

	void AddPreviouslyHitActor(AActor* actorToDamage) { PreviouslyHitActors.Add(actorToDamage); }

	void RemovePreviouslyHitActor(AActor* actor) { PreviouslyHitActors.Remove(actor);  }

	float GetDamage() const;

	float GetExplosionDamage() const;

	float GetExplosionRadius() const;

	void SetDamage(float newDamage);
	
	float GetStunMultiplier() const;

	void EmptyPreviouslyHitActors();

	UFUNCTION()
	void SetDestroyOnHit(bool destroy);

	virtual void ResetProjectile();

	virtual void DisableCollisions();

	virtual void LaunchProjectile(AActor* pOwner);
	virtual void EnableProjectileCollisions(bool Val = true);

	virtual TOptional<FItemId> GetProjectileItemType() const { return {}; }

	TOptional<FGameplayEffectSpec> GetCachedOwnerGameplayEffectSpec() const;
	TOptional<FGameplayEffectSpec> GetCachedNoOwnerGameplayEffectSpec() const;
	const FGameplayEffectSpec& GetOrCreateNoOwnerGameplayEffectSpec(const UAbilitySystemComponent* targetComponent);

	void SetGameplayEffectSpec(const FGameplayEffectSpec& spec);

	const FGameplayTag& GetDamageType() const { return DamageType; }

	FOnOverlapEnd OnOverlapEnded;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly)
	float ForcedExpireDistance = 4000.f;	

	unsigned SpawnRecursionCounter = 0;

	bool expired = false;

	void ForceExipire() { expired = true;  }

	UPROPERTY(Instanced, VisibleDefaultsOnly, Category = "Dungeons|Projectile")
	class UImpactActionHandler* ImpactActionHandler = nullptr;
	
	TOptional<float> SourceItemPower;
	float WeaponItemPower;
protected:
	void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeons|Projectile")
	void ToggleCharged(bool charged);

	UFUNCTION(BlueprintNativeEvent, Category = "Dungeons|Projectile")
	void ToggleMagical(bool magical);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectile")
	bool ShouldIgnoreCollision(class AActor* OtherActor) const;

	void DealDamage(AActor* actorToDamage, UHealthComponent* healthComponent);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectile")
	FVector GetHomingTargetLocation();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Dungeons|Projectile")
	void ProcessHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FVector& HitLocation, const FVector& HitNormal, FProjectileHitResponse &hitResponse);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons|Projectile")
	void OnProjectileReset();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons|Projectile")
	void OnProjectileLaunch(AActor* pOwner);

	UFUNCTION(BlueprintCallable)
	bool IsMagical() const { return !bCanTriggerEnchantments; }

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(AActor* overlappedActor, AActor* otherActor);

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float damage = 100;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	float stunMultiplier = 1.f;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly)
	FGameplayTag DamageType;

	UPROPERTY()
	TArray<const AActor*> PreviouslyHitActors;

	int numHits = 0;

	float pushbackMultiplier = 1.f;
	
	TOptional<FGameplayEffectSpec> CachedOwnerGameplayEffectSpec;	
	TOptional<FGameplayEffectSpec> CachedNoOwnerGameplayEffectSpec;

	TOptional<FGameplayEffectSpec> CachedApplyOnHitGameplayEffectSpec;

	bool bIsUnderwater;
private:

	AActor* FindEffectCauser(AActor* pOwner);

	UPROPERTY(Replicated)
	float DamageFactor = 1.f;
	
	//vrak: I don't like this - but im doing it anyway. I really want to make sure they don't overwrite each other.
	float DamageFactorMultiplier = 1.f;

	FVector spawnLocation;

	BaseProjectileDefaults		mDefaultValues;

	bool WillMiss = false; // D11.DB - For the Blind enchantment.

	void PreSave(const class ITargetPlatform * TargetPlatform) override;

	UPROPERTY()
	float SerializedExplosionDamage;

	UPROPERTY()
	float SerializedExplosionRadius;
};


//Base Class for props from projectiles
UCLASS(Abstract, BlueprintType)
class DUNGEONS_API ABaseProjectileProp : public AActor, public IAttachable
{
	GENERATED_BODY()
public:

	/** Called when the Actor is outside the hard limit on world bounds, we dont want this as they are cached */
	virtual void OutsideWorldBounds() {};

	/** Called when the actor falls out of the world 'safely' (below KillZ and such), we dont want this as they are cached  */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) {};

	virtual void InitialiseProp(bool bCharged, float fPropLifetime) {};

	virtual void Reset() {};

	virtual void DelayedExplosion(float delay, float radius, UAbilitySystemComponent*, const TOptional<FGameplayEffectSpec>&, const FPushback&, FGameplayTag);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSecs) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeons | Attachable")
	void AttachedToActorHidden(AActor* attachedToActor);
	virtual void AttachedToActorHidden_Implementation(AActor* attachedToActor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float LifeTime = 0.0f;

	/** Enabling this will make the projectilemanager re-use the oldest prop actor when the maximum spawn limit for this type has been reached */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons | ProjectileActorManager")
	bool ReplaceOldInstances = false;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginPulse(float ExplosionDelay);

	UFUNCTION(BlueprintImplementableEvent)
	void OnExplode();

	virtual void ExplodeNative(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, const FPushback& pushback, FGameplayTag cue);

private:
	void Explode(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, FPushback pushback, FGameplayTag cue);
};




UCLASS()
class DUNGEONS_API UBaseProjectileDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBaseProjectileDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UBlindProjectileDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBlindProjectileDamageGameplayEffect();
};
UCLASS()
class DUNGEONS_API UNoOnwerProjectileDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UNoOnwerProjectileDamageGameplayEffect();
};
