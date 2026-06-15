// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "PlentifulQuiverInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API APlentifulQuiverInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	int GetDisplayCount() const override;

	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	UFUNCTION(NetMulticast, Reliable)
	void OnSetCanAddArrow();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	UPROPERTY(Transient, Replicated)
	bool bCanAddArrow = true;
};
