// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayTagContainer.h"
#include "RecyclerQuiverInstance.generated.h"

struct FOnAttributeChangeData;

/**
 * 
 */
UCLASS()
class DUNGEONS_API ARecyclerQuiverInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;

	int GetDisplayCount() const override;

	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	void OnHit();

	UFUNCTION(NetMulticast, Reliable)
	void OnSetAddArrow(bool addArrow);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
private:

	UPROPERTY(Transient, Replicated)
	bool bCanAddArrow = true;

	bool bShouldAddArrow = false;
};
