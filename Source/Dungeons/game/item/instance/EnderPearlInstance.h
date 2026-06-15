// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/item/instance/ThrowableItemInstance.h"
#include "game/actor/item/EnderPearlItem.h"
#include "EnderPearlInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AEnderPearlInstance : public AThrowableItemInstance
{
	GENERATED_BODY()
public:
	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	//void OnMouseMoved(float mouseX, float mouseY) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnderPearlItem> ClassToSpawn;
	
private:
	UPROPERTY(Transient, Replicated)
	float MaxDistance = 700.0f;
};
