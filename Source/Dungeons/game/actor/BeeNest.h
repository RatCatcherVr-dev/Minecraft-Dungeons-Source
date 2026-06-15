// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameplayTagContainer.h>
#include "world/entity/EntityTypes.h"
#include <GameplayEffect.h>
#include <Optional.h>
#include "repgraph/Actor_RepSpatializeStatic.h"
#include <AbilitySystemInterface.h>
#include "character/mob/MobCharacter.h"
#include "BeeNest.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnExipired)

UCLASS()
class DUNGEONS_API ABeeNest : public AActor_RepSpatializeStatic, public IAbilitySystemInterface
{
	GENERATED_BODY()
private:
	class UAbilitySystemComponent* AbilitySystemComponent;
	FTimerHandle EnqueuedSpawnHandle;
	FTimerHandle TimeToLiveHandle;

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type) override;

	
	void OnBeeDeath();
	void SummonBee();
	void EnqueueBeeSummon();
	void OnLifetimeExipired();

	void OnMobsSummoned(const TArray<AMobCharacter*>&);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Expired();
public:
	void Expire();

	ABeeNest();
	void InitFromItem(int MaxNumBees, float SourceItemPower, float SpawnDelayRangeMin = 0.f, float SpawnDelayRangeMax = 0.5f);

	FOnExipired OnExpired;

	EntityType GetBeeType() const;
	virtual void SetLifeSpan(float) override;
	UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; };

	void DestryoAllBees();
	TArray<AMobCharacter*> GetBees() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnDespawn();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeeSpawn();

	UPROPERTY(EditDefaultsOnly)
	int BeesToSpawnAtAtime = 1;

	UPROPERTY(EditDefaultsOnly)
	int MaxNumBees = 3;

	UPROPERTY(EditDefaultsOnly)
	float AutoDestroyDelay = 2.f;

	//This will only be used if the nest is spawned from a different source than an artifact
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.1", ClampMax = "0.5"))
	float SpawnDelayRangeMin = 0.5f;
	//These will only be used if the nest is spawned from a different source than an artifact
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.5", ClampMax = "10"))
	float SpawnDelayRangeMax = 2.0f;
	UPROPERTY(EditDefaultsOnly)
	float SpawnRadius = 300.f;

	UPROPERTY(Instanced, VisibleDefaultsOnly, Category = "Dungeons")
	class UMobSummonHelper* SummonHelper;

	UPROPERTY(ReplicatedUsing = OnRep_Expired)
	bool Expired = false;

	TOptional<float> SourceItemPower;
};
