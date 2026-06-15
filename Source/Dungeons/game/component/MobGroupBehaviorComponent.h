#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../abilities/attributes/MovementAttributeSet.h"
#include "../abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include <GameplayEffect.h>
#include "MobGroupBehaviorComponent.generated.h"

UENUM(BlueprintType)
enum class EMobGroupAttackedState : uint8
{
	Normal,
	Flee,
	Agressive,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMobGroupBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMobGroupBehaviorComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mob Group Behavior")
	EMobGroupAttackedState MobGroupOnAttackedState;

	void SetState(EMobGroupAttackedState newState);
	EMobGroupAttackedState GetState();

	void OnMobWasAttacked(AActor* Attacker);

	AActor* GetLastAttacker();
	void SetLastAttacker(AActor* newLastAttacker);

	UFUNCTION()
	void ResetState();
	
	bool IsLastAttackerAlive();
	UFUNCTION()
	void CheckAttackerHealth();

	bool IsNearPlayer();
protected:
	virtual void BeginPlay() override;

	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mob Group Behavior")
	float MobsCheckRange = 3000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mob Group Behavior")
	float BackToNormalStateTime = 5.f;

	AActor* LastAttacker;

	EMobGroupAttackedState MobState = EMobGroupAttackedState::Normal;

	FTimerHandle LastAttackerAliveQueryHandle;
	FTimerHandle BackToNormalHandle;
};
