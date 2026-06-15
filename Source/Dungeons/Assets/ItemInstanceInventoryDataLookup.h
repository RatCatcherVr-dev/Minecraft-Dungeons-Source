// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "game/item/SerializableItemId.h"
#include "game/item/stats/ItemStatsTypes.h"
#include "game/item/stats/ItemStatsUtil.h"
#include "ItemInstanceInventoryDataLookup.generated.h"

USTRUCT()
struct DUNGEONS_API FStatsWrapper {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FItemStatsEntry> Stats;
};

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEONS_API UItemInstanceInventoryDataLookup : public UDataAsset
{
	GENERATED_BODY()

public:
	USoundCue* GetInventoryDropSound(const FItemId&) const;
	const TArray<FItemStatsEntry> GetStats(const FItemId&) const;
	
protected:
	void PreSave(const ITargetPlatform* TargetPlatform) override;
	
	UPROPERTY(VisibleAnywhere)
	TMap<FSerializableItemId, USoundCue*> InventoryDropSounds;

	UPROPERTY(VisibleAnywhere)
	TMap<FSerializableItemId, FStatsWrapper> Stats;

};
