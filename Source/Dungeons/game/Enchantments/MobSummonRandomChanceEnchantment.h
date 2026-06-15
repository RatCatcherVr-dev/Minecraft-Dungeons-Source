// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "MobSummonRandomChanceEnchantment.generated.h"


class AMobCharacter;
/**
 * 
 */
UCLASS(Abstract)
class DUNGEONS_API UMobSummonRandomChanceEnchantment : public UEnchantment
{
	GENERATED_BODY()

public:
	UMobSummonRandomChanceEnchantment();

	void EndPlay(const EEndPlayReason::Type reason) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float SpawnDelaySeconds = 0.1f;

	UPROPERTY(Instanced, VisibleDefaultsOnly, Category = "Dungeons")
	class UMobSummonHelper* SummonHelper;

	//Chance to trigger at level 1;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseTriggerChance = 0.04f;

	//Chance to trigger increase per invested level.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TriggerChanceIncreasePerLevel = 0.03f;

	//Total number of concurrent bees allowed.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MaxNumMobs = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool DestroySummonsOnDestruction = true;

	FText CreateDescription() const override;

protected:
	void OnMobsSummoned(const TArray<AMobCharacter*>&);
	
	void AttemptMobSummon(game::mobspawn::TransformProvider, FRandomStream&);

	int NumMobsAllowedToSummon() const;

private:
	void SummonMob(game::mobspawn::TransformProvider);
	FTimerHandle SpawnHandle;
};
