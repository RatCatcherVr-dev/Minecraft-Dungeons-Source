// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "LootTableItemDropComponent.h"

TArray<FNetworkedItemDropData> ULootTableItemDropComponent::GatherItemDropData(const FItemDropSource & dropSource)
{
	return { FNetworkedItemDropData(IncludeUniques ? EDropGeneratorCategory::LootTableIncludeChildren : EDropGeneratorCategory::LootTable, LootTable, SpawnType, MinCount, MaxCount, Proability) };
}
