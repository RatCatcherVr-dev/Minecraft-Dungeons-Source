#pragma once

#include "ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "ArmorProperty.h"
#include <AbilitySystemInterface.h>
#include "Beekeeper.Generated.h"

UCLASS()
class DUNGEONS_API UBeekeeperGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBeekeeperGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UBeekeeper : public UArmorProperty
{
	GENERATED_BODY()

public:
	UBeekeeper();

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	FFormatOrderedArguments createFormattedDisplayValueString() const override;

	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	bool IsAllowedToSpawn() const;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Chance = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SpawnRadius = 300.0f;

	//Positive to set a cap of concurrent bees, < 1 for unlimited.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MaxNumberOfBees = 3;

	//Cooldown between spawns. <= 0 for no cooldown.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Cooldown = 0.f;

	float spawnTimestamp = 0;
	FActiveGameplayEffectHandle handle;

	UPROPERTY(Instanced, VisibleDefaultsOnly, Category = "Dungeons")
	class UMobSummonHelper* SummonHelper;
};
