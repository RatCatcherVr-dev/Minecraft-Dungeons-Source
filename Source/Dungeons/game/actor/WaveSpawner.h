#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "lovika/io/LevelFile.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "game/component/MissionProgressComponent.h"
#include "WaveSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuccessfulMobSpawn);

USTRUCT(BlueprintType)
struct DUNGEONS_API FWeightedMobType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	FString mobType;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float weight;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDLCWeightedMobType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	EDLCName DLCType;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	TArray<FWeightedMobType> MobTypes;
};

class APlayerCharacter;
class AMobCharacter;

UCLASS()
class DUNGEONS_API AWaveSpawner : public APropActor_RepSpatializeStatic
{
	GENERATED_BODY()

public:
	AWaveSpawner(const FObjectInitializer& objectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Despawn(AMobCharacter* mob);

	UFUNCTION()
	virtual void OnGameReady();

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	TArray<FWeightedMobType> weightedMobTypes;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	TArray<FDLCWeightedMobType> dlcWeightedMobTypes;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	class UParticleSystem* spawnParticles;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	class USoundCue* spawnSound;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	class UParticleSystem* cleanupParticles;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	class USoundCue* cleanupSound;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float spawnMinDistance = 500.f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float spawnRadius = 700.f;

	/** The max rate at which mobs spawn (per second); this is multiplied with number of players alive */
	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float spawnRateMax = 0.4f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float waveSizeStart = 1.0f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float waveSizeIncreasePerWave = 0.25f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float waveSizeMax = 4.0f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float waveSizePerPlayerScalingFactor = 0.5f;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	float despawnRate = 0.1f;

	/** The maximum number of alive mobs spawned */
	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	int countMax = 20;

	UPROPERTY(EditAnywhere, Category = "WaveSpawner")
	class USoundCue* despawnSound;

	UPROPERTY(BlueprintAssignable, Category = "WindPillar|PillarCharge")
	FOnSuccessfulMobSpawn OnSuccessfulMobSpawn;

protected:
	virtual void TickSpawner(float DeltaTime);

	virtual void CalculateFraction(float DeltaTime);

	TOptional<FTransform> GenerateTransform(UWorld* world, const TArray<APlayerCharacter*>& players);

	virtual FVector GetSpawnCentrePoint(AActor* actor) const;

protected:
	bool despawnActive = false;

	float despawnAccumulator = 0.f;

	float currentWaveSize = 1.f;

	int NumPlayerAlive = 0;

	float fraction = 0.f;

	int requestedSpawnCount = 0;

	io::MobGroup mobGroup;

	TArray<TWeakObjectPtr<AMobCharacter>> mobs;

	TWeakObjectPtr<class AGameBP> game;

	float debugScaledWaveSize;

	int debugSpawnRequestCount;
};
