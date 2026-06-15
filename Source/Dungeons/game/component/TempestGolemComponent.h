#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "AoeAttackComponent.h"
#include "TempestGolemComponent.generated.h"

UENUM(BlueprintType)
enum class TempestArmType : uint8
{
	RIGHT = 0,
	LEFT = 1,
	SHIELD = 2,
	INVALID = 3
};

UENUM(BlueprintType)
enum class TempestArmStatus : uint8
{
	ACTIVE = 0,
	ATTACKING = 1,
	DISABLED = 2,
	DISABLING = 3,
	REACTIVATING = 4,
	INVALID = 5,
	TRACKING = 6,
	LOCKED = 7,
};

UENUM(BlueprintType)
enum class TempestStatus : uint8
{
	INVALID = 0,
	INTRO = 1,
	ACTIVE = 2,
	STUNNED = 3
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightArmStatusChanged, TempestArmStatus, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeftArmStatusChanged, TempestArmStatus, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldArmsStatusChanged, TempestArmStatus, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTempestStatusChanged, TempestStatus, newStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTempestGolemDamageReceived, float, amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTempestGolemUnstunPercentageHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTempestGolemDeath);

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UTempestGolemComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UTempestGolemComponent();	

	void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnRightArmStatusChanged OnRightArmStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnLeftArmStatusChanged OnLeftArmStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnShieldArmsStatusChanged OnShieldArmsStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnTempestStatusChanged OnTempestStatusChanged;
	UPROPERTY(BlueprintAssignable)
	FOnTempestGolemDamageReceived OnTempestGolemDamageReceived;
	UPROPERTY(BlueprintAssignable)
	FOnTempestGolemUnstunPercentageHit OnTempestGolemUnstunPercentageHit;
	UPROPERTY(BlueprintAssignable)
	FOnTempestGolemDeath OnTempestGolemDeath;

	UFUNCTION(BlueprintCallable)
	bool CanArmAttack(TempestArmType armType);
	UFUNCTION(BlueprintCallable)
	bool IsArmTracking(TempestArmType armType);
	UFUNCTION(BlueprintCallable)
	void ArmAttack(TempestArmType armType);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TempestArmStatus GetArmStatus(TempestArmType armType);
	UFUNCTION(BlueprintCallable)
	void SetArmStatus(TempestArmType armType, TempestArmStatus newStatus);

	//Tempest
	UFUNCTION(BlueprintCallable)
	void CheckTempestStun();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TempestStatus GetTempestStatus();
	UFUNCTION(BlueprintPure)
	bool IsTempestActive();
	UFUNCTION(BlueprintCallable)
	void SetTempestStatus(TempestStatus newStatus);

	void StartArmTracking(TempestArmType armtype);
	UFUNCTION()
	void OnTrackingDone(TempestArmType armType);

	//Other
	UPROPERTY(BlueprintReadOnly)
	float leftArmAttackWeight = 0.5f;
	UPROPERTY(BlueprintReadOnly)
	float rightArmAttackWeight = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	float ArmsTrackingTime = 0.5f;
	UPROPERTY(EditDefaultsOnly)
	float ArmsLockedTime = 0.5f;
	UPROPERTY(EditDefaultsOnly)
	float ShieldTargetInRangeTime = 0.5f;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetLeftArmWeight();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRightArmWeight();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetAttackSpeed();
	UFUNCTION(BlueprintCallable)
	void SetAttackSpeed(float newSpeed);

	UFUNCTION(BlueprintCallable)
	void ApplyDamageReduction(float damageMultiplier = 0.2f);
	UFUNCTION(BlueprintCallable)
	void RemoveDamageReduction();

	UFUNCTION(BlueprintCallable)
	void DealDamage(TempestArmType armType);

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> leftTargets;
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> rightTargets;
	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> shieldTargets;

	UPROPERTY(EditDefaultsOnly)
	float RemoveStunAfterDamagePercentage = 0.1f;

	UPROPERTY(BlueprintReadOnly)
	FVector lookAtLocation;

	FString GetArmStatusText(TempestArmStatus ArmStatus);

	UFUNCTION()
	void HandleDamageReceived(float amount);
	bool HasReceivedEnoughDamageToUnstun();

	void OnDeath() const;

	UFUNCTION()
	void OnRep_TempestStatus();
	UFUNCTION()
	void OnRep_TempestRightArmStatus();
	UFUNCTION()
	void OnRep_TempestLeftArmStatus();
	UFUNCTION()
	void OnRep_TempestShieldArmsStatus();


	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_TempestRightArmStatus)
	TempestArmStatus rightArmStatus = TempestArmStatus::ACTIVE;
	UPROPERTY(ReplicatedUsing = OnRep_TempestLeftArmStatus)
	TempestArmStatus leftArmStatus = TempestArmStatus::ACTIVE;
	UPROPERTY(ReplicatedUsing = OnRep_TempestShieldArmsStatus)
	TempestArmStatus shieldArmsStatus = TempestArmStatus::ACTIVE;
	
	UPROPERTY(ReplicatedUsing = OnRep_TempestStatus)
	TempestStatus tempestStatus = TempestStatus::INVALID;

	float swordArmsAttackSpeed = 1.0f;

	float percDamageReceivedWhileStunned = 0;

	UAbilitySystemComponent* abilitySystemComp;
	FActiveGameplayEffectHandle damageReductionHandle;	

	class UAoeAttackComponent* LeftArmAttackComponent;
	class UAoeAttackComponent* RightArmAttackComponent;
	class UAoeAttackComponent* ShieldAttackComponent;
	UHealthComponent* TempestGolemHealthComponent;

};