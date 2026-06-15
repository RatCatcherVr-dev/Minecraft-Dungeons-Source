#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayPrediction.h"
#include "GameplayEffectTypes.h"
#include "game/util/DamageFilter.h"
#include "game/util/ImpactFilter.h"
#include <GameplayEffect.h>
#include "HealthComponent.generated.h"

class UAbilitySystemComponent;

//////////////////////////////////////////////////////////////////////////

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChanged, const FOnAttributeChangeData&);
DECLARE_MULTICAST_DELEGATE(FOnRevive);
DECLARE_MULTICAST_DELEGATE(FOnBeforeDeath);
DECLARE_MULTICAST_DELEGATE(FOnDeath);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShieldChange, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeal, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDamageReceived, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDamageTypeReceived, float, const FGameplayTagContainer&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDamageReceivedWithType, float, const FGameplayAttribute&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthFractionChanged, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthFraction, float, currentPercent, float, lastPercent);

//////////////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class EDamageComponent : uint8 {
	Default,
	Melee,
	Projectile,
	Explosion,
	Fall,
	Potion,
	AOE,
	Item,
	Harvester,
	Magic,
	KillZone
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FBarOnScreenInformation {
	GENERATED_USTRUCT_BODY()

	FBarOnScreenInformation()
		: OnScreenIcon(nullptr)
		, MaxDistanceToAppear(3000)
	{
	}

	/** Icon to show when this mob bar is showing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	UTexture2D* OnScreenIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayAbilityTriggerPayload)
	float MaxDistanceToAppear = 3000;
};

//////////////////////////////////////////////////////////////////////////

UCLASS()
class DUNGEONS_API UHealSelfGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UHealSelfGameplayEffect();

	static FGameplayModifierInfo GetHealingInfo(EGameplayModOp::Type modifierOperation);
};

UCLASS()
class DUNGEONS_API UHealthRegenerationGameplayEffect : public UHealSelfGameplayEffect {
	GENERATED_BODY()
public:
	UHealthRegenerationGameplayEffect();
	const float tickRate = 0.5f;
};

UCLASS()
class DUNGEONS_API UReviveGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UReviveGameplayEffect();
};

UCLASS()
class DUNGEONS_API UKillGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UKillGameplayEffect();
};

UCLASS()
class DUNGEONS_API URespawnGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URespawnGameplayEffect();
};

UCLASS()
class DUNGEONS_API UFreezeRespawnGameplayEffect : public URespawnGameplayEffect {
	GENERATED_BODY()
public:
	UFreezeRespawnGameplayEffect();
};

UCLASS()
class DUNGEONS_API USetHealthThresholdGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USetHealthThresholdGameplayEffect();
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), DisplayName="HealthComponent")
class DUNGEONS_API UHealthComponent : public UActorComponent {
	GENERATED_BODY()

public:

	// I hate that these exists, would be so much better if we could initialize the attributes instead.

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite)
	bool Shield = false;

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float Health = 500;

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxHealth = 500;

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite)
	bool ResistDeath = false; // D11.DB

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite)
	bool Invincible = false;

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite)
	FBarOnScreenInformation BarOnScreenInformation;
	
	UHealthComponent();

	void BeginPlay() override;

	void ApplyHeal(float heal);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerApplyHeal(float heal);

	void Revive(float healthPercentage = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Kill(); // Function should be removed entirely, or replaced with a helper?

	/**
	 * Initiates kill using gameplay effect. Use for kills that require context. Less performant than Kill().
	 */
	void KillWith(FGameplayTag damageType);

	bool IsNotAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")	// Called once in blueprints, used to avoid an issue with cloned ragdoll
	bool IsAlive() const;

	bool IsHealthMaxed() const;

	bool IsDeathPrevented() const;

	UFUNCTION()
	void OnResurrection() { NumberOfTimesResurrected++; };

	int GetNumberOfTimesRessurected() const { return NumberOfTimesResurrected; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ShouldImpactProjectile(const FGameplayTag& damageType) const;
	
	float GetCurrentHealthPercentage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetHealthAsDisplayAmount(float damage);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetMaximumHealth() const;

	float GetMaximumHealthBase() const;

	float GetConstantDesignedMaximumHealth() const;

	UFUNCTION()
	float GetDamageMultiplier(const FGameplayTagContainer& damageTypes) const;

	void SetHealthThresholds(TArray<float> newHealthThresholds);

	// Returns tags applied by damage that caused death
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FGameplayTagContainer GetDeathDamageTags() const;

	FOnChanged OnChanged;
	FOnHeal OnHeal;
	FOnDeath OnDeath;
	FOnBeforeDeath OnBeforeDeath;
	FOnRevive OnRevive;
	FOnDamageReceived OnDamageReceived;
	FOnDamageTypeReceived OnDamageTypeReceived;
	FOnDamageReceivedWithType OnDamageReceivedWithType;
	FOnShieldChange OnShieldChange;
	FOnHealthFractionChanged OnHealthFractionChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnHealthFraction OnHealthFraction;

private:
	void OnAttributeMaxHealthChange(const FOnAttributeChangeData& data);
	void OnAttributeHealthChange(const FOnAttributeChangeData& data);
	void OnServerAttributeHealthChange(const FOnAttributeChangeData& data) const;
	void OnAttributeShieldChange(const FOnAttributeChangeData& data) const;
	void BroadcastDamageAmountAndType(float amount, const FOnAttributeChangeData& type) const;

	float GetHealthIncrease(float heal) const;

	UAbilitySystemComponent* GetAbilitySystem();
	const UAbilitySystemComponent* GetAbilitySystem() const;

	bool IsMissionCompleted() const;

	UPROPERTY()
	int NumberOfTimesResurrected = 0;

	/** Used to scale damage from any attack and pushback from melee and aoe attacks only */
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	FDamageFilter DamageFilter; // Where is this populated?

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	FImpactFilter ImpactFilter;

	TOptional<float> PreviouslySeenHealth;
	TOptional<float> CachedHealthFraction;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	TArray<float> HealthResistThresholds;

	int CurrentHealthThreshold = 0;
	FGameplayTagContainer DeathDamageTags;
};

namespace damageTag {
	FGameplayTag def();
	FGameplayTag aoe();
	FGameplayTag explosion();
	FGameplayTag strongExplosion();
	FGameplayTag damageFriends();
	FGameplayTag harvester();
	FGameplayTag killzone();
	FGameplayTag lava();
	FGameplayTag trap();
	FGameplayTag magic();
	FGameplayTag wind();
	FGameplayTag magicThorns();
	FGameplayTag poison();
	FGameplayTag melee();
	FGameplayTag ranged();
	FGameplayTag weak();
	FGameplayTag fire();
	FGameplayTag toxicWater();
	FGameplayTag drowning();
	FGameplayTag environmental();
}