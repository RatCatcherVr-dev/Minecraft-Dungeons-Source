#pragma once

#include "GameplayEffect.h"
#include "../abilities/effects/WorldDamageGameplayEffect.h"
#include "Bubbled.generated.h"

UCLASS()
class DUNGEONS_API UBubbledHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void ApplyBubbleEffect(TSubclassOf<UBubbledGameplayEffect> BubbleEffect, AActor* InstigatorActor, AActor* InstigatorProjectile, AActor* ActorToBubble);

	UFUNCTION(BlueprintCallable)
	static void DealDamageOnRadius(TSubclassOf<UBubbledDamageGameplayEffect> DamageEffect, float magnitude, AActor* BubbledActor, AActor* EffectInstigator);
};

UCLASS()
class DUNGEONS_API UBubbledGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBubbledGameplayEffect(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "BubbleGameplayEffect")
	float Duration = 3.f;
};

UCLASS()
class DUNGEONS_API UBubbledDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBubbledDamageGameplayEffect();

	UPROPERTY(EditAnywhere, Category = "BubbleGameplayEffect")
	float PopDamageRadius = 400.0f;

	UPROPERTY(EditAnywhere, Category = "BubbleGameplayEffect")
	float Damage = 40.0f;
};