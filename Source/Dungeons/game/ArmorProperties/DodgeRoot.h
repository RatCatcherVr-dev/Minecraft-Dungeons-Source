// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "DodgeRoot.generated.h"


UCLASS()
class DUNGEONS_API UDodgeRootVisualGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()

public:
	UDodgeRootVisualGameplayEffect();
};

UCLASS()
class DUNGEONS_API UDodgeRootGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()

public:
	UDodgeRootGameplayEffect();
};

UCLASS()
class DUNGEONS_API UDodgeRootPoisonGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()

public:
	UDodgeRootPoisonGameplayEffect();
};

UCLASS()
class DUNGEONS_API UDodgeRoot : public UArmorProperty
{
	GENERATED_BODY()

public:
	UDodgeRoot();
protected:
	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;

	FActiveGameplayEffectHandle Handle;

	UPROPERTY(EditDefaultsOnly)
	float Range = 700.f;

	UPROPERTY(EditDefaultsOnly)
	float RootDuration = 3.f;

	UPROPERTY(EditDefaultsOnly)
	float RootDelay = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	int NumberOfMobsToRoot = 2;

	UPROPERTY(EditDefaultsOnly)
	float DamagePerSecond = 45.f;

	UPROPERTY(EditDefaultsOnly)
	float Period = 0.5f;
private:
	
	FGameplayEffectSpec CreateRootSpec() const;
	FGameplayEffectSpec CreatePoisonSpec() const;

	void ApplyStun(const TArray<ABaseCharacter*>&, FPredictionKey);
	void ApplyStunDeferred(TArray<FGameplayEffectSpec> Specs, TArray<TWeakObjectPtr<ABaseCharacter>>);
};
