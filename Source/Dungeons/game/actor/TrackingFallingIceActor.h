#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "FallingIceActor.h"
#include "game/component/MoveToTargetMovementComponent.h"

#include "TrackingFallingIceActor.generated.h"

// ----- Falling Ice Actor ----- //
UCLASS()
class DUNGEONS_API ATrackingFallingIceActor : public AFallingIceActor {
	GENERATED_BODY()
public:
	ATrackingFallingIceActor();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* target);

	UFUNCTION()
	void OnTrackingComplete();

	void OnSummonPlayed() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float mTrackingTime = 4.f;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	UMoveToTargetMovementComponent* mTrackingComponent;

	UPROPERTY(Replicated)
	AActor* mTrackingTarget;

	FTimerHandle mTrackingTimerHandle;
};
