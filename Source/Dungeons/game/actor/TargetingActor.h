// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetingActor.generated.h"

DECLARE_DELEGATE_TwoParams(FOnTargetingEnded, FVector, bool);

UCLASS()
class DUNGEONS_API ATargetingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	FOnTargetingEnded OnTargetingEnded;

	UPROPERTY(BlueprintReadWrite)
	float NormalizedMagnitude = 0.0f;

	void BeginPlay() override;

	void Tick(float deltaTime) override;

	void SetTargetActor(AActor* target);
	TWeakObjectPtr<AActor> GetTargetActor() const;

	void AdjustCursorToMouse(class APlayerController* playerController);
protected:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void TargetSelected(FVector target);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void TargetingCanceled();

	UPROPERTY(BlueprintReadWrite)
	FVector CurrentTarget;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float MoveSpeed = 2000.0f;
private:
	void TryOffsetFromGamePadAxes(APlayerController* playerController, float deltaTime);
	void UpdateToTargetActorLocation();

	FVector GetCameraOrientedTiltDirection(APawn* pawn) const;
	FVector FindBelowGeometry() const;

	void OnAxisX(float value);
	void OnAxisY(float value);

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	TWeakObjectPtr<AActor> TargetActor;
};
