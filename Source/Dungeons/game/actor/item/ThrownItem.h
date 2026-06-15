// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "game/actor/PropActor.h"

#include "ThrownItem.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AThrownItem : public APropActor
{
	GENERATED_BODY()
	
public:
	AThrownItem(const class FObjectInitializer& OI);

	void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void OnHitObject(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue* ImpactSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UParticleSystem* ImpactEffect = nullptr;
private:
	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
