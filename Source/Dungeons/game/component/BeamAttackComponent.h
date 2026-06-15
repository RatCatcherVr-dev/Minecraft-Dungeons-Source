#pragma once

#include "CoreMinimal.h"
#include "game/component/AttackComponent.h"
#include "GameplayEffect.h"
#include "game/util/Pushback.h"
#include "BeamAttackComponent.generated.h"

class UMobCharacterMovementComponent;

UCLASS()
class DUNGEONS_API UBeamDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBeamDamageGameplayEffect();
};

UENUM(Blueprintable)
enum class EBeamAttackPhase : uint8 {
	Stopped,
	Targeting,
	Damaging,
};

UENUM(Blueprintable)
enum class EPitchType : uint8 {
	None,
	Beam,
	Actor,
};

UENUM(Blueprintable)
enum class EPitchPhase : uint8 {
	None,
	Target,
	Damage,
	Both,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackPhaseChanged, EBeamAttackPhase, newPhase);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UBeamAttackComponent : public UAttackComponent
{
	GENERATED_BODY()

public:
	UBeamAttackComponent();

	void BeginPlay();

	UFUNCTION()
	void BeginTargetingBeam();

	UFUNCTION()
	void BeginDamagingBeam();

	UFUNCTION()
	void EndBeam();

	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTargetActor();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetAttackRange() const override;

	bool IsAttackInProgress() const override;

	void ChangeBeamMode(bool continuous);

	float GetFullBeamTime();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	FName originSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	TMap<EBeamAttackPhase, UAnimSequenceBase*> animSequences;

	// use a single beam effect for all phases, false to use separate ones
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	bool unifiedBeamEffect = true;

	// remove effect when the attack ends or keep effect applied
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent", meta = (Editcondition = "unifiedBeamEffect"))
	bool removeEffectOnAttackEnd = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent", meta = (Editcondition = "unifiedBeamEffect"))
	TSubclassOf<UGameplayEffect> beamEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent", meta = (Editcondition = "!unifiedBeamEffect"))
	TMap<EBeamAttackPhase, TSubclassOf<UGameplayEffect>> beamEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	TSubclassOf<UGameplayEffect> damageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	FName slot = FName(TEXT("FullBody"));

	// For continuous beam, damage per second
	// for non-continuous beam, damage done on beam end
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	float damage = 100.0f;

	//Time before start targeting
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	float delayTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	TMap<EBeamAttackPhase, float> PhaseBeamTime = { {EBeamAttackPhase::Targeting, 2}, {EBeamAttackPhase::Damaging, 2} };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	float beamRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	float beamRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	TMap<EBeamAttackPhase, float> PhaseRotationsSpeed = { {EBeamAttackPhase::Targeting, 100}, {EBeamAttackPhase::Damaging, 50} };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	bool friendlyFire = false;

	// False deals damage once at end of beam, true deals damage continuously while beam is in damage mode
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	bool continuousDamage = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	bool piercing = false;

	// Whether to rotate the beam, actor, or neither on the Y-axis to aim at the target.
	// If beam, Notify VFX will have to rotate as well.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	EPitchType beamPitchType = EPitchType::None;

	// Which phases to apply the pitch rotation during.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent", meta = (Editcondition = "beamPitchType != EPitchType::None"))
	EPitchPhase beamPitchPhase = EPitchPhase::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeamAttackComponent")
	FPushback onKillPushback;

	virtual void StopAttack() override;
	void OnStunned(const FGameplayTag tag, const int32 tagCount);

	void SetNewPhase(EBeamAttackPhase newPhase);

	UPROPERTY(BlueprintAssignable)
	FOnAttackPhaseChanged OnAttackPhaseChanged;

	UFUNCTION()
	void OnRep_PhaseChange();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;
	TArray<AActor*> GetTargetsInBeam();
	void ApplyDamage(const ABaseCharacter* target, FGameplayEffectSpec damageSpec);
	void ApplyDamageToTargetsInBeam(float DeltaTime);
	void ActorPitchRotation();

	FTimerHandle attackBeginDelayHandle;
	FTimerHandle attackNextPhaseDelayHandle;
	FActiveGameplayEffectHandle unifiedBeamEffectHandle;
	FActiveGameplayEffectHandle targetingBeamEffectHandle;
	FActiveGameplayEffectHandle damagingBeamEffectHandle;
	FSharedPredictionContext sharedPredictionContext;

	ABaseCharacter* owner;

	AActor* currentAttackTarget;
	UMobCharacterMovementComponent* MobCharacterMovementComponent;

	UPROPERTY(ReplicatedUsing = OnRep_PhaseChange)
	EBeamAttackPhase currentPhase = EBeamAttackPhase::Stopped;
};