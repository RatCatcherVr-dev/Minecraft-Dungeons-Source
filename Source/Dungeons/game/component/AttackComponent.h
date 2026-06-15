#pragma once

#include "Components/ActorComponent.h"
#include "GameplayPrediction.h"
#include "game/abilities/prediction/GameplayPredictionExtensions.h"
#include "AttackComponent.generated.h"

class ABaseCharacter;

USTRUCT()
struct FAttackComponentAimData {
	GENERATED_BODY()
	AActor* target = nullptr;	
};

DECLARE_DELEGATE_RetVal(FAttackComponentAimData, FAttackAimProvider)


UENUM(BlueprintType)
enum class ESuccessFailure : uint8
{
	Success,
	Failure
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta = (ShortTooltip = "The base for Attack Components."))
class DUNGEONS_API UAttackComponent : public UActorComponent {
	GENERATED_BODY()
public:
	UAttackComponent();

	UFUNCTION(Category = "Dungeons|AttackComponent")
	virtual FPredictionKey AttackLocal(AActor* attackTarget = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|AttackComponent")
	virtual bool CanAttack(AActor* attackTarget = nullptr) const;	

	virtual float GetAttackRange() const;
	float GetAttackRangeSquared() const;

	virtual bool IsAttackInProgress() const;

	virtual bool InAttackOrientation(AActor* attackTarget) const;

	float LastAttackTimestampSeconds() const; 
	float CanAttackFromTimestampSeconds() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|AttackComponent")
	float GetCoolDownAsScalarValue() const;

	void PredictionKeyRejectedOrCaughtUpCallback(FPredictionKey::KeyType key);

	virtual bool BeginAttack(const FAttackAimProvider& targetProvider);
	virtual void EndAttack(AActor* attackTarget = nullptr);
	virtual bool IsAttacking() const;

	virtual void Stop() final;

	virtual void StopAttack() {};

	void OnDeath();

	void CancelCooldown();

	int AttackCounter = 0;
	int SuccessfulAttackCounter = 0;
protected:
	UFUNCTION(Server, Reliable, WithValidation, Category = "Dungeons|AttackComponent")
	void AttackServer(AActor* attackTarget, int32 seed = 0, FPredictionKey key = FPredictionKey());

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AttackComponent")
	float attackRateSeconds = 1.0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AttackComponent")
	float activeCancelPoint = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AttackComponent")
	float passiveCancelPoint = 0.8f;
	
	float canAttackFromSecond = 0;
	float lastAttackTimestampSeconds = -1e10;

	float GetSecondsUntilNextPossibleAttack() const;

	FPredictionKey outstandingPrediction;

	// todo: Temporary while supporting blueprints (Attack^^ can't be made virtual)
	virtual void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext());


	bool attackPending = false;

	FTimerHandle pendingAttackHandle;

	class ABaseCharacter* cachedOwner = nullptr;

	void BeginPlay() override;

	FVector GetAttackVector(AActor* target) const;
private:
	void AttackWrapper(FAttackAimProvider targetProvider);
};
