#include "Dungeons.h"
#include "LootUnlockerComponent.h"
#include "ClientEventHub.h"
#include "DungeonsGameState.h"
#include "game/actor/character/loot/LobbyChest.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/KillTrackerComponent.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "lovika/LovikaLevelActor.h"
#include "util/telemetry/Assert.h"
#include "UserWidget.h"
#include "game/item/drop/ItemDropGenerator.h"

ULootUnlockerComponent::ULootUnlockerComponent() {
}

void ULootUnlockerComponent::BeginPlay() {
	Super::BeginPlay();
	// seamless travel : clear any previous bindings
	OnUnlockRequestedRandomGear.Clear();
	OnUnlockRequestedRandomItem.Clear();
	OnUnlockRequestedStatic.Clear();
	OnUnlockRequestedEmeralds.Clear();
}

ABasePlayerController* ULootUnlockerComponent::GetPlayerController() const {
	return Cast<ABasePlayerController>(GetOwner());
}

APlayerCharacter* ULootUnlockerComponent::GetPlayerCharacter() const {
	if (const auto* PlayerController = GetPlayerController()) {
		return Cast<APlayerCharacter>(PlayerController->GetPawn());
	}
	return nullptr;
}

int32 ULootUnlockerComponent::GetNumCompletedMissionsAtDifficulty(EGameDifficulty difficulty) const {
	if (const APlayerCharacter* character = GetPlayerCharacter()) {
		if (auto* progress = character->FindComponentByClass<UMissionProgressComponent>()) {
			return progress->GetNumCompletedMissionsAtDifficulty(difficulty);
		}
	}
	return 0;
}

// Can be removed if LobbyBP is refactored to use MissionProgressComponent of PlayerController directly.
bool ULootUnlockerComponent::GetHasCompletedMissionAtDifficulty(EGameDifficulty difficulty, ELevelNames levelName) const {
	if (const APlayerCharacter* character = GetPlayerCharacter()) {
		if (auto* progress = character->FindComponentByClass<UMissionProgressComponent>()) {
			return progress->GetHasCompletedMissionAtDifficulty(difficulty, levelName);
		}
	}
	return false;
}

// Can be removed if LobbyBP is refactored to use KillTrackerComponent of PlayerController directly.
int32 ULootUnlockerComponent::GetNumMobKills(const FString& MobEntityTypeName) const {
	if (auto* character = GetPlayerCharacter()) {
		if (auto* killTracker = character->FindComponentByClass<UKillTrackerComponent>()) {
			return killTracker->GetNumKillsInCategory(MobEntityTypeName);
		}
	}
	return 0;
}

void ULootUnlockerComponent::GiveLoot() const {
	if (const auto controller = GetPlayerController()) {
		const auto loot = GetLoot();
		if (auto stash = controller->GetItemStashComponent()) {
			if (loot.item) {
				// add to player inventory + Broadcast to UI
				stash->ClientGiftItem(loot.item.GetValue());
			}
		}

		if (loot.numEmeralds > 0) {
			if (auto wallet = controller->GetWalletComponent()) {
				wallet->ClientAdd(game::item::type::Emerald.getId(), loot.numEmeralds);
			}
		}
	}
}

ULootUnlockerComponent::Loot ULootUnlockerComponent::GetLoot() const {
	Loot loot;
	if (const auto chest = mChest.Get()) {
		switch (chest->ChestLootType) {
			case ELobbyChestLootType::RandomItem:
				loot.item = GetRandomLoot(itemgen::predicates::ItemVendor());
				break;

			case ELobbyChestLootType::RandomGear:
				loot.item = GetRandomLoot(itemgen::predicates::GearVendor());
				break;

			case ELobbyChestLootType::Static: 
				loot.item = GenerateStaticLoot(*chest);
				break;

			case ELobbyChestLootType::Emeralds:
				loot.numEmeralds = chest->EmeraldsReward;
				break;

			default:
				checkf(false, TEXT("Called GetLoot with unknown chestType!"));
		}
	}
	return loot;
}

TOptional<FInventoryItemData> ULootUnlockerComponent::GetRandomLoot(const itemgen::Pred& predicate) const {
	ensure(GetPlayerController());

	if (const auto controller = GetPlayerController()) {
		if (const auto chest = mChest.Get()) {
			const auto targetPlayer = controller->GetControlledPlayerCharacter();
			const auto itemRarityChance = FRareItemChance::GetAdjustedChanceFromCategory(ALobbyChest::GetItemRarityChanceCategory(), targetPlayer);
			const game::item::drop::DropGenerationInput rewardData(targetPlayer, predicate, itemRarityChance, itemgen::ItemSource::LootUnlocker);
			return generateDroppedItem(GetOwner()->GetWorld(), rewardData);
		}
	}
	return {};
}

bool ULootUnlockerComponent::TryConsumeEmeralds() {
	const auto chest = mChest.Get();
	if (!ensure(chest)) {
		return false;
	}

	const auto amount = chest->GetEmeraldUnlockCost(GetPlayerCharacter());
	if (amount <= 0) {
		return true;
	}

	if (auto* controller = GetPlayerController()) {
		if (auto* wallet = controller->GetWalletComponent()) {
			if (wallet->GetEmeraldBalance() >= amount) {
				wallet->Deduct(game::item::type::Emerald.getId(), amount);
				return true;
			}
		}
	}

	return false;
}

TOptional<FInventoryItemData> ULootUnlockerComponent::GenerateStaticLoot(const ALobbyChest& chest) const {
	const auto config = chest.StaticRewardItemPower >= 0 ?
		itemgen::configs::Item(chest.StaticRewardItemId, chest.StaticRewardItemPower) :
		itemgen::configs::LobbyItem((chest.StaticRewardItemId));

	if (const auto character = GetPlayerCharacter()) {
		return generate({}, itemgen::looters::FromPlayerCharacter(*character), config);
	}
	else {
		return generate({}, itemgen::looters::Unknown(), config);
	}
}

int32 ULootUnlockerComponent::GetEmeraldCount() const {	
	if (auto* controller = GetPlayerController()) {
		if (auto* wallet = controller->GetWalletComponent()) {
			return wallet->GetEmeraldBalance();
		
		}
	}
	return 0;
}

int32 ULootUnlockerComponent::GetCharacterLevel() const {
	if (auto* player = GetPlayerCharacter()) {
		if (auto* experience = player->FindComponentByClass<UPlayerExperienceComponent>()) {
			return experience->CurrentLevel();
		}
	}

	return 0;
}

void ULootUnlockerComponent::UnlockRequested() const {
	if (const auto chest = mChest.Get()) {
		const bool canBeUnlocked = chest->CanBeUnlockedBy(GetPlayerCharacter());
		const FChestDynamicProperties unlockProperties(chest->GetEmeraldUnlockCost(GetPlayerCharacter()), chest->StaticRewardItemId, chest->ChestLootType, chest->EmeraldsReward);

		if (chest->UnlockRequirements.canBeReused || canBeUnlocked) {
			switch (chest->ChestLootType)
			{
			case ELobbyChestLootType::RandomGear:
				OnUnlockRequestedRandomGear.Broadcast(chest->ChestType, chest->UnlockRequirements, unlockProperties, canBeUnlocked);
				break;
			case ELobbyChestLootType::RandomItem:
				OnUnlockRequestedRandomItem.Broadcast(chest->ChestType, chest->UnlockRequirements, unlockProperties, canBeUnlocked);
				break;
			case ELobbyChestLootType::Static:
			{
				const FItemMetaData metaData(chest->StaticRewardItemId);
				OnUnlockRequestedStatic.Broadcast(chest->ChestType, chest->UnlockRequirements, unlockProperties, metaData, canBeUnlocked);
			}
			break;
			case ELobbyChestLootType::Emeralds:
				OnUnlockRequestedEmeralds.Broadcast(chest->ChestType, chest->UnlockRequirements, unlockProperties);
				break;
			default:
				ensure(false);

			}

			OnAnyUnlockRequested.Broadcast(chest->ChestType);
		}
		else {
			OnFailedToOpenChest.Broadcast();
			chest->FailedToUnlock();
		}
	}
}

bool ULootUnlockerComponent::ServerRequestOpen_Validate(ALootActor* chest) {
	return true;
}

void ULootUnlockerComponent::ServerRequestOpen_Implementation(ALootActor* chest) {
	auto character = GetPlayerCharacter();
	chest->Open(character);
	character->GetClientEventHubComponent()->OpenChest();
}

void ULootUnlockerComponent::RequestUnlock(ALobbyChest* chest) {
	mChest = chest;

	UnlockRequested();
}

void ULootUnlockerComponent::UnlockChest() {
	auto chest = mChest.Get();

	if (!chest) {
		ensure_networked(false, this);
		return;
	}
	chest->UnlockedBy(GetPlayerCharacter());

	GiveLoot();
	OnUnlockFinished.Broadcast();
}

void ULootUnlockerComponent::Accept() {
	//Server uses its own reference to which loot we are trying to unlock
	if (auto chest = mChest.Get()) {
		if (chest->CanBeUnlockedBy(GetPlayerCharacter()) && TryConsumeEmeralds()) {
			UnlockChest();
		}

		OnAnyUnlockCompleted.Broadcast(chest->ChestType, true);
		mChest.Reset();
	}
	else {
		OnAnyUnlockCompleted.Broadcast(chest->ChestType, false);
	}
}

void ULootUnlockerComponent::Reject() {
	if (auto chest = mChest.Get()) {
		OnAnyUnlockCompleted.Broadcast(chest->ChestType, false);
	}
	mChest.Reset();
}
