#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include <GameplayEffect.h>
#include "MobGroupBehaviorComponent.h"
#include "DolphinBehaviorComponent.generated.h"


UCLASS()
class DUNGEONS_API UDolphinsGraceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDolphinsGraceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDolphinBehaviorComponent : public UMobGroupBehaviorComponent
{
	GENERATED_BODY()

public:
	UDolphinBehaviorComponent();

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDolphinsGraceGameplayEffect> Effect;

	ALootActor* GetNearestTreasureChest();

	UFUNCTION()
	void DoLootActorQuery();

	bool IsNearChest();

	bool IsNearPlayer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dolphin Behavior")
	float DolphinGraceSpeedBoostAmount = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Dolphin Behavior")
	float DolphinGraceDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dolphin Behavior")
	float ChestMaximumDistance = 3000;
	ALootActor* NearestTresureChest;

	FTimerHandle LootActorQueryHandle;

	bool bIsNearChest = false;

	TArray< TWeakObjectPtr<APlayerCharacter> > NearbyPlayers;
};
