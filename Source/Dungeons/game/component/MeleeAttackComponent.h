#pragma once

#include "DrawDebugHelpers.h"
#include "game/component/AttackComponent.h"
#include "game/util/Pushback.h"
#include <GameplayEffect.h>
#include "../ImpactSoundCueProvider.h"
#include "game/item/stats/ItemStatsTypes.h"
#include "Animation/AnimMontage.h"
#include "world/entity/EntityTypes.h"
#include "MeleeAttackComponent.generated.h"

class UItemSlot;
class APlayerCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerMeleeAttack, const APlayerCharacter*);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnMeleeDamageDealt, bool, const ABaseCharacter*, const ABaseCharacter*, const FSharedPredictionContext&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAfterMeleeDamageDealt, bool, const ABaseCharacter*, const FSharedPredictionContext&);
DECLARE_MULTICAST_DELEGATE_FiveParams(FOnAttackVariantEnd, AActor*, FVector, int32, FRandomStream*, const FSharedPredictionContext&);

struct MeleeAttackParametersCapture {
	float AttackSpeedMultiplier = 1.f;
	float AngleMultiplier = 1.f;
	float RangeMultiplier = 1.f;
	float PushbackMultiplier = 1.f;

	MeleeAttackParametersCapture(const UAbilitySystemComponent* component);
	MeleeAttackParametersCapture() {};
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEnchantmentTriggerSettings {
	GENERATED_USTRUCT_BODY()
	// Whether OnAfterMeleeAttack enchantment callback should trigger when variant changes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	bool bTriggerEnchantmentsOnVariantChange = false;

	// Whether OnAfterMeleeAttack enchantment callback should trigger when after damage is dealt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	bool bTriggerEnchantmentsAfterDamage = true;

	// Whether OnAfterMeleeAttack enchantment callback should trigger when releasing attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	bool bTriggerEnchantmentsOnRelease = false;
};

UENUM(BlueprintType)
enum class EMeleeAttackVariantType : uint8
{
	TargetAndSplash,
	DistanceBased,
};


USTRUCT(BlueprintType)
struct DUNGEONS_API FMeleeAttackComponentAttackVariant {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|AttackVariant")
	class UAnimSequenceBase* AttackSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|AttackVariant")
	FName Slot = FName(TEXT("UpperBody"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	FPushback pushback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	EMeleeAttackVariantType AttackVariantType = EMeleeAttackVariantType::TargetAndSplash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float AttackAnimationTimeDurationSeconds = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float ConeAngleDegrees = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float ConeOffsetUnits = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float MinRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float AttackActivationRangeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float MaxZDiff = 210.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float DamageDelaySeconds = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float Damage = 35.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AttackVariant")
	FGameplayTag DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float DamageSplashMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float EffectsPushbackMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float StunMultiplier = 1.f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float CooldownSeconds = 1.0f;

	//Multiplier for the configured reset time. Defaults to 1.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float AttackVariantResetMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	USoundCue* HitSoundCue = nullptr;

	UPROPERTY(Transient)
	UAnimMontage* DynamicMontageCache;

	// Set to -1 for infinite looping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	int VariantLoopAmount = 1;

	// Effect to play during variant
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	TSubclassOf<UGameplayEffect> VariantActiveEffect;

	// Effect to apply when loop finishes and is not cancelled, on variants with no looping this will always trigger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	TSubclassOf<UGameplayEffect> VariantChangeEffect;

	// Interval for continous damage execution, set to 0 for no continuous damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	float ContinuousDamageInterval = 0.f;

	// Settings for where OnAfterMeleeAttack enchantment callback should be called
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	FEnchantmentTriggerSettings EnchantmentTriggerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariant")
	bool bOverrideCancelPoint = false;

	// Overrides the passive cancel point value, 0.0-1.0 where 1.0 means unable to cancel during whole attack variant
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bOverrideCancelPoint"), Category = "Dungeons|AttackVariant")
	float CancelPointOverride = 0.f;

	bool shouldLoopAgain(int32 loopCount) const {
		return (VariantLoopAmount < 0) || (loopCount + 1 < VariantLoopAmount);
	}
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UMeleeAttackComponent : public UAttackComponent, public IImpactSoundProvider {
	GENERATED_BODY()

public:
	UMeleeAttackComponent();

	void BeginPlay() override;

	int32 GetAttackVariantIndex() const;

	const TArray<FMeleeAttackComponentAttackVariant>& GetActiveAttackVariants() const;
	const TArray<FMeleeAttackComponentAttackVariant>& GetConfiguredAttackVariants() const;
	FOnPlayerMeleeAttack OnPlayerMeleeAttack;

	FOnAfterMeleeDamageDealt OnAfterMeleeDamageDealt;
	FOnMeleeDamageDealt OnMeleeDamageDealt;
	FOnAttackVariantEnd OnVariantEnd;

	const FMeleeAttackComponentAttackVariant& GetCurrentAttackVariant() const;	

	bool CanAttack(AActor* attackTarget = nullptr) const override;	

	bool IsAttackInProgress() const override;

	float GetAttackRange() const override;

	float GetAttackActivationRange() const;
	
	bool IsWithinAttackRange(AActor* attackTarget) const;

	bool IsWithinFuzzyArc(FVector target) const;

	bool InAttackOrientation(AActor* attackTarget) const override;

	bool IsTargetInAttackWedge(AActor* attackTarget);

	float GetMinAttackRange() const;

	void StopAttack() override;

	UFUNCTION()
	void CancelActions();

	FPredictionKey AttackLocal(AActor* attackTarget = nullptr) override;

	void AttackWithVariantIndex(AActor* attackTarget, FVector attackVector, int32 variantIndex, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext());

	// D11.DB
	void SetRangeBoost(float boost = 0.0f);
	void ResetRangeBoost() {
		SetRangeBoost();
	}

	// D11.DB - Made this public. Required for certain AI behaviours.
	void SetAttackVariantIndex(int32 index);

	// D11.SSN
	bool IsMeleeDisabled() const;
	
	bool bRemoveInvisibilityAfterAttack = true;
	void SetHoldingAttack(bool holding);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetHoldingAttack(bool holding);
protected:
	void PredictionKeyRejectedOrCaughtUpCallback(FPredictionKey::KeyType key);

	void ResetAttackVariantTimer();

	UFUNCTION(Server, Reliable, WithValidation)
	void AttackWithVariantIndexServer(AActor* attackTarget, FVector attackVector, int32 variantIndex, int32 seed, FPredictionKey key);

	void ApplyDamage(MeleeAttackParametersCapture capture, FMeleeAttackComponentAttackVariant variant, int index, TWeakObjectPtr<AActor> attackTarget, FVector attackVector, int32 seed, FSharedPredictionContext predictionContext);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float AttackVariantResetSeconds = 3.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float AttackAnimationResetSeconds = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float TargetAttackRangeMargin = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float SameTargetAttackRangeBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float SameTargetAttackRangeThresholdSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	float SharedCooldownConstant = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|MeleeAttackComponent")
	bool bAllowSameTeamSplashDamage = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|AttackVariants")
	TArray<FMeleeAttackComponentAttackVariant> ConfiguredAttackVariants;

	UFUNCTION()
	void OnWeaponSlotUpdated(UItemSlot* slot);

	USoundCue* GetImpactSound_Implementation(int index) const override;

private:

	void SetAttackVariants(TArray<FMeleeAttackComponentAttackVariant>& variants);

	UPROPERTY()
	TArray<FMeleeAttackComponentAttackVariant> ActiveAttackVariants;

	UPROPERTY()
	float ActiveAttackVariantResetSeconds;

	int32 currentIndex = 0;

	int32 loopCount = 0;

	FTimerHandle delayTimerHandle;
	FTimerHandle attackVariantResetTimerHandle;
	FTimerHandle attackVariantResetAnimationTimerHandle;
	FTimerHandle rangeBoostTimerHandle; // D11.DB
	FTimerHandle continousDamageTimerHandle;
	FTimerHandle attackVariantEndTimerHandle;
	FTimerHandle attackVariantCooldownTimerHandle;
	TArray<ABaseCharacter*> targetDamageCharacters;
	TWeakObjectPtr<AActor> weapon;
	
	TWeakObjectPtr<AActor> lastPrimaryTargetDamaged;
	float lastPrimaryTargetDamagedStamp;

	float mRangeBoost; // D11.DB
	float mRangeBoostTimer; // D11.DB

	bool IsSameTargetWithTimeThreshold(AActor* target, float timeThreshold) const;

	bool CanTarget(const ABaseCharacter* target) const;	
	bool IsAttackAllowed(const ABaseCharacter* attacker, const ABaseCharacter* target) const;

	void FindActorsInAttackWedge(MeleeAttackParametersCapture capture, FMeleeAttackComponentAttackVariant variant, TWeakObjectPtr<AActor> attackTarget, TArray<ABaseCharacter*>& outList) const;

	static AActor* FindPrimaryDamageActor(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, TArray<TArray<ABaseCharacter*>::ElementType> filteredTargets);

	static float GetSplashAdjustedDamage(float attackDamage, float damageSplashMultiplier, AActor* primaryDamageTarget, ABaseCharacter* currentTarget);
	static float GetDistanceBasedDamage(float attackDamage, float attackRange, FVector attackLocation, ABaseCharacter* currentTarget);
	AActor* FindDamageSource(ABaseCharacter* attacker) const;
	static float GetStunMultiplier(float stunMultiplier, const AActor* primaryDamageTarget, ABaseCharacter* currentTarget);
	static float Get2DDistanceToTarget(AActor* attacker, AActor* attackTarget);

	UFUNCTION()
	void OnAttackVariantEnd(AActor* attackTarget, FVector attackVector, int32 index, int32 seed, FSharedPredictionContext predictionContext, bool wouldLoopNext);
	UFUNCTION()
	void OnVariantCooldownDone(int32 index);
	UFUNCTION()
	void OnDeath();
	void RemoveVariantEffects();

	UFUNCTION()
	void OnMeleeAttack(const TArray<EntityType>& killedMobs);
	UFUNCTION(Client, Reliable)
	void Client_OnMeleeAttack(const TArray<EntityType>& killedMobs);
public:
	void PlayAttackVariantMontage(int32 variantIndex, float playRate);
	float GetAttackVariantsStat(const TArray<FMeleeAttackComponentAttackVariant>& attacks, EItemStats stat);
	static float GetAttackVariantsStat(const TArray<FMeleeAttackComponentAttackVariant>& attacks, EItemStats stat, float VariantResetSeconds);
	static float GetAttackVariantTotalTimeContributionFactor(const TArray<FMeleeAttackComponentAttackVariant>& attacks, const FMeleeAttackComponentAttackVariant& askedVar);
	static float GetAttackVariantIndexRelevanceWeight(const FMeleeAttackComponentAttackVariant& askedVar, int numAttacks, int attackVariantIndex, float VariantResetSeconds);
	static float GetAttackVariantIndexRelevanceWeightFactor(const TArray<FMeleeAttackComponentAttackVariant>& attacks, int attackVariantIndex, float VariantResetSeconds);
private:
	int32 lastAttackIndex = -1;	
	bool bHoldingAttack = false;
	FActiveGameplayEffectHandle VariantActiveEffectHandle;
	FActiveGameplayEffectHandle VariantChangeEffectHandle;
};

UCLASS()
class DUNGEONS_API UMeleeDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeDamageGameplayEffect();
};