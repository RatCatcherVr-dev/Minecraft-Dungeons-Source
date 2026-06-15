#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NoExportTypes.h"
#include "DungeonsProjectileMovementComp.generated.h"

/**
 *
 */
UCLASS()
class DUNGEONS_API UDungeonsProjectileMovementComp : public UProjectileMovementComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnProjectileStartsSinking);

protected:

	FVector ComputeVelocity(FVector InitialVelocity, float DeltaTime) const override;
public:

	// Arrow travel resistance. The bigger the value the less resistance we will have
	UPROPERTY(EditAnywhere, Category = "DungeonsProjectileMovementComp | Resistance", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float Drag = 0.93f;

	// At which speed want the projectile to start sinking
	UPROPERTY(EditAnywhere, Category = "DungeonsProjectileMovementComp | Resistance", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SpeedToleranceToSink = 100000.f;
	
	// How much time do we want to wait until the drag kicks in
	UPROPERTY(EditAnywhere, Category = "DungeonsProjectileMovementComp | Resistance")
	float TimeToKickDragIn = 0.1f;

	// How much time do we want to wait until the drag kicks in
	UPROPERTY(EditAnywhere, Category = "DungeonsProjectileMovementComp | Resistance")
	float FallRate = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DungeonsProjectileMovementComp | Resistance")
	bool bUseUnderWaterPhysics = false;

	UFUNCTION(BlueprintCallable, Category = "DungeonsProjectileMovementComp")
	void ResetComponent();

	UFUNCTION(BlueprintPure, Category = "DungeonsProjectileMovementComp")
	bool IsSinking() const;

	UPROPERTY(BlueprintAssignable, Category = "DungeonsProjectileMovementComp Delegates")
	FOnProjectileStartsSinking OnProjectileStartsSinking;

	void SetUseUnderWaterPhysics(bool useUnderWaterPhysics);
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	void StoreInitialVelocity();

private:

	bool bIsSinking;
	float TimeSinceLaunched = 0.f;	
	float InitialLaunchSpeed = 0.f;
	float SinkStartTime = 0.f;
	FVector InitialLaunchVector;
};
