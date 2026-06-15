#pragma once

#include "ObjectiveRing.generated.h"

UCLASS()
class DUNGEONS_API AObjectiveRing : public AActor {
	GENERATED_BODY()

public:
	AObjectiveRing(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;	

	void Tick(float deltaTime) override;
	
	void SetPath(const FVector& _source, const FVector& _target);
	
private:
	UPROPERTY(EditDefaultsOnly)
	float speed = 1.0f;

	FVector source;
	FVector target;

	float fraction;
};