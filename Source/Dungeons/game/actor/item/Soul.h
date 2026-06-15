#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "game/component/HealthComponent.h"
#include "Soul.generated.h"

class AGearItemInstance;

UCLASS()
class DUNGEONS_API ASoul : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoul();

	void LaunchSoul();

	void StopSoul(bool PushToStore = true);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons|Soul")
	void OnShouldAbsorb();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons|Soul")
	void OnLaunchSoul();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons|Soul")
	void OnStopSoul();

	void Tick(float DeltaTime) override;

	void Initialize(FVector spawnPos, FVector risePos, float travelTime, float riseTime);
private:
	FVector SpawnPosition;

	FVector RiseToPosition;

	float RiseSpeedMultiplier = 1.0f;

	float StartTime;

	float TravelTime;
	float RiseTime;
};
