#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/animinstance/PerfectFormAnimInstance.h"
#include "PerfectFormComponent.generated.h"


class UPerfectFormAnimInstance;

enum class CombatPhase {
	PhaseInit = 0,
	PhaseOne = 1,
	PhaseTwo = 2,
	PhaseFinal = 3,
};
enum class TransitionPhase {
	Begin = 0,
	Active = 1,
	Ending = 2,
	NONE = 3,
};
enum class LaserPhase {
	Begin = 0,
	Active = 1,
	Ending = 2,
	NONE = 3,
};
enum class AttackType {
	Charge = 0,
	BombSpew = 1,
	EndermiteScatter = 2,
	EndermiteFeast = 3,
	LaserPhaser = 4,
	VoidPull = 5,
	NONE = 6,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetMirrorsActiveDelegate, bool, isActive, float, activationProbability);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetVoidPoolActiveDelegate, bool, isActive);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndermiteFeastDelegate, bool);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UPerfectFormComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerfectFormComponent();

	UPROPERTY(BlueprintAssignable)
	FOnSetVoidPoolActiveDelegate OnVoidPoolActiveDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnSetMirrorsActiveDelegate OnSetMirrorsActiveDelegate;

	FOnEndermiteFeastDelegate OnEndermiteFeastDelegate;

	UPROPERTY(ReplicatedUsing = OnRep_AnimLocomotionState)
	EPerfectFormAnimLocomotion PerfectFormLocomotionBlend = EPerfectFormAnimLocomotion::Init;

	UPROPERTY(ReplicatedUsing = OnRep_LaserBodyRotationRate)
	float laserBodyRotationRate = 0.f;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetAnimLocomotionState(EPerfectFormAnimLocomotion animLocomotion);

	void SetLaserBodyRotationRate(float inRate);

	void SetEndermiteFeast(bool isActive);

	void SetVoidPoolActive(bool isActive);

	void SetMirrorsActive(bool isActive, float activationChance);

	bool GetDebugState() const;

	UFUNCTION()
	void OnRep_AnimLocomotionState();

	UFUNCTION()
	void OnRep_LaserBodyRotationRate();

private:
	void TickOnScreenDebugging(float DeltaTime);

	UPerfectFormAnimInstance* GetAnimInstance();

public:	// Debugging
	void PrintOnce(const FString&, float);

	void SetCombatPhaseDebug(CombatPhase inPhase) { cPhase = inPhase; }

	void SetAttackTypeDebug(AttackType inType) { aType = inType; }

	void SetTransitionPhaseDebug(TransitionPhase inPhase) { tPhase = inPhase; }

	void SetLaserPhaseDebug(LaserPhase inPhase) { lPhase = inPhase; }

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebuggingEnabled = false;

	class UHealthComponent* HealthComponent = nullptr;

	UPerfectFormAnimInstance* Animinstance = nullptr;

	CombatPhase cPhase{ CombatPhase::PhaseInit };

	AttackType aType{ AttackType::NONE };

	TransitionPhase tPhase{ TransitionPhase::NONE };

	LaserPhase lPhase{ LaserPhase::NONE };
};
