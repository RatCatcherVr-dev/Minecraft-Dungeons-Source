// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "RaidCaptainItemDropComponent.h"
#include "game/util/ActorQuery.h"
#include "game/Game.h"
#include "game/actor/item/StorableItem.h"


URaidCaptainItemDropComponent* URaidCaptainItemDropComponent::Create(UObject* outer)
{
	URaidCaptainItemDropComponent* object = NewObject<URaidCaptainItemDropComponent>(outer);

	return object;
}

TArray<FNetworkedItemDropData> URaidCaptainItemDropComponent::GatherItemDropData(const FItemDropSource & dropSource)
{
	TArray<FSerializableItemId> tokenTypes;
	TArray<AStorableItem*> storableItems = actorquery::getActors<AStorableItem>(GetWorld());
	game::Game* game = actorquery::getGame(GetWorld());
	if (game)
	{
		int DifficultyTokensInGame = storableItems.FilterByPredicate([](const AStorableItem* item) { return item->ItemData.GetItemId() == game::item::type::DifficultyToken.getId(); }).Num();
		if (!game->settings().difficulty.emergentDifficulty().IsAtMaxRaidDifficulty() && (game->settings().difficulty.emergentDifficulty().raidDifficulty + DifficultyTokensInGame) < MaxDifficultyTokensSpawned)
		{
			tokenTypes.Add(game::item::type::DifficultyToken.getId());
		}
	}

	int MysteryTokensInGame = storableItems.FilterByPredicate([](const AStorableItem* item) { return item->ItemData.GetItemId() == game::item::type::MysteryToken.getId(); }).Num();

	auto& WorldAffectors = affector::get(GetWorld());
	if (!game->settings().difficulty.emergentDifficulty().IsAtMaxMidGameAffectors() && game->settings().difficulty.emergentDifficulty().midGameAffectorsNum + MysteryTokensInGame < MaxMysteryTokensSpawned)
	{
		tokenTypes.Add(game::item::type::MysteryToken.getId());
	}
	return { FNetworkedItemDropData(tokenTypes.Num() > 0 ? EDropGeneratorCategory::Token : EDropGeneratorCategory::Gear, tokenTypes, SpawnType, MinCount, MaxCount, Probability)
};
}

void URaidCaptainItemDropComponent::OnMobKilled(const FOnAttributeChangeData& data)
{
	UItemDropComponent::OnMobKilled(data);
}
