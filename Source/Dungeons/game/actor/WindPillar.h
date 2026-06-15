#pragma once

#include "game/Game.h"
#include "game/util/Pushback.h"
#include "game/actor/WaveSpawner.h"

#include "WindPillar.generated.h"


class ABaseCharacter;
class APlayerCharacter;
class AMobCharacter;

UENUM(BlueprintType)
enum class EPillarState : uint8
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPillarStateChange, EPillarState, NewState);

UCLASS()
class DUNGEONS_API AWindPillar : public AWaveSpawner
{
	GENERATED_BODY()
	
public:	
	AWindPillar(const FObjectInitializer& objectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar")
	EPillarState GetPillarState() const;

	UFUNCTION(BlueprintCallable, Category = "WindPillar")
	void ActivatePillar();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar|Tracking")
	bool IsPlayerPresent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar|Tracking")
	bool IsMobPresent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar|PillarCharge")
	bool PillarHasCharge() const;

	UFUNCTION(BlueprintCallable, Category = "WindPillar|PillarCharge", meta = (ToolTip = "Call to immediately deplete any remaining charge from this pillar"))
	void DepleteCharge(bool ResetDelay = true);

	UFUNCTION(BlueprintCallable, Category = "WindPillar|PillarCharge")
	void StartRecharge();
	
	UFUNCTION(BlueprintCallable, Category = "WindPillar|PillarCharge")
	void SetChargeValue(float NewCharge);

	UFUNCTION(BlueprintCallable, Category = "WindPillar|PillarCharge", meta = (ToolTip = "Call to disable the pillar after the boss encounter is defeated, depletes any remaining charge"))
	void DisablePillar();

	UPROPERTY(BlueprintAssignable, Category = "WindPillar")
	FOnPillarStateChange OnPillarStateChange;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar|Debugging")
	bool GetDebugMode() const;

	UFUNCTION(BlueprintCallable, Category = "WindPillar|Tracking")
	bool AddCharacterToTracking(ABaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable, Category = "WindPillar|Tracking")
	bool RemoveCharacterFromTracking(ABaseCharacter* BaseCharacter);

	UFUNCTION(BlueprintCallable, Category = "WindPillar|Tracking")
	void ClearAllTracking();

	UFUNCTION(BlueprintCallable, Category = "WindPillar|Tracking")
	TArray<APlayerCharacter*> GetTrackedPlayers();

	UFUNCTION(BlueprintCallable, Category = "WindPillar|Pushback")
	void ApplyAreaPushback();

	UFUNCTION(BlueprintCallable, Category = "WindPillar")
	void SetPillarState(const EPillarState NewState);

	UFUNCTION(BlueprintCallable, Category = "WindPillar")
	void ResetTimerDelay();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "WindPillar|OnRep")
	void OnRep_CurrentCharge();

	UFUNCTION()
	void OnRep_PillarState();


private:
	void DebugMessage(const FString& Message, float TimeToDisplay, FColor TextColor = FColor::White);

	void TickDebugging(float DeltaTime);

	bool AnyTrackedPlayerIsAlive() const;

	void UpdateState();

	void DrainCharge();

	void RegenerateCharge();

	void RapidRecharge();

	void TimedDelay(float DeltaTime);

	void SpawnInitialWave();

protected:
	UPROPERTY(EditAnywhere, Category = "WindPillar|Debugging")
	bool DebugMode = false;
	
	UPROPERTY(EditAnywhere, Category = "WindPillar|Debugging", meta = (ToolTip = "Requires DebugMode to be enabled"))
	bool DebugSpawning = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar", Replicated)
	bool BossPillar = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|InitialWave")
	bool InitialWave = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|InitialWave", meta = (ClampMin = 0))
	int InitialWaveSize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|PillarCharge", meta = (ClampMin = 1.f, ToolTip = "Pillars maximum charge value"))
	float MaximumCharge = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Modifies the rate at which a player will drain the pillar [1 * Multiplier] per tick"))
	float DrainChargeMultiplier = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Pillars natural recharge rate when a player is not present, amplified by any mobs present [(1 * Multiplier) * No.OfMobs] per tick"))
	float RechargeMultiplier = 4.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Seperate pillar recharge rate (ignoring mobs), used by a BossMode enabled pillar [1 * Multiplier] per tick - r.e. Boss arm re-activation"))
	float RapidRechargeMultiplier = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|PillarCharge", meta = (ClampMin = 0.f, ToolTip = "Delay in seconds before a BossMode pillar will begin the rapid recharge phase after being depleted"))
	float RapidRechargeDelay = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|Pushback")
	FPushback PushbackValues{ true, 5.f, 1.f, false, 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|Pushback", meta = (ClampMin = 0.f))
	float PushbackTimerMinimum = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|Pushback", meta = (ClampMin = 0.f))
	float PushbackTimerMaximum = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WindPillar|Pushback", meta = (ClampMin = 0))
	int32 PushbackDamage = 100;

	UPROPERTY(BlueprintReadWrite, Category = "WindPillar|Pushback")
	FTimerHandle PushbackTimerHandle;

private:
	UPROPERTY(BlueprintReadOnly, Category = "WindPillar|PillarCharge", meta = (AllowPrivateAccess = "true"), ReplicatedUsing = OnRep_CurrentCharge)
	float CurrentCharge = -1.f;

	UPROPERTY(BlueprintReadOnly, Category = "WindPillar|Tracking", meta = (AllowPrivateAccess = "true"))
	TArray<APlayerCharacter*> TrackedPlayerCharacters;

	UPROPERTY(BlueprintReadOnly, Category = "WindPillar|Tracking", meta = (AllowPrivateAccess = "true"))
	TArray<AMobCharacter*> TrackedMobCharacters;

	UPROPERTY(ReplicatedUsing = OnRep_PillarState)
	EPillarState State = EPillarState::Inactive;

	bool ActivatedOnce;

	float DelayTimer;

	int32 DebugMessageIndex;
};
