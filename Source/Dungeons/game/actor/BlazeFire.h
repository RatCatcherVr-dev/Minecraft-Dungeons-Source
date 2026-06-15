#pragma once

#include "BlazeFire.generated.h"

UCLASS()
class DUNGEONS_API ABlazeFire : public AActor {
	GENERATED_BODY()

public:
	ABlazeFire(const FObjectInitializer& objectInitializer);	

	void Tick(float deltaTime) override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	float healFraction = 0.03f;

	UPROPERTY(EditDefaultsOnly)
	float radius = 75.f;
};