#pragma once

#include <CoreMinimal.h>
#include <Components/SphereComponent.h>
#include <GameplayEffect.h>
#include "GameplayEffectTriggerSphereComponent.generated.h"

class AActor;
class UPrimitiveComponent;

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UGameplayEffectTriggerSphereComponent : public USphereComponent
{
	GENERATED_BODY()
public:
	UGameplayEffectTriggerSphereComponent();

	void BeginPlay();
	//void EndPlay(EEndPlayReason::Type EndPlayReason); // @todo: if this is ever on an actor with limited lifetime, we need to remove the applied effects when removed

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Player Only Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> PlayerOnlyEffects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mob Only Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> MobOnlyEffects;
private:
	/** Apply a gameplay effect to actor */
	FActiveGameplayEffectHandle ApplyGameplayEffect(UAbilitySystemComponent& abilitySystem, UGameplayEffect& effectTemplate, AActor* instigator);
	UFUNCTION(BlueprintCallable, Category = GameplayEffects, meta = (DisplayName = "ApplyGameplayEffectToActor", ScriptName = "ApplyGameplayEffectToActor"))
	FActiveGameplayEffectHandle BP_ApplyGameplayEffectToActor(UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass);
	/** Remove a gameplay effect from actor */
	void RemoveGameplayEffect(UAbilitySystemComponent& abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass, AActor* instigator);
	UFUNCTION(BlueprintCallable, Category = GameplayEffects, meta = (DisplayName = "RemoveGameplayEffectFromActor", ScriptName = "RemoveGameplayEffectFromActor"))
	void BP_RemoveGameplayEffectFromActor(UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TMap<AActor*, TArray<FActiveGameplayEffectHandle>> ActiveGameplayEffectsHandles;
};