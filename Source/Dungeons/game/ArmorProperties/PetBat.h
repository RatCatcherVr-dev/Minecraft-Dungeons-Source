// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include <GameplayEffect.h>
#include "PetBat.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API UPetGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPetGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API UPetBat : public UArmorProperty
{
	GENERATED_BODY()
public:
	UPetBat();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPetGameplayEffect> Effect;

private:
	void OnMobSpawned(AMobCharacter*);
	
	TWeakObjectPtr<AMobCharacter> Bat;

	bool TrySpawnBat();

	void Relocate();

	FTimerHandle relocateHandle;
};
