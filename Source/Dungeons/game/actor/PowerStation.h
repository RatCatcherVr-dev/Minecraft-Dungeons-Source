#pragma once

#include "game/Game.h"
#include "PowerStation.generated.h"


UENUM(BlueprintType)
enum class EPowerStationState : uint8
{
	Inactive,
	Active,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerStationStateChange, EPowerStationState, NewState);

UCLASS()
class DUNGEONS_API APowerStation : public AActor_RepAlways
{
	GENERATED_BODY()
	
public:	
	APowerStation(const FObjectInitializer& objectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PowerStation")
	USkeletalMeshComponent* PowerStationMesh;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WindPillar")
	EPowerStationState GetPowerStationState() const;

	UFUNCTION(BlueprintCallable, Category = "WindPillar")
	void ActivatePowerStation();

	UFUNCTION(BlueprintCallable, Category = "WindPillar|PillarCharge", meta = (ToolTip = "Call to disable the pillar after the boss encounter is defeated, depletes any remaining charge"))
	void DisablePowerStation();

	UPROPERTY(BlueprintAssignable, Category = "WindPillar")
	FOnPowerStationStateChange OnPowerStationStateChange;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_PowerStationState();

	UFUNCTION(BlueprintImplementableEvent)
	void PowerStationOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStateChanged();

private:
	UPROPERTY(ReplicatedUsing = OnRep_PowerStationState)
	EPowerStationState State = EPowerStationState::Inactive;
};
