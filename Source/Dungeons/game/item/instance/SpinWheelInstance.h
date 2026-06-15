// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "SpinWheelInstance.generated.h"

DECLARE_DELEGATE(FOnReturn);
DECLARE_DELEGATE(FOnHit);

UCLASS()
class DUNGEONS_API USpinWheelGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	USpinWheelGameplayEffect();
};

USTRUCT()
struct DUNGEONS_API FVectorSpeedInfo {
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	float TargetSize;
	UPROPERTY(EditDefaultsOnly)
	float Acceleration;
};

USTRUCT()
struct DUNGEONS_API FVectorForce {
	GENERATED_BODY()
public:
	FVectorForce() {}
	FVectorForce(FVector force, FVectorSpeedInfo speedInfo);

	void Update(float DeltaTime);

	void SetDirectionKeepSize(const FVector& dir);

	FVector Vector;
private:
	FVectorSpeedInfo SpeedInfo;
};

enum class EWheelStateType {
	None,
	Launch,
	Arc,
	Return
};

struct ForceWithDirection {
	ForceWithDirection(FVectorForce force, std::function<void(FVectorForce&)> dirSetter)
		: Force(force)
		, DirectionSetter(dirSetter) {
	}
	FVectorForce Force;
	std::function<void(FVectorForce&)> DirectionSetter;
};

class WheelState {
public:
	WheelState(
		std::function<void(WheelState&)> startConfiguration,
		std::function<bool(WheelState&)> completeCondition,
		std::function<void(const FVector&)> moveVectorSetter,
		ASpinWheelActor* wheel,
		TArray<ForceWithDirection> forces,
		EWheelStateType state,
		EWheelStateType nextState,
		TMap<FString, float> floatOptions,
		TMap<FString, FVector> vectorOptions);

	EWheelStateType Update(float deltaTime);

	std::function<void(WheelState&)> StartConfiguration;
	std::function<bool(WheelState&)> CompleteCondition;
	std::function<void(const FVector&)> MoveVectorSetter;
	ASpinWheelActor* Wheel;
	TArray<ForceWithDirection> Forces;
	EWheelStateType State;
	EWheelStateType NextState;
	TMap<FString, float> FloatOptions;
	TMap<FString, FVector> VectorOptions;
};

UCLASS()
class DUNGEONS_API ASpinWheelActor : public AActor {
	GENERATED_BODY()
public:
	ASpinWheelActor();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

	void Tick(float deltaTime) override;

	FVector MoveVector;
	float Damage;

	float GetMaxSpeed() const;

	FOnReturn OnReturn;
	FOnHit OnHit;
protected:
	// Max speed of the sum movement vector 
	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed;

	// Min distance spin wheel has to travel in the launch phase
	UPROPERTY(EditDefaultsOnly)
	float LaunchDistance;
	// Duration of the arc phase
	UPROPERTY(EditDefaultsOnly)
	float ArcDurationSeconds;
	// Counter force of the launch vector, always opposite of launch vector
	UPROPERTY(EditDefaultsOnly)
	FVectorSpeedInfo LaunchDecelerationInfluence;
	// Left side directional influence in the launch phase
	UPROPERTY(EditDefaultsOnly)
	FVectorSpeedInfo LaunchSideInfluence;
	// Downward pull in the arc phase, always points towards player
	UPROPERTY(EditDefaultsOnly)
	FVectorSpeedInfo ArcDownwardInfluence;
	// Left side directional influence in the arc phase
	UPROPERTY(EditDefaultsOnly)
	FVectorSpeedInfo ArcSideInfluence;
	// Return Influence, always points towards player
	UPROPERTY(EditDefaultsOnly)
	FVectorSpeedInfo ReturnInfluence;
	// Distance to player where it will suck in towards player in the return phase
	UPROPERTY(EditDefaultsOnly)
	float SuckInDistance;

	// Distance to player where it will be absorbed
	UPROPERTY(EditDefaultsOnly)
	float AbsorbDistance;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ImpactTag;

	UFUNCTION(BlueprintImplementableEvent)
	void OnReturnedToPlayer();
private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	EWheelStateType State;
	TMap<EWheelStateType, WheelState> States;
};


UCLASS()
class DUNGEONS_API ASpinWheelInstance : public AItemInstance {
	GENERATED_BODY()
public:
	ASpinWheelInstance();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

	void Activate(const FPredictionKey& predictionKey) override;
	bool CanActivate() const override;

	bool IsBusy() const override;

	float GetStats(EItemStats stat) const;

protected:
	// The initial launch velocity 
	UPROPERTY(EditDefaultsOnly)
	float LaunchVelocity;

	// Launch offset relative to player's forward vector
	UPROPERTY(EditDefaultsOnly)
	FVector LaunchOffset;

	// Rotation offset of the initial launch vector
	UPROPERTY(EditDefaultsOnly)
	FRotator InitialDirectionOffsetRotation;

	// Delay until spin wheel launches
	UPROPERTY(EditDefaultsOnly)
	float LaunchDelay;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASpinWheelActor> SpinWheelActorClass;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 100.f;

	int hits = 0;

private:
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerSpawnWheel(const FPredictionKey& predictionKey, const FVector& direction);

	void SpawnWheel(const FVector& direction);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnWheel(const FPredictionKey& predictionKey, const FVector& direction);

	void OnLaunch(FPredictionKey predictionKey);
	void OnWheelHit();
	void OnWheelReturn();

	FVector GetWheelDirection() const;

	TWeakObjectPtr<ASpinWheelActor> Wheel;
	FTimerHandle TimerHandle;
};