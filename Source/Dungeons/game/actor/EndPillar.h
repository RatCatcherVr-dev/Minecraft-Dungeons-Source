#pragma once

#include "game/Game.h"
#include "game/util/Pushback.h"
#include "game/actor/WaveSpawner.h"

#include "EndPillar.generated.h"


class ABaseCharacter;
class APlayerCharacter;
class AMobCharacter;

UENUM(BlueprintType)
enum class EEndPillarState : uint8
{
	Inactive,
	Active,
	DrainCharge,
	RegenerateCharge,
	RapidRecharging,
	TimedDelay,
	Depleted,
	Defeated
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndPillarStateChange, EEndPillarState, NewState);

UCLASS()
class DUNGEONS_API AEndPillar : public AWaveSpawner
{
	GENERATED_BODY()
	
public:	
	AEndPillar(const FObjectInitializer& objectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EndPillar")
	EEndPillarState GetPillarState() const;

	UFUNCTION(BlueprintCallable, Category = "EndPillar")
	void ActivatePillar();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EndPillar|Tracking")
	bool IsPlayerPresent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EndPillar|Tracking")
	bool IsMobPresent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EndPillar|PillarCharge")
	bool PillarHasCharge() const;

	UFUNCTION(BlueprintCallable, Category = "EndPillar|PillarCharge", meta = (ToolTip = "Call to immediately deplete any remaining charge from this pillar"))
	void DepleteCharge(bool ResetDelay = true);

	UFUNCTION(BlueprintCallable, Category = "EndPillar|PillarCharge")
	void StartRecharge();
	
	UFUNCTION(BlueprintCallable, Category = "EndPillar|PillarCharge")
	void SetChargeValue(float NewCharge);

	UFUNCTION(BlueprintCallable, Category = "EndPillar|PillarCharge", meta = (ToolTip = "Call to disable the pillar after the boss encounter is defeated, depletes any remaining charge"))
	void DisablePillar();

	UPROPERTY(BlueprintAssignable, Category = "EndPillar")
	FOnEndPillarStateChange OnEndPillarStateChange;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EndPillar|Debugging")
	bool GetDebugMode() const;

	UFUNCTION(BlueprintCallable, Category = "EndPillar|Tracking")
	bool AddCharacterToTracking(ABaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable, Category = "EndPillar|Tracking")
	bool RemoveCharacterFromTracking(ABaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable, Category = "EndPillar|Tracking")
	void ClearAllTracking();

	UFUNCTION(BlueprintCallable, Category = "EndPillar|Tracking")
	TArray<APlayerCharacter*> GetTrackedPlayers();

	UFUNCTION(BlueprintCallable, Category = "EndPillar|Pushback")
	void ApplyAreaPushback();

	UFUNCTION(BlueprintCallable, Category = "EndPillar")
	void SetPillarState(const EEndPillarState NewState);

	UFUNCTION(BlueprintCallable, Category = "EndPillar")
	void ResetTimerDelay();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "EndPillar|OnRep")
	void OnRep_CurrentCharge();

	UFUNCTION()
	void OnRep_PillarState();


private:

#if WITH_EDITOR || UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	void DebugMessage(const FString& Message, float TimeToDisplay, FColor TextColor = FColor::White);

	void TickDebugging(float DeltaTime);
#endif

	bool AnyTrackedPlayerIsAlive() const;

	void UpdateState();

	void DrainCharge();

	void RegenerateCharge();

	void RapidRecharge();

	void TimedDelay(float DeltaTime);

	void SpawnInitialWave();

protected:
	UPROPERTY(EditAnywhere, Category = "EndPillar|Debugging")
	bool DebugMode = false;
	
	UPROPERTY(EditAnywhere, Category = "EndPillar|Debugging", meta = (ToolTip = "Requires DebugMode to be enabled"))
	bool DebugSpawning = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar", Replicated)
	bool BossPillar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|InitialWave")
	bool InitialWave = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|InitialWave", meta = (ClampMin = 0))
	int InitialWaveSize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|PillarCharge", meta = (ClampMin = 1.f, ToolTip = "Pillars maximum charge value"))
	float MaximumCharge = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Modifies the rate at which a player will drain the pillar [1 * Multiplier] per tick"))
	float DrainChargeMultiplier = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Pillars natural recharge rate when a player is not present, amplified by any mobs present [(1 * Multiplier) * No.OfMobs] per tick"))
	float RechargeMultiplier = 4.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Seperate pillar recharge rate (ignoring mobs), used by a BossMode enabled pillar [1 * Multiplier] per tick - r.e. Boss arm re-activation"))
	float RapidRechargeMultiplier = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Delay in seconds before a BossMode pillar will begin the rapid recharge phase after being depleted"))
	float RapidRechargeDelay = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|Pushback")
	FPushback PushbackValues{ true, 5.f, 1.f, false, 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|Pushback", meta = (ClampMin = 0.f))
	float PushbackTimerMinimum = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|Pushback", meta = (ClampMin = 0.f))
	float PushbackTimerMaximum = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EndPillar|Pushback", meta = (ClampMin = 0))
	int32 PushbackDamage = 100;

	UPROPERTY(BlueprintReadWrite, Category = "EndPillar|Pushback")
	FTimerHandle PushbackTimerHandle;

private:
	UPROPERTY(BlueprintReadOnly, Category = "EndPillar|PillarCharge", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentCharge)
	float CurrentCharge = -1.f;

	UPROPERTY(BlueprintReadOnly, Category = "EndPillar|Tracking", meta = (AllowPrivateAccess = "true"))
	TArray<APlayerCharacter*> TrackedPlayerCharacters;

	UPROPERTY(BlueprintReadOnly, Category = "EndPillar|Tracking", meta = (AllowPrivateAccess = "true"))
	TArray<AMobCharacter*> TrackedMobCharacters;

	UPROPERTY(ReplicatedUsing = OnRep_PillarState)
	EEndPillarState State = EEndPillarState::Inactive;

	bool ActivatedOnce;

	float DelayTimer;

	int32 DebugMessageIndex;
};
