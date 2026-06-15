
#pragma once

#include "CoreMinimal.h"
#include "PlayerCoopCamera.generated.h"

class USceneComponent;
class ULovikaSpringArmComponent;
class UCameraComponent;

UCLASS(BlueprintType)
class DUNGEONS_API APlayerCoopCamera : public AActor
{
	GENERATED_BODY()

public:
	APlayerCoopCamera(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BecomeViewTarget(class APlayerController* PC) override;
	virtual void EndViewTarget(class APlayerController* PC) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	USceneComponent* CameraAim;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	ULovikaSpringArmComponent* LovikaSpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	bool AllowPopping = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	bool IsViewTarget = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	float ZoomAmount = 0.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	bool CanPop();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	float MaximumZoomMultiplier = 1.f;

	UFUNCTION()
	void BlockPopping(float val);

	UFUNCTION()
	void UnblockPopping();
	
};
