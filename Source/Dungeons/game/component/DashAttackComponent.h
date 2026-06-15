// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/component/AttackComponent.h"
#include <NavigationQueryFilter.h>
#include "DashAttackComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UDashAttackComponent : public UAttackComponent
{
	GENERATED_BODY()
	
public:
	UDashAttackComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float GetAttackRange() const override;

	bool CanAttack(AActor* attackTarget = nullptr) const override;

	bool IsWithinRange(AActor* attacktarget) const;

	bool IsAtLocation(AActor* attacktarget);

	void SetMovmementComponentSpeedParams();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	float maxAttackRangeRangeUnits = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	float minAttackRangeRangeUnits = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	float DashSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	float AcceptableAtLocationRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	bool DebugVisuals = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|DashAttackComponent")
	TSubclassOf<UNavigationQueryFilter> DefaultNavigationFilterClass;

protected:
	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;

private:
	class FloatRange AttackRange() const;
};
