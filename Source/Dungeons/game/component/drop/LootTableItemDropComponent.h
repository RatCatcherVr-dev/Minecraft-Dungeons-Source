// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/component/drop/ItemDropComponent.h"
#include "LootTableItemDropComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class DUNGEONS_API ULootTableItemDropComponent : public UItemDropComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	EDropSpawnType SpawnType = EDropSpawnType::Individual;

	UPROPERTY(EditDefaultsOnly)
	TArray<FSerializableItemId> LootTable;

	UPROPERTY(EditDefaultsOnly)
	float Proability = 1.f;

	UPROPERTY(EditDefaultsOnly)
	int32 MinCount = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxCount = 1;

	//If set to true, the table that is generated from will include any uniques based on the item types in the list.
	UPROPERTY(EditDefaultsOnly)
	bool IncludeUniques = false;

private:
	TArray<FNetworkedItemDropData> GatherItemDropData(const FItemDropSource& dropSource) override;
};
