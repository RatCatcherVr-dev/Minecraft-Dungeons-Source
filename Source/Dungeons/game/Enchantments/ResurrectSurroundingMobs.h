// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "ResurrectSurroundingMobs.generated.h"

UCLASS()
class DUNGEONS_API UResurrectSurroundingMobs : public UEnchantment
{
	GENERATED_BODY()
public:
	UResurrectSurroundingMobs();

	void OnStart() override;
	void OnEnd() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	UPROPERTY(EditDefaultsOnly)
	float ResurrectRadius = 5000.f;

	UPROPERTY(EditDefaultsOnly)
	float ResurrectChance = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float ResurrectTime = 3.f;

	UPROPERTY(EditDefaultsOnly)
	bool bDebugVisuals = false;
private:
	UFUNCTION()
	void OnAreaOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnAreaOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	USphereComponent* Area;
};
