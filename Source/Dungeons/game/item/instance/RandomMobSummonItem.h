// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/MobSummonItem.h"
#include "RandomMobSummonItem.generated.h"

USTRUCT()
struct DUNGEONS_API FMobChoice {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EntityType MobType;
	UPROPERTY(EditDefaultsOnly)
	float Weight = 1.f;

	float getWeight() const { return Weight; }
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API ARandomMobSummonItem : public AMobSummonItem
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<FMobChoice> MobsToChooseFrom;

	EntityType GetMobType() const override;
	float GetStats(EItemStats stat) const override;
};
