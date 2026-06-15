#pragma once

#include "game/util/ActorQuery.h"
#include "EffectsActor.generated.h"


USTRUCT(BlueprintType)
struct DUNGEONS_API FSpawnEffects {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	UParticleSystem* PreSpawnMediumParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	UParticleSystem* PreSpawnLargeParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	UParticleSystem* SpawnMediumParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	UParticleSystem* SpawnLargeParticles = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	USoundCue* PreSpawnSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	USoundCue* SpawnSound = nullptr;
};


UCLASS()
class DUNGEONS_API AEffectsActor : public AActor {
	GENERATED_BODY()

public:
	AEffectsActor(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnEffectsAtLocation(UParticleSystem* emitter, USoundCue* sound, const FVector& location);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerSpawnEffectsAtLocation(UParticleSystem* emitter, USoundCue* sound, const FVector& location);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnEffectsAtLocation(UParticleSystem* emitter, USoundCue* sound, const FVector& location);
	
	void PreSpawnEffectsAtLocation(FString packName, bool isLarge, const FVector& location);
		
	void SpawnEffectsAtLocation(FString packName, bool isLarge, const FVector& location);

	UFUNCTION(BlueprintCallable)
	void SpawnEffectsInRadius(UParticleSystem* emitter, USoundCue* sound, const FVector& location, float areaRadius, float instanceRadius);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arena")
	TMap<FString, FSpawnEffects> packs;	

	static AEffectsActor* GetInstance(UWorld* world);
};