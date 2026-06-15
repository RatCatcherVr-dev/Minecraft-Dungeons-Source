#pragma once

#include <GameplayTagContainer.h>
#include "util/Algo.hpp"
#include "game/util/Pushback.h"
#include <GameplayEffect.h>
#include "game/actor/character/BaseCharacter.h"
#include "ImpactActionHandler.generated.h"

class ABaseProjectile;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnImpactActorSpawned, AActor*, const FImpactInfo&);
DECLARE_DELEGATE_RetVal_TwoParams(FTransform, FTransformGenerator, const ABaseProjectile*, const FImpactInfo&);

namespace impactaction {
	void ExplodeProjectile(UAbilitySystemComponent* instigator, FGameplayEffectSpec& spec, float radius, FGameplayTag explosionCue, AActor* origin, const FPushback& push, int32* OUT_MobKillCount = nullptr);
}

UENUM()
enum class EImpactTriggerType : uint8 {
	//Only trigger on the final impact
	Final,
	//Trigger on every impact
	Always
};

UENUM()
enum class EEffectLevelSource : uint8 {
	//The weapon that fired the projectile
	RangedWeapon,
	//Uses the item power of the source of the projectile, if there was a source. Falls back to ranged weapon power if it fails.
	SourceItem,
};

UENUM(BlueprintType)
enum class ESpawnActorDefaultTransform : uint8 {
	//Just copies projectile transform
	ProjectileTransform,
	//Location is set to the impact location, rotation is 0 and scale is 1.
	ImpactLocation,
	//Location is set to feet of impacted character, rotation is 0 and scale is 1. If no character is impacted it will fall back to ImpactLocation
	BelowCharacter
};

UENUM(BlueprintType)
enum class ESpawnActorSnappingMode : uint8 {
	//No snapping to grid
	None,
	//Snaps in the X and Y dimensions but leaves Z
	SnapXY,
	//Snaps all dimension
	Snap
};


namespace impactaction {
	void ExplodeProjectile(UAbilitySystemComponent* instigator, FGameplayEffectSpec& spec, float radius, FGameplayTag explosionCue, AActor* origin, const FPushback& push, int32* OUT_MobKillCount/* = nullptr*/);
	float GetItemPower(const ABaseProjectile* projectile, EEffectLevelSource source);
}

USTRUCT()
struct DUNGEONS_API FImpactInfo {
	GENERATED_BODY()

	bool bDidHitTerrain;

	bool bFinalImpact;

	UPROPERTY()
	AActor* ImpactedActor;

	FVector ImpactLocation;
};

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class DUNGEONS_API UImpactAction : public UObject {
	GENERATED_BODY()
public:
	virtual void OnLaunch(ABaseProjectile* projectile) {};
	virtual void OnImpact(ABaseProjectile* projectile, const FImpactInfo& impactInfo) {};
	virtual void Reset() {};
	virtual void PostInit() {}

	UPROPERTY(EditAnywhere)
	EImpactTriggerType TriggerType = EImpactTriggerType::Final;
};

UCLASS(Abstract)
class DUNGEONS_API UGameplayEffectImpactAction : public UImpactAction {
	GENERATED_BODY()
public:
	void OnLaunch(ABaseProjectile* projectile) override;
	void Reset() override;
	void PostInit() override;
	virtual void OnGameplayEffectSpecCreated(FGameplayEffectSpec&, ABaseProjectile*) {};
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectToApply;

	UPROPERTY(EditAnywhere)
	bool UsePowerSource = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "UsePowerSource"))
	EEffectLevelSource PowerSource = EEffectLevelSource::SourceItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!UsePowerSource"))
	float PowerLevel = 1.0f;

	TOptional<FGameplayEffectSpec> Spec;

};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UExplosionAction : public UGameplayEffectImpactAction {
	GENERATED_BODY()
public:
	UExplosionAction();

	void OnImpact(ABaseProjectile*, const FImpactInfo&) override;
	void OnGameplayEffectSpecCreated(FGameplayEffectSpec&, ABaseProjectile*) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRespectProjectileDamageMultiplier = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ExplosionCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPushback Pushback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BlockPiercingAndFuseShot = true;
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UDelayedExplosionAction : public UGameplayEffectImpactAction {
	GENERATED_BODY()
public:
	UDelayedExplosionAction();

	void OnImpact(ABaseProjectile*, const FImpactInfo&) override;
	void OnGameplayEffectSpecCreated(FGameplayEffectSpec&, ABaseProjectile*) override;

	UPROPERTY(EditAnywhere,  BlueprintReadWrite)
	float ExplosionDelaySeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ExplosionCue;

	UPROPERTY(Category = "Dungeons|Projectile", EditDefaultsOnly, BlueprintReadWrite)
	FPushback Pushback;
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UMobSpawnAction : public UImpactAction {
	GENERATED_BODY()
public:
	void OnImpact(ABaseProjectile*, const FImpactInfo&) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MobType = "slime-cauldron";
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UChainLightningAction : public UGameplayEffectImpactAction {
	GENERATED_BODY()
public:
	UChainLightningAction();

	void OnImpact(ABaseProjectile*, const FImpactInfo&) override;
	void OnGameplayEffectSpecCreated(FGameplayEffectSpec&, ABaseProjectile*) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 37.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChainRadius = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FinalImpactExclusionRadius = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delay = 0.1f;
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API USpawnActorAction : public UImpactAction {
	GENERATED_BODY()
public:
	void OnImpact(ABaseProjectile*, const FImpactInfo&) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> ActorClass;

	//Transform generation mode when no delegate is present
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorDefaultTransform DefaultTransformMode = ESpawnActorDefaultTransform::ImpactLocation;

	//Applied to the transform generated by delegate or default transform mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESpawnActorSnappingMode SnappingMode = ESpawnActorSnappingMode::None;
	
	FOnImpactActorSpawned OnActorSpawned;
	FTransformGenerator TransformGenerator;

	//If true, actor will spawn on clients as well as host
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSpawnLocally = false;

	//Adds random chance of spawning if server only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bSpawnLocally", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SpawnChance = 1.f;
private:

	FTransform GetDefaultTransform(ABaseProjectile*, const FImpactInfo&) const;
	FTransform Snap(const FTransform&) const;
};

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UEffectToRandomTargetsAction : public UGameplayEffectImpactAction {
	GENERATED_BODY()
public:
	UEffectToRandomTargetsAction();
	virtual void OnImpact(ABaseProjectile*, const FImpactInfo&);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> SetByCallerTagMagnitudes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Period = .0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SearchRadius = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinTargets = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTargets = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAlwaysApplyToImpactedTarget = true;

	void OnGameplayEffectSpecCreated(FGameplayEffectSpec&, ABaseProjectile*) override;

private:
	void ApplyEffectToRandomTargets(ABaseProjectile* source, ABaseCharacter* instigator, const FImpactInfo& impactInfo) const;
	
};

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class DUNGEONS_API UImpactActionHandler : public UObject {
	GENERATED_BODY()
public:
	UImpactActionHandler();

	void OnImpact(ABaseProjectile*, FImpactInfo);
	void OnLaunch(ABaseProjectile*);
	void Reset();
	void PostInit();


	bool HasActions() const;
	TArray<UImpactAction*> GetActions() const;

	void AddRuntimeAction(UImpactAction*);
	void RemoveRuntimeAction(UImpactAction*);

	TArray<UImpactAction*> GetRuntimeActions(TSubclassOf<UImpactAction> cls) const;
	bool HasAction(TSubclassOf<UImpactAction> cls) const;

	TArray<const UImpactAction*> GetActions(TSubclassOf<UImpactAction> cls) const;

	template <typename T> TArray<const T*> GetActions() const {
		return algo::map_as<TArray<const T*>>(GetActions(T::StaticClass()), RETLAMBDA(Cast<const T>(it)));
	}

	template <typename T> bool HasAction() const {
		return HasAction(T::StaticClass());
	}

	template <typename T> TArray<T*> GetRuntimeActions() const {
		return algo::map_cast(GetRuntimeActions(T::StaticClass()));
	}

protected:
	UPROPERTY(Instanced, EditDefaultsOnly)
	TArray<UImpactAction*> DefaultImpactActions;

	UPROPERTY(Transient)
	TArray<UImpactAction*> RuntimeImpactActions;

	TArray<UImpactAction*> QueuedActions;
private:
	TOptional<FImpactInfo> ImpactInfo;

	bool bHasLaunched = false;
};
