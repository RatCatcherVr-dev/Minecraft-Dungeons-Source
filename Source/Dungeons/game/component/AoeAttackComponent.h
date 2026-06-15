#pragma once

#include "game/component/AttackComponent.h"
#include "game/util/Pushback.h"
#include "GameplayTagContainer.h"
#include "HealthComponent.h"
#include "game/actor/DungeonsTargetMarker.h"
#include "AoeAttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAoeSignature);

UCLASS(Category = "Dungeons", BlueprintType, EditInlineNew)
class DUNGEONS_API UAoeTargetProvider : public UObject {
	GENERATED_BODY()
public:
	//Returns origin of attack and fills outTargets with targets.
	UFUNCTION(BlueprintNativeEvent)
	FVector RequestTargetForComponent(const class UAoeAttackComponent* component, TArray<AActor*>& outTargets) const;

	UFUNCTION(BlueprintNativeEvent)
	float GetAttackRange() const;

	UPROPERTY(EditDefaultsOnly, Category = "TargetProvider")
	TSubclassOf<ADungeonsTargetMarker> Marker;

	UPROPERTY(EditDefaultsOnly, Category = "TargetProvider", meta = (EditCondition = "Marker != nullptr"))
	float minMarkerTimeToLock;
	UPROPERTY(EditDefaultsOnly, Category = "TargetProvider", meta = (EditCondition = "Marker != nullptr"))
	float maxMarkerTimeToLock;
};

/*
 * Gathers targets in a sphere. Offset is in the forward direction of the actor. */
UCLASS(BlueprintType)
class DUNGEONS_API USphereInfrontAoeTargetProvider : public UAoeTargetProvider {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float attackRadius = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float attackOffset = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float attackOffsetY = 0.0f;

	FVector RequestTargetForComponent_Implementation(const UAoeAttackComponent* component, TArray<AActor*>& outTargets) const override;
	float GetAttackRange_Implementation() const override;
};

/*
 * Gathers targets in a cube. Offset is in the forward direction of the actor. */
UCLASS(BlueprintType)
class DUNGEONS_API UCubeInfrontAoeTargetProvider : public UAoeTargetProvider {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float attackWidth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float attackLength = 500.0f;

	FVector RequestTargetForComponent_Implementation(const UAoeAttackComponent* component, TArray<AActor*>& outTargets) const override;
	float GetAttackRange_Implementation() const override;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAoeAttackComponent : public UAttackComponent
{
	GENERATED_BODY()
	
public:
	UAoeAttackComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoeAttackComponent")
	class UAnimSequenceBase* AoeSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoeAttackComponent")
	FName Slot = FName(TEXT("FullBody"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AoeAttackComponent")
	FName AttackSocketName = FName(TEXT(""));

	UPROPERTY(BlueprintAssignable, Category = "AoeAttackComponent")
	FOnAoeSignature OnAoe;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoeAttackComponent")
	float attackDamage = 30.0f;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoeAttackComponent")
	float damageDelay = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoeAttackComponent")
	float totalTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoeAttackComponent")
	bool ignoreFriendlyFire = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AoeAttackComponent")
	FPushback pushback;

	void ApplyDamage(FSharedPredictionContext predictionContext);
	
	UPROPERTY(Instanced, EditDefaultsOnly, Category = "AoeAttackComponent")
	UAoeTargetProvider* TargetProvider;

	//Effect applied to targets of this attack.
	UPROPERTY(EditDefaultsOnly, Category = "AoeAttackComponent")
	TSubclassOf<UAoeAttackDamageGameplayEffect> DamageEffect;

	//Effect applied to to attacker when staring attack. Removed when attack stops or is finished.
	UPROPERTY(EditDefaultsOnly, Category = "AoeAttackComponent")
	TSubclassOf<UGameplayEffect> AttackingEffect;

	void CompleteAttack();

	void StopAttack() override;

	float GetAttackRange() const override { return TargetProvider->GetAttackRange(); }

	bool IsAttacking() const override { return completeTimerHandle.IsValid(); };

	FVector GetOwnerAttackSocketLocation() const;
	FVector GetOwnerAttackSocketRotation() const;

	virtual FVector GetAttackLocation() const { return AttackSocketName.IsNone() ? GetOwner()->GetActorLocation() : GetOwnerAttackSocketLocation(); };
	virtual FVector GetAttackRotation() const { return AttackSocketName.IsNone() ? GetOwner()->GetActorForwardVector() : GetOwnerAttackSocketRotation(); }

	bool IsAttackInProgress() const override;

protected:
	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;

	UPROPERTY(EditDefaultsOnly, Category = "AoeAttackComponent")
	FGameplayTag EffectCue;

	FTimerHandle delayTimerHandle;

private:
	FActiveGameplayEffectHandle AttackingEffectHandle;
	
	FTimerHandle completeTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "AoeAttackComponent")
	FGameplayTag DamageType;
};

UCLASS()
class DUNGEONS_API UAoeAttackDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAoeAttackDamageGameplayEffect();
};
