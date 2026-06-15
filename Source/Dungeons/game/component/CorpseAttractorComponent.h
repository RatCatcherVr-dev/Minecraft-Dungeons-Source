#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "CorpseAttractorComponent.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UCorpseAttractorHealGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCorpseAttractorHealGameplayEffect();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UCorpseAttractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCorpseAttractorComponent();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type Reason) override;

	void TickCorpse(AMobCharacter* corpse, const FVector& targetLocation);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void OnActorDeath(AActor* Actor, AActor*, AActor*);

	UFUNCTION()
	void OnRep_CurrentlyActiveMobs();

	/** Currently munched mobs */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentlyActiveMobs, Transient)
	TArray<TWeakObjectPtr<AMobCharacter>> CurrentlyActiveMobs;

	/** Used to get delta towards replicated mobs */
	TSet<TWeakObjectPtr<AMobCharacter>> LocalMobs;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float radius = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float pickupDelay = .5f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float kickAmount = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float finalAmount = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float mobHealthReward = 50.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float playerHealthReward = 100.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float removeTimeSeconds = 1.f;
};
