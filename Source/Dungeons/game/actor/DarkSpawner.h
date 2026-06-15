#pragma once

#include "game/actor/WaveSpawner.h"

#include "DarkSpawner.generated.h"


UCLASS()
class DUNGEONS_API ADarkSpawner : public AWaveSpawner {
	GENERATED_BODY()

public:
	ADarkSpawner(const FObjectInitializer& objectInitializer);
	
	void BeginPlay() override;
	
	void Tick(float DeltaTime) override;

private:
	void CalculateFraction(float DeltaTime) override;

	FVector GetSpawnCentrePoint(AActor* actor) const override;

 	void TickSuddenDeath();

	TWeakObjectPtr<class ADimmer> dimmer;

	bool nightModeActive = false;

 	float nightModeTime = 0.f;
};