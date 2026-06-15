// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/util/Pushback.h"
#include <GameplayPrediction.h>
#include <GameplayEffect.h>
#include "GameplayEffectDamageComponent.generated.h"

class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UGameplayEffectDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGameplayEffectDamageComponent();

	UFUNCTION(BlueprintCallable)
	void AttackLocal(ABaseCharacter* target);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	FPushback Pushback;
private:
	void ApplyDamage(ABaseCharacter* target);
};
