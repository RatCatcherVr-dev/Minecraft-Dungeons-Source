#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "Array.h"
#include "Engine/World.h"
#include "OutputDevice.h"
#include "ItemTypeDefs.h"
#include "ItemUtil.h"
#include "game/GameSettings.h"
#include "game/GameTypes.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/ItemDropActor.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/SoulComponent.h"
#include "game/component/drop/EyeOfEnderDefines.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/Enchantments/generator/EnchantmentGenerator.h"
#include "game/Enchantments/generator/EnchantmentGeneratorConfigs.h"
#include "game/inventory/InventoryItem.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/item/stats/ItemStats.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/item/ItemType.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/util/ActorQuery.h"
#include "game/Game.h"
#include "game/actor/Dimmer.h"
#include "util/EnumUtil.h"
#include "game/level/environment/Environment.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/Conversion.h"
#include "game/objective/ObjectiveUtil.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/GameBP.h"
#include "lovika/world/level/terrain/Pathfinder.h"
#include "save/JsonCommon.h"

static void GrantDefaultItems(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			for(const auto* type : GetItemRegistry().GetValues()) {
				if (!type->isWorkInProgress() && !type->isConsumable() && !type->hasTag(ItemTag::HealthPotion) && !type->hasTag(ItemTag::Arrow) && !type->isInstant()) {
					itemStash->ClientAddItem(itemgen::generate(*type));
				}
			}
		}
	}
}

static bool ItemTypeAllowedOnAnyMissionDifficulty(const ItemType& type, EGameDifficulty difficulty) {
	for (const auto* missiondef : missions::getAllCurrentlyEnabled()) {
		if (type.isAllowedOnMissionDifficulty(missiondef->level(), difficulty)) {
			return true;
		}
	}
	return false;
}

static void GrantDifficultyItems(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() <= 0) {
		out.Log(TEXT("First argument must a number representing a difficulty"));
		return;
	}
	auto diff = EnumValueFromString(EGameDifficulty, "Difficulty_" + commands[0]);
	if(!diff.IsSet()){
		out.Log(TEXT("First argument not a difficulty, please specify 1-3"));
		return;
	}
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			for(const auto* itemType : GetItemRegistry().GetValues()) {
				const auto& type = *itemType;
				if (!type.isWorkInProgress() && !type.isConsumable() && ItemTypeAllowedOnAnyMissionDifficulty(type, diff.GetValue()) && !type.hasTag(ItemTag::HealthPotion) && !type.hasTag(ItemTag::Arrow) && !type.isInstant()) {
					itemStash->ClientAddItem(itemgen::generate(type));
				}
			}
		}
	}
}

static void GrantAllEnchantments(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {

			const auto& enchantmentTypes = game::enchantment::type::getAvailableEnchantments();
			EEnchantmentCategory lastCategory = EEnchantmentCategory::Unset;
			bool lastIsSoulUser = false;			
			int addedEnchantments = 0;

			TArray<FEnchantmentData> enchantments;
			TOptional<FItemId> currentItemType;
			
			auto popItem = [&]() {
				if (currentItemType && addedEnchantments > 0) {

					const auto& itemType = GetItemRegistry().Get(currentItemType.GetValue());

					TArray<FEnchantmentData> copiedEnchantments = TArray<FEnchantmentData>(enchantments);
					auto itemData = game::item::generator::generate(itemType.getId(), 1.0f, std::move(copiedEnchantments));

					itemStash->ClientAddItem(itemData);
				}
			};

			for (auto enchantmentType : enchantmentTypes) {
				if (enchantmentType.isDisabledForPlayers()) {
					continue;
				}

				const auto nextIsSoulUser = enchantmentType.hasTag(EEnchantmentTag::SoulUsage);
				const auto nextCategory = enchantmentType.getEnchantmentCategory();
				if (lastCategory != nextCategory || addedEnchantments >= 3 || lastIsSoulUser != nextIsSoulUser) {

					popItem();

					lastCategory = nextCategory;
					lastIsSoulUser = nextIsSoulUser;
					addedEnchantments = 0;
					currentItemType.Reset();
					enchantments.Reset();
				}

				if (!currentItemType) {
					TArray<FItemId> ItemIds;
					for (auto& item : GetItemRegistry().GetValues()) {
						ItemIds.Add(item->getId());
					}					

					const auto& matchingItemIds = ItemIds.FilterByPredicate([nextIsSoulUser,nextCategory](const FItemId& checkedItemId) {
						const auto& checkedItemType = GetItemRegistry().Get(checkedItemId);
						const EEnchantmentCategory itemEnchantmentCategory = game::enchantment::type::EnchantmentCategoryFromItemTag(checkedItemType.getTag());
						const bool isSoulUsing = checkedItemType.isSoulGatherItem();
						return itemEnchantmentCategory == nextCategory && (!nextIsSoulUser || isSoulUsing) && !checkedItemType.isWorkInProgress() && !checkedItemType.isUnique();
					});
					if (matchingItemIds.Num() > 0) {
						currentItemType = matchingItemIds[FMath::RandRange(0, matchingItemIds.Num()-1)];
					}
				}

				enchantments.Add(FEnchantmentData{ enchantmentType.getEnchantmentTypeID(), 1 });
				for (int i = 1; i < game::enchantment::RowSize; i++) {
					//Fill the empty rows
					enchantments.Add(FEnchantmentData{ EEnchantmentTypeID::Unset, 0 });
				}

				addedEnchantments++;
			}

			popItem();
		}
	}
}



static void PrintAllItemsStats(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			
			const auto meleeScores = game::item::stats::getItemStatsTotals([&](const ItemType& t) { return t.getTag() == ItemTag::MeleeWeapon; }, {
				EItemStats::AverageDamage,
				EItemStats::AttackSpeed,
				EItemStats::AverageArea,
				EItemStats::AreaDamage,
				EItemStats::DamagePerSec,
				EItemStats::PushbackStr,
				EItemStats::HitSnappiness
			});

			const auto rangedScores = game::item::stats::getItemStatsTotals([&](const ItemType& t) { return t.getTag() == ItemTag::RangedWeapon; }, {
				EItemStats::AverageDamage,
				EItemStats::AttackSpeed,
				EItemStats::QuiverAmmo,
				EItemStats::DamagePerQuiver,
				EItemStats::AreaDamage,
				EItemStats::ArrowSpeed,
				EItemStats::DamagePerSec
			});


			const auto itemScores = game::item::stats::getItemStatsTotals([&](const ItemType& t) { return t.getTag() == ItemTag::Item; }, {
				EItemStats::RechargeSpeed,				
			});

			out.Logf(TEXT("Items:\n"));
			itemScores.PrintAllTo(out);

			out.Logf(TEXT("Melee:\n"));
			meleeScores.PrintAllTo(out);

			out.Logf(TEXT("Ranged:\n"));
			rangedScores.PrintAllTo(out);
		}
	}
}

static void PrintPowerfulEnchantmentCounts(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto& types = GetItemRegistry().GetValues();

	const auto printPowerfuls = [&](const TArray<const ItemType*>& types) {
		for (auto& type : types) {
			const auto powerful = game::enchantment::generator::getAvailableEnchantmentsForType(type->getId()).powerful;

			const auto maybeEnchantmentNames = powerful.Num() <= 4 ?
				" (" + FString::JoinBy(powerful, TEXT(", "), RETLAMBDA(it->getDisplayName().ToString())) + ')' :
				"";

			UE_LOG(LogDungeons, Log, TEXT(" %2d: %11s %s%s"),
				powerful.Num(),
				*FString::ChrN(powerful.Num(), '*'),
				*type->getId().GetBackingType().ToString(),
				*maybeEnchantmentNames);
		}
	};

	UE_LOG(LogDungeons, Log, TEXT("Ranged:"));
	printPowerfuls(algo::copy_if(types, RETLAMBDA(it->getTag() == ItemTag::RangedWeapon)));

	UE_LOG(LogDungeons, Log, TEXT("Melee:"));
	printPowerfuls(algo::copy_if(types, RETLAMBDA(it->getTag() == ItemTag::MeleeWeapon)));

	UE_LOG(LogDungeons, Log, TEXT("Armor:"));
	printPowerfuls(algo::copy_if(types, RETLAMBDA(it->getTag() == ItemTag::Armor)));
}

static void ListItemsConsoleCommand(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		auto serializer = playerOwner->GetCharacterSerializeComponent();
		if (!serializer) {
			return;
		}

		auto& items = serializer->GetItems();

		for (int i = 0; i < items.size(); ++i) {
			const auto& item = items[i];
			const auto& itemType = GetItemRegistry().Get(item.type);
			out.Logf(TEXT("%i: %s, %1.2f"), i, *itemType.getName(), item.power);
		}
	}
}

static void ListAllItemNamesConsoleCommand(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	out.Log("ItemType ids:");
	for (const auto& val : GetItemRegistry().GetValues()) {
		out.Logf(TEXT("%s :: %s"), *val->getName(), *val->getId().GetBackingType().ToString());
	}

	out.Log(" -- done -- ");

}

static void Drop(APlayerCharacter* player, AItemDropActor* itemDropActor, int index) {
	if (!GetItemRegistry().GetValues().IsValidIndex(index)) return;
	
	itemDropActor->DropItems(player->GetActorLocation(), GetItemRegistry().GetValues()[index]->getId(), 1, player);
	FTimerHandle handle;
	player->GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateStatic(&Drop, player, itemDropActor, index + 1), 0.1, false );
}

static void DropAll(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (playerOwner->HasAuthority()) {
			if (AItemDropActor* itemDropActor = actorquery::getFirstActor<AItemDropActor>(world)) {
				Drop(playerOwner, itemDropActor, 0);
			}
			else {
				out.Log("unable to drop items, no AItemDropActor found.");
			}

		}
		else {
			out.Log("Only server can spawn consumables.");
		}
	}
}


static void DropItemRandom(const TArray<FString>& commands, UWorld* world, FOutputDevice& out)
{	
	int amount = 1;		
	EItemRarityChanceCategory source = EItemRarityChanceCategory::Mob;
	EDropCategory category = EDropCategory::Gear;
	itemgen::ItemSource itemSource = itemgen::ItemSource::Drop;	

	int index = 0;
	while (commands.Num() > index) {
		const auto& command = commands[index++];
		if (command.IsNumeric()) {
			amount = FCString::Atoi(*command);			
		} else if (auto rarityChance = EnumValueFromString(EItemRarityChanceCategory, command)) {
			source = rarityChance.GetValue();
		} else if (auto dropCategory = EnumValueFromString(EDropCategory, command)) {
			category = dropCategory.GetValue();
		} else if (command.Equals("gilded", ESearchCase::IgnoreCase) || command.Equals("netherite", ESearchCase::IgnoreCase)) {
			itemSource = itemgen::ItemSource::NetheriteMerchant;
		}		
	}

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		const game::item::drop::DropGenerationInput rewardData(playerOwner,
			game::item::drop::getPredicateForItemCategory(category), FRareItemChance::GetChanceFromCategory(source), itemSource);

		auto generatedItems = generateDroppedItems(world, rewardData, amount);

		AItemEmitterActor::SpawnSimpleItemEmitter(world, generatedItems, playerOwner->GetActorLocation(), playerOwner, true, false, false);
	}
	return;
}


static void DropItem(const TArray<FString>& commands, UWorld* world, FOutputDevice& out)
{
	float power = 1.f; // default for here
	int amount = 1;
	FVector offset = FVector::ZeroVector;
	bool isLocal = false;

	if (commands.Num() <= 0) {
		out.Log(TEXT("First argument must Name of Item."));
		return;
	} if (commands.Num() > 1) {
		if (commands[0].IsNumeric() || !commands[1].IsNumeric()) {
			out.Log(TEXT("First arg must be name of item and second argument must be a number (if present)"));
			return;
		}
		else {
			amount = FCString::Atoi(*commands[1]);
		}

	}

	if (commands.Num() > 2 && commands[2].IsNumeric())
		offset.X = FCString::Atof (*commands[2]);

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		auto type = GetItemRegistry().Request(*commands[0]);
		if (!type) {
			out.Logf(TEXT("No item found with name %s"), *commands[0]);
			return;
		}

		UE_LOG(LogDungeons, Log, TEXT("dropping %d x item: %s"), amount, *type.GetValue().GetBackingType().ToString());
		FVector pos = playerOwner->GetActorLocation();
		UE_LOG(LogDungeons, Log, TEXT("location %f, %f, %f offset: %f, %f, %f"), pos.X, pos.Y, pos.Z, (pos + offset).X
			, (pos + offset).Y
			, (pos + offset).Z);

		if (const auto ItemDropActor = actorquery::getFirstActor<AItemDropActor>(world)) {
			ItemDropActor->DropItems(playerOwner->GetActorLocation() + offset, type.GetValue(), amount, playerOwner);
		}
		else {
			out.Log("unable to drop items, no AItemDropActor found.");
		}
	}
}

static void CreateItem(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	float power = 1.f;
	bool isLocal = true;
	int index = 1;
	auto rarity = EItemRarity::Common;
	bool gilded = false;
	bool modified = false;

	if (commands.Num() <= 0) {
		out.Log(TEXT("First argument must Name of Item."));
		return;
	} 
	
	while (commands.Num() > index) {
		const auto& command = commands[index++];
		if (command.IsNumeric()) {
			power = FCString::Atof(*command);
			FMath::Clamp(power, 1.f, 3.f);
		}
		else if (command.Equals("server", ESearchCase::IgnoreCase)) {
			isLocal = false;
		}
		else if (command.Equals("gilded", ESearchCase::IgnoreCase) || command.Equals("netherite", ESearchCase::IgnoreCase)) {
			gilded = true;
		}
		else if (command.Equals("modified", ESearchCase::IgnoreCase)) {
			modified = true;
		}
		else if (auto enumRarity = EnumValueFromString(EItemRarity, command)) {
			rarity = enumRarity.GetValue();
		}
	}

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			auto itemId = GetItemRegistry().Request(*commands[0]);
			if (!itemId) {
				out.Logf(TEXT("No item found with name %s"), *commands[0]);
				return;
			}
			const auto type = itemId.GetValue();
			const auto& itemType = GetItemRegistry().Get(type);
			bool isUnique = itemType.isUnique();

			if (itemType.getFixedRarity().IsSet()) {
				rarity = itemType.getFixedRarity().GetValue();
			}
			
			bool boss = false;
			bool unique = rarity == EItemRarity::Unique;

			auto itemData = itemgen::generate({}, itemgen::looters::FromConsolePlayerCharacter(*playerOwner), itemgen::configs::ConsoleItem(type, power, boss, unique, gilded));
			if(!itemData) {
				out.Logf(TEXT("Item data from generator was unset"));
				return;
			}
			
			itemData.GetValue().bIsModified = modified;
			itemData.GetValue().Rarity = rarity;

			if (auto storableClass = itemType.getStorableClass()) {
							
				TArray<FInventoryItemData> sourceItemList = {itemData.GetValue() };
				AItemEmitterActor::SpawnSimpleItemEmitter(world, sourceItemList, playerOwner->GetActorLocation(), playerOwner, true, false, false, FItemEmitterActorCompleteDelegate::CreateLambda([playerOwner, isLocal](TArray< AStorableItem* > & SpawnedItems) {

					for (auto item : SpawnedItems)
					{
						if (playerOwner->HasAuthority()) {
							item->SetReplicates(!isLocal);
						}
					}
				}));

			} else {
				itemStash->ClientAddItem(itemData.GetValue());
			}
		}
	}
}

// From BP_EndPortal.uasset links in BP schema:
// EEyeOfEnder from EyeOfEnderDropComponent.h
// %%%  EEyeOfEnder %%%%%%%%%%%%%%%%%% Tag name (str) %%%%%%%% Mob BP %%%% MobType %% 

static void CreateEyeOfEnder(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 0) {
		out.Log(TEXT("First argument must cast to EEyeOfEnder."));
		out.Log(TEXT("Use any of deepwood_brook, old_town_hall, sunken_watchtower, the_undercroft, the_trial_of_the_nameless, highblock_hideaway"));
		return;
	}

	TOptional<EEyeOfEnderType> eyeOfEnderType = EnumValueFromString(EEyeOfEnderType, args[0]);
	if (!eyeOfEnderType.IsSet())
	{
		out.Logf(TEXT("Error: No such type for EEyeOfEnder: %s"), *args[0]);
		return;
	}

	FInventoryItemData inventoryItemData = FInventoryItemData(game::item::type::EyeOfEnder.getId());
	inventoryItemData.SubItemID = static_cast<int>(eyeOfEnderType.GetValue());
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, args)) {
		AStorableItem* eyeOfEnderItem = game::item::util::spawnStorableItem(*world, playerOwner->GetActorLocation(), inventoryItemData);
		eyeOfEnderItem->SetReplicates(true);
		eyeOfEnderItem->ApplyDropEffect(playerOwner);
	}
}

static void CreateAllEyesOfEnder(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (int i = 0; i < TOTAL_EYES_OF_ENDER; i++) {
		TOptional<EEyeOfEnderType> eyeOfEnderType = static_cast<EEyeOfEnderType>(i + 1);
		if (!eyeOfEnderType.IsSet())
		{
			out.Logf(TEXT("Error: No such type for EEyeOfEnder: %s"), *StaticEnum<EEyeOfEnderType>()->GetValueAsString(static_cast<EEyeOfEnderType>(i + 1)));
			continue;
		}

		FInventoryItemData inventoryItemData = FInventoryItemData(game::item::type::EyeOfEnder.getId());
		inventoryItemData.SubItemID = static_cast<int>(eyeOfEnderType.GetValue());
		if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, args)) {
			AStorableItem* eyeOfEnderItem = game::item::util::spawnStorableItem(*world, playerOwner->GetActorLocation(), inventoryItemData);
			eyeOfEnderItem->SetReplicates(true);
			eyeOfEnderItem->ApplyDropEffect(playerOwner);
		}
	}
}

static void DimLights(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() <= 0 || !commands[0].IsNumeric()) {
		out.Log(TEXT("First argument must be a number"));
		return;
	}

	const auto level = FCString::Atoi(*commands[0]);

	actorquery::getGame(world)->mEnvironment->dimLights(level, FNightColor());
}

static void EnterNight(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto dimmer = actorquery::getFirstActor<ADimmer>(world)) {
		dimmer->EnterNight();
	}
}

static void ExitNight(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto dimmer = actorquery::getFirstActor<ADimmer>(world)) {
		dimmer->ExitNight();
	}
}

static void RingAtObjective(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	actorquery::getFirstActor<AGameBP>(world)->RingAtObjective();
}

static void ListEnchantmentsForItemConsoleCommand(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!commands.Num()) {
		out.Log(TEXT("Not enough arguments passed to List Enchantments."));
		return;
	}

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (const auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			if (const auto item = itemStash->GetInventoryItem(FCString::Atoi(*commands[0])))
			{
				out.Logf(TEXT(" --- %s --- \n\n"), *item->Item.GetItemId().ToString());
				for (const auto& enchantment : item->Item.Enchantments)
				{
					const auto& enchantmentType = game::enchantment::type::getEnchantmentType(enchantment.TypeID);
					out.Logf(TEXT(": %s %i"), /*i, */*enchantmentType.getDisplayName().ToString(), enchantment.Level);
				}
			}
		}
	}

}

static void ResetEnchantmentsForItemConsoleCommand(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!commands.Num()) {
		out.Log(TEXT("Need arg 0 = ItemInventoryIndex."));
		return;
	}
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>()) {
			if (itemStash->ResetItemEnchantments(FCString::Atoi(*commands[0])))
				itemStash->RefreshCachedPoints(); //?
		}
	}
}

static void RerollEnchantmentsForItemConsoleCommand(const TArray<FString>& commands, UWorld* world, FOutputDevice& out)  {
	if (!commands.Num()) {
		out.Log(TEXT("Not enough arguments passed to Reroll: index-in-inventory [difficulty-fraction=0.5f]"));
		return;
	}
	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (auto itemStash = playerOwner->FindComponentByClass<UItemStashComponent>())  {
			const int itemIndex = FCString::Atoi(*commands[0]);
			if (const UInventoryItem* item = itemStash->GetInventoryItem(itemIndex)) {
				const auto& itemType = GetItemRegistry().Get(item->Item.GetItemId());

				if (game::enchantment::type::EnchantmentCategoryFromItemTag(itemType.getTag()) == EEnchantmentCategory::Unset)  {
					out.Logf(TEXT("%s cannot be enchanted."), *itemType.getName());
					return;
				}
				const float difficultyFraction = commands.Num() >= 2 ? FCString::Atof(*commands[1]) : 0.5f;
				const auto enchantments = enchgen::generate(itemType.getId(), enchgen::configs::FromDifficultyFraction(difficultyFraction));
				itemStash->SetItemEnchantments(itemIndex, enchantments);
				itemStash->RefreshCachedPoints();
			}
		}
	}
}

static void AddSouls(const TArray<FString>& commands, UWorld* world, FOutputDevice& out)
{
	if (!commands.Num()) {
		out.Log(TEXT("Not enough arguments passed to soul amount."));
		return;
	}
	int soulAmount = FCString::Atoi(*commands[0]);

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		if (const auto soulComponent = playerOwner->FindComponentByClass<USoulComponent>()) {
			soulComponent->ServerAddSouls(soulAmount);
		}
	}
}

static void DoSpawnChest(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	const auto index = [&] {
		if (commands.Num() > 0) {
			if (commands[0].IsNumeric()) {
				return FMath::Clamp(FCString::Atoi(*commands[0]), 0, 4);
			}

			out.Log(TEXT("First arg must be chest type index (0, 1, 2)"));
		}

		return 0;
	}();

	static const std::array<FString, 5> paths = {
		FString { "/Game/Decor/Prefabs/RewardChest/BP_WoodenChest" },
		FString { "/Game/Decor/Prefabs/RewardChest/BP_FancyChest" },
		FString { "/Game/Decor/Prefabs/RewardChest/BP_DeluxeChest" },
		FString { "/Game/Content_DLC4/Decor/Prefab/Functional/GoldChests/BP_GoldChest_Rare" },
		FString { "/Game/Content_DLC4/Decor/Prefab/Functional/GoldChests/BP_GoldChest_Small" },
	};

	const auto count = [&] {
		if (commands.Num() > 1) {
			if (commands[1].IsNumeric()) {
				return FCString::Atoi(*commands[1]);
			}

			out.Log(TEXT("First arg must be chest type index (0, 1, 2) and second argument must be the number of chests (if present)"));
		}

		return 1;
	}();

	if (APlayerCharacter* playerOwner = GetPlayerCharacter(world, commands)) {
		const auto forward = playerOwner->GetActorForwardVector() * FMath::Max(300.f, count * 45.f);

		const float angleIncrement = 360.f / count;
		for (int i = 0; i < count; i++) {
			game::objective::placeObject(
				*world,
				paths[index],
				conversion::ueToPos(playerOwner->GetActorLocation() + forward.RotateAngleAxis(angleIncrement * i, FVector::UpVector)),
				angleIncrement * i
			);
		}
	}
}

static void DoSetItemDropPowerBonus(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Items.DropPowerBonus", world, commands)) {
		return;
	}
	auto bonus = ArgAsFloat(commands, 0);
	if (!bonus) {
		out.Log(TEXT("Incorrect power bonus argument (should be a numeric value)"));
		return;
	}
	if (auto* game = actorquery::getGame(world)) {
		// I'm fine with const casting here, since it's "just" a console command
		const_cast<game::Settings&>(game->settings()).difficultyStats.SetExtraDropItemPowerBonus(bonus.GetValue());
	}
}

static void DoSetItemRewardPowerBonus(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Items.RewardPowerBonus", world, commands)) {
		return;
	}
	auto bonus = ArgAsFloat(commands, 0);
	if (!bonus) {
		out.Log(TEXT("Incorrect power bonus argument (should be a numeric value)"));
		return;
	}
	if (auto* game = actorquery::getGame(world)) {
		// I'm fine with const casting here, since it's "just" a console command
		const_cast<game::Settings&>(game->settings()).difficultyStats.SetExtraRewardItemPowerBonus(bonus.GetValue());
	}
}

static FAutoConsoleCommand ListItemsCommand(TEXT("Dungeons.Items.List")
	, TEXT("List all your items. Useful for finding out which index an item is on.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ListItemsConsoleCommand)
	, ECVF_Cheat);

static FAutoConsoleCommand ListAvailableItemsCommand(TEXT("Dungeons.Items.AllItemNames")
	, TEXT("List all item names, to then be able to query a specific item.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ListAllItemNamesConsoleCommand)
	, ECVF_Cheat);

static FAutoConsoleCommand ListItemEnchantsCommand(TEXT("Dungeons.Items.Enchantment.List")
	, TEXT("List all the enchantments on an item (itemIndex)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ListEnchantmentsForItemConsoleCommand)
	, ECVF_Cheat);

static FAutoConsoleCommand RerollItemEnchantsCommand(TEXT("Dungeons.Items.Enchantment.Reroll")
	, TEXT("Reroll available enchantments on an item (itemIndex)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&RerollEnchantmentsForItemConsoleCommand)
	, ECVF_Cheat);

static FAutoConsoleCommand ResetItemEnchantsCommand(TEXT("Dungeons.Items.Enchantment.Reset")
	, TEXT("Reset the enchantment points on a specific item (itemIndex) Warning: does not update inventory UI")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ResetEnchantmentsForItemConsoleCommand)
	, ECVF_Cheat);

static FAutoConsoleCommand CreateItemCommand(TEXT("Dungeons.Items.Create")
	, TEXT("Creates a new item and adds it to your inventory.")
	TEXT("\nUsage: Dungeons.Items.Create \"ItemId\" <ItemPower> \"IsServer\" \"Rarity\" \"Gilded\" \"Modified\"")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CreateItem)
	, ECVF_Cheat);

static FAutoConsoleCommand CreateEyeOfEnderCommand(TEXT("Dungeons.Items.CreateEyeOfEnder")
	, TEXT("Drops an eye of ender of the supplied 'type' for thestronghold progression.")
	TEXT("\nUsage: Dungeons.Items.CreateEyeOfEnder \"EEyeOfEnderType\" ")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CreateEyeOfEnder)
	, ECVF_Cheat);

static FAutoConsoleCommand CreateAllEyesOfEnderCommand(TEXT("Dungeons.Items.CreateAllEyesOfEnder")
	, TEXT("Drops all eyes of ender for thestronghold progression.")
	TEXT("\nUsage: Dungeons.Items.CreateAllEyesOfEnder")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&CreateAllEyesOfEnder)
	, ECVF_Cheat);

static FAutoConsoleCommand DimLightsCommand(TEXT("Dungeons.Lights.Dim")
	, TEXT("Sets the light level")
	TEXT("\nUsage: Dungeons.Lights.Dim level")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DimLights)
	, ECVF_Cheat);

static FAutoConsoleCommand EnterNightCommand(TEXT("Dungeons.Lights.EnterNight")
	, TEXT("Enter night")
	TEXT("\nUsage: Dungeons.Lights.EnterNight")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&EnterNight)
	, ECVF_Cheat);

static FAutoConsoleCommand ExitNightCommand(TEXT("Dungeons.Lights.ExitNight")
	, TEXT("Sets the light level")
	TEXT("\nUsage: Dungeons.Lights.ExitNight")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ExitNight)
	, ECVF_Cheat);

static FAutoConsoleCommand RingAtObjectiveCommand(TEXT("Dungeons.Objective.Ring")
	, TEXT("Rings a bell at the closest objective or door")
	TEXT("\nUsage: Dungeons.Objective.Ring")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&RingAtObjective)
	, ECVF_Cheat);

static FAutoConsoleCommand DropItemCommand(TEXT("Dungeons.Items.DropItem")
	, TEXT("Calls ItemDropActor to spawn a specific item on the ground")
	TEXT("\nUsage: Dungeons.Items.DropItem \"ItemId\"")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DropItem)
	, ECVF_Cheat);

static FAutoConsoleCommand DropItemRandomCommand(TEXT("Dungeons.Items.DropItemRandom")
	, TEXT("Generated and drops a number of items based on the current level and input drop source category and drop category")
	TEXT("\nUsage: Dungeons.Items.DropItemRandom [amount=1] [EItemRarityChanceCategory=Mob] [EDropCategory=Gear]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DropItemRandom)
	, ECVF_Cheat);



static FAutoConsoleCommand DropAllCommand(TEXT("Dungeons.Items.DropAll")
	, TEXT("Calls ItemDropActor to spawn ALL items on the ground.")
	TEXT("\nUsage: Dungeons.Items.DropAll")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DropAll)
	, ECVF_Cheat);

static FAutoConsoleCommand GrantAllItems(TEXT("Dungeons.Items.CreateAllItems")
	, TEXT("Grants all items with power 1.0f and no enchantments.")
	TEXT("\nUsage: Dungeons.Items.CreateAllItems")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GrantDefaultItems)
	, ECVF_Cheat);


static FAutoConsoleCommand GrantAllItemsInDifficulty(TEXT("Dungeons.Items.CreateAllItemsInDifficulty")
	, TEXT("Grants all items which are droppable in the selected difficulty")
	TEXT("\nUsage: Dungeons.Items.CreateAllItemsInDifficulty [difficulty 1-3]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GrantDifficultyItems)
	, ECVF_Cheat);


static FAutoConsoleCommand GrantAllItemEnchantments(TEXT("Dungeons.Items.CreateAllItemEnchantments")
	, TEXT("Grants random items going through all enchantments")
	TEXT("\nUsage: Dungeons.Items.CreateAllItemEnchantments")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GrantAllEnchantments)
	, ECVF_Cheat);

static FAutoConsoleCommand AddSoulsCommand(TEXT("Dungeons.Items.AddSouls")
	, TEXT("Adds specified amount of souls to player's soul component")
	TEXT("\nUsage: Dungeons.Items.AddSouls")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&AddSouls)
	, ECVF_Cheat);

static FAutoConsoleCommand PrintAllStats(TEXT("Dungeons.Items.PrintAllStats")
	, TEXT("Prints all item estimated score values to output")
	TEXT("\nUsage: Dungeons.Items.PrintAllStats")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintAllItemsStats)
	, ECVF_Cheat);

static FAutoConsoleCommand PrintPowerfulEnchantmentCountsCommand(TEXT("Dungeons.Items.PrintPowerfulEnchantmentCounts")
	, TEXT("Prints out the number of powerful enchantments for each gear")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintPowerfulEnchantmentCounts)
	, ECVF_Cheat);

static FAutoConsoleCommand SpawnChest(TEXT("Dungeons.Items.SpawnChest")
	, TEXT("Spawns chests around the player")
	TEXT("\nUsage: Dungeons.Items.SpawnChest 0|1|2 count")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSpawnChest)
	, ECVF_Cheat);

static FAutoConsoleCommand ItemDropPowerBonus(TEXT("Dungeons.Items.DropPowerBonus")
	, TEXT("Sets the item drop power bonus (defaults to 0 == no power bonus)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetItemDropPowerBonus)
	, ECVF_Cheat);

static FAutoConsoleCommand ItemRewardPowerBonus(TEXT("Dungeons.Items.RewardPowerBonus")
	, TEXT("Sets the item reward power bonus (defaults to 0 == no power bonus)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetItemRewardPowerBonus)
	, ECVF_Cheat);


TAutoConsoleVariable<int32> CVarDebugDrawItems(
	TEXT("Dungeons.DebugDraw.Item"),
	0,
	TEXT("Enables debug drawing of items.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarUnlimitedEnchantment(
	TEXT("Dungeons.Enchantments.Unlimited"),
	0,
	TEXT("Unlimited enchantment points. Go nuts.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarEnableOpenInventoryWithKey(
	TEXT("Dungeons.Items.EnableOpenInventoryWithKey"),
	1,
	TEXT("Enables opening the inventory with the 'I' key.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);