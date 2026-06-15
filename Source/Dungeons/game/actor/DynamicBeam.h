#pragma once

#include "character/player/PlayerCharacter.h"
#include "DynamicBeam.generated.h"

UCLASS()
class DUNGEONS_API ADynamicBeam : public AActor {
	GENERATED_BODY()

public:
	ADynamicBeam(const FObjectInitializer& objectInitializer);
	
	void UpdateBeam();

	bool CanMaintainBeam() const;

	void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Spawn(ABaseCharacter* source, ABaseCharacter* target);

	UFUNCTION(BlueprintCallable)
	void Despawn();

	UFUNCTION(BlueprintNativeEvent)
	void OnSpawn(UParticleSystemComponent* spawnedBeam);
	
private:
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* emitter;

	UParticleSystemComponent* beam;

	TWeakObjectPtr<ABaseCharacter> sourceActor;
	TWeakObjectPtr<ABaseCharacter> targetActor;

	UPROPERTY(EditDefaultsOnly)
	FName sourceSocketName;

	UPROPERTY(EditDefaultsOnly)
	FName targetSocketName;

	bool useSourceSocket;
	bool useTargetSocket;

	UPROPERTY(EditDefaultsOnly)
	int32 emitterIndex = 0;	

	UPROPERTY(EditDefaultsOnly)
	float distanceMax = 700.f;

	UPROPERTY(EditDefaultsOnly)
	FName intensityParameterName;
};