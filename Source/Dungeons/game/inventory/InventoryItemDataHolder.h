// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Assets/ItemInstanceInventoryDataLookup.h"
#include "InventoryItemDataHolder.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UInventoryItemDataHolder : public UObject
{
	GENERATED_BODY()
public:
	void Initialize();

	USoundCue* GetInventoryDropSound(const FItemId&) const;
	const TArray<FItemStatsEntry> GetStats(const FItemId&) const;
	
private:

	void OnLookupLoaded(FAssetData asset);
	
	UPROPERTY(Transient)
	UItemInstanceInventoryDataLookup* DataLookup;
};
