// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/ContinousDamageActor.h"
#include "game/actor/ImpactActionHandler.h"
#include "ShockWeb.generated.h"

UCLASS()
class DUNGEONS_API UShockWebGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShockWebGameplayEffect();
};

UCLASS()
class DUNGEONS_API AShockWebNodeActor : public AContinousDamageActor {
	GENERATED_BODY()
public:
	AShockWebNodeActor(const FObjectInitializer& ObjectInitializer);

	void SetTarget(AShockWebNodeActor*, float Range);
	void Tick(float) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeamDisabled();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeamEnabled();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDetached();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWillDestroy();
	
	void Attach(const FImpactInfo&);

	void OnCharacterDeath();

	void Expire();
	
	//Offset along -velocity vector when spawning in the world.
	UPROPERTY(EditDefaultsOnly)
	float WorldSpawnOffset = 40.f;
	
protected:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;	
	
	void ToggleBeam(bool enabled);

	void LifeSpanExpired() override;

	//Offset away from self along traced
	UPROPERTY(EditDefaultsOnly)
	float TraceBiasTerrain = 51;

	UPROPERTY(EditDefaultsOnly)
	float AutoDestroyDelaySeconds = 1.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* DamageCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UActorBeamTargetUpdaterComponent* BeamUpdater = nullptr;

	USceneComponent* Rotator = nullptr;

private:
	UFUNCTION()
	void OnRep_IsActive();

	UFUNCTION()
	void OnRep_IsFading();

	UFUNCTION()
	void OnRep_Target();
	
	float CurrentTraceBias = 0;

	float Range = 0;

	UPROPERTY(ReplicatedUsing = OnRep_IsActive, Transient)
	bool bIsBeamActive = false;

	UPROPERTY(ReplicatedUsing = OnRep_IsFading, Transient)
	bool bIsFading = false;

	UPROPERTY(ReplicatedUsing = OnRep_Target, Transient)
	TWeakObjectPtr<AShockWebNodeActor> Target;

	FTraceHandle BeamTrace;
};

UCLASS()
class DUNGEONS_API UShockWeb : public UEnchantment
{
	GENERATED_BODY()
public:
	UShockWeb();
protected:
	UPROPERTY()
	TArray<AShockWebNodeActor*> SpawnedActors;

	UPROPERTY(EditDefaultsOnly)
	float Range = 2400.f;

	UPROPERTY(EditDefaultsOnly)
	float DamagePerSecond = 37.5f;

	UPROPERTY(EditDefaultsOnly)
	float Period = 0.25f;

	UPROPERTY(EditDefaultsOnly)
	float Duration = 20.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AShockWebNodeActor> ActorToSpawn;

	void OnProjectileLaunch(ABaseProjectile* fromProjectile) override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TArray<TSubclassOf<ABaseProjectile>> DeniedProjectiles;	

	void OnNodeSpawned(AActor*, const FImpactInfo&,  FGameplayEffectSpecHandle specHandle);
};