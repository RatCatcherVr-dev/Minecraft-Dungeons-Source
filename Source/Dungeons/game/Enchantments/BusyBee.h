// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "MobSummonRandomChanceEnchantment.h"
#include "BusyBee.generated.h"
/**
 * 
 */
UCLASS()
class DUNGEONS_API UBusyBee : public UMobSummonRandomChanceEnchantment
{
	GENERATED_BODY()

public:
	UBusyBee();

	void OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) override;
};
