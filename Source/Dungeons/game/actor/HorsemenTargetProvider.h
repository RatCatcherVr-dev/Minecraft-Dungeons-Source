#pragma once

#include "character/player/PlayerCharacter.h"
#include <GameplayTagContainer.h>
#include "HorsemenTargetProvider.generated.h"

UCLASS()
class DUNGEONS_API AHorsemenTargetProvider : public AActor {
	GENERATED_BODY()

public:
	AHorsemenTargetProvider(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;

	UFUNCTION()
	void ApplyDamage(float damage, AActor* byWhom);

	AActor* GetMostAggresive() const;
	
	void Tick(float DeltaTime);

	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

private:

	UPROPERTY(EditAnywhere)
	float reducePerSecond;

	// there's no maxheap in unreal; a map is ok since we have max 4 players
	TMap<uint32, float> map;
};