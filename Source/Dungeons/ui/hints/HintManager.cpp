#include "Dungeons.h"
#include "HintManager.h"
#include "ui/MissionProgressHandler.h"
#include "game/event/TileEvents.h"
#include "game/trigger/Triggers.h"
#include "game/objective/ObjectivesSystem.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/loot/LobbyChest.h"
#include "game/actor/StrongholdFrame.h"
#include "game/actor/props/StrongholdSubdungeonDoor.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/component/SecretComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/component/PickupStorableComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/LootUnlockerComponent.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "game/ObjectDistanceNotifier.h"
#include "game/item/instance/AItemInstance.h"
#include "online/sessions/OnlineUtil.h"

#include "UnrealString.h"

#include <set>
#include <vector>
#include <algorithm>
#include "game/component/ShopperComponent.h"

void UHintManager::LevelLoaded(const game::Game& game) {
	HintStates.clear();

	TArray<ABasePlayerController*> playerControllers;
	TArray<APlayerCharacter*> playerCharacters;
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		if (auto playerController = Cast<ABasePlayerController>(it->Get())) {
			if (auto playerCharacter = playerController->GetControlledPlayerCharacter()) {
				if (auto serializer = playerCharacter->GetCharacterSerializeComponent()) {
					if (serializer->HasProfile()) {
						// This is a local player that has hint progress

						playerControllers.Add(playerController);
						playerCharacters.Add(playerCharacter);

						const auto& hintsExpired = serializer->GetExpiredUiHints();
						for (const auto& hintProgress : hintsExpired) {
							HintStates[hintProgress.hintType].completed = true;
						}
					}
				}
			}
		}
	}

	ResetInitializers(game, playerControllers, playerCharacters);

	ELevelNames levelName = game.settings().levelName;
	SetupHintListeners(game, levelName, playerCharacters);
}

bool UHintManager::IsHintVisible(EUIHintType hintType) const {
	auto it = HintStates.find(hintType);
	if (it != HintStates.end()) {
		return it->second.visible;
	}
	else {
		return false;
	}
}

// D11.DJB
void UHintManager::InitHintsUserEnabled(bool Enabled)
{
	mUserEnabledHints = Enabled;
}

void UHintManager::SetHintsUserEnabled(bool Enabled)
{
	mUserEnabledHints = Enabled;

	if (!mUserEnabledHints)
		HideAllHints();
}

void UHintManager::SetupHintListeners(const game::Game& game, ELevelNames levelName, const TArray<APlayerCharacter*>& playerCharacters) {
	const bool isLobby = levelName == ELevelNames::Invalid;
	if (levelName == ELevelNames::squidcoast) {
		ShowHint(EUIHintType::Movement);

		PlayerMovementListeners.AddListener([this]() {
			CompleteHint(EUIHintType::Movement);
		});

		PlayerMovementListeners.AddListener([this]() {
			ShowHint(EUIHintType::Objective);
		});
		ObjectiveListeners.AddListener([this](const FString& id) {
			if (id == "squid_coast_zombie") {
				CompleteHint(EUIHintType::Objective);
				ShowHint(EUIHintType::DefeatZombie);
			}

			if (id == "squid_coast_zombie_defeated") {
				CompleteHint(EUIHintType::DefeatZombie);
			}

			if (id == "squid_coast_shoot_skeleton") {
				ShowHint(EUIHintType::PickupArrow);
			}
		});
		ItemPickedUpListeners.AddListener([this](const FItemId& itemType, ABasePlayerController*) {
			if (itemType == game::item::type::Arrow.getId()) {
				CompleteHint(EUIHintType::PickupArrow);
				ShowHint(EUIHintType::RangedAttack);
			}
		});
		RangedAttackListeners.AddListener([this]() {
			CompleteHint(EUIHintType::RangedAttack);
		});

		ObjectiveListeners.AddListener([this](const FString& id) {
			if (id == "squid_coast_fireworks_arrow_found") {
				ShowHint(EUIHintType::Artifact_Pickup);
			}
		});
		struct FireworksArrowHintState {
			ABasePlayerController* PlayerWithFireworks = nullptr;
			bool Reopen = false;
		};
		auto sharedState = std::make_shared<FireworksArrowHintState>();
		ItemPickedUpListeners.AddListener([this, sharedState](const FItemId& itemType, ABasePlayerController* playerController) {
			if (itemType == game::item::type::FireworksArrowItem.getId()) {
				CompleteHint(EUIHintType::Artifact_Pickup);
				ShowHint(EUIHintType::Artifact_OpenInventory);

				sharedState->PlayerWithFireworks = playerController;
			}
		});
		InventoryOpenedListeners.AddListener([this, sharedState](ABasePlayerController* playerController) {
			if (sharedState->PlayerWithFireworks == playerController) {
				CompleteHintIfVisible(EUIHintType::Artifact_OpenInventory);

				if (IsCompleted(EUIHintType::Artifact_OpenInventory)) {
					ShowHint(EUIHintType::Artifact_Equip);
				}
			}
			else if (IsHintVisible(EUIHintType::Artifact_OpenInventory)) {
				HideHintIfVisible(EUIHintType::Artifact_OpenInventory);
				sharedState->Reopen = true;
			}
		});
		InventoryClosedListeners.AddListener([this, sharedState](ABasePlayerController*) {
			if (IsHintVisible(EUIHintType::Artifact_Equip)) {
				HideHintIfVisible(EUIHintType::Artifact_Equip);
				ResetCompletion(EUIHintType::Artifact_Equip);
				ResetCompletion(EUIHintType::Artifact_OpenInventory);

				ShowHint(EUIHintType::Artifact_OpenInventory);
			}

			if (IsCompleted(EUIHintType::Artifact_Equip)) {
				ShowHint(EUIHintType::Artifact_Activate);
			}

			if (sharedState->Reopen) {
				ShowHint(EUIHintType::Artifact_OpenInventory);
				sharedState->Reopen = false;
			}
		});
		ItemEquippedListeners.AddListener([this](UItemSlot* itemSlot) {
			if (const auto& item = itemSlot->GetItem()) {
				if (item->GetItemId() == game::item::type::FireworksArrowItem.getId()) {
					CompleteHintIfVisible(EUIHintType::Artifact_Equip);
				}
			}
		});
		ItemActivatedListeners.AddListener([this](UItemSlot* itemSlot) {
			if (const auto& item = itemSlot->GetItem()) {
				if (item->GetItemId() == game::item::type::FireworksArrowItem.getId()) {
					CompleteHintIfVisible(EUIHintType::Artifact_Activate);
					if (IsCompleted(EUIHintType::Artifact_Activate)) {
						ShowHint(EUIHintType::Artifact_RangedAttack);
					}
				}
			}
		});
		RangedAttackListeners.AddListener([this]() {
			CompleteHintIfVisible(EUIHintType::Artifact_RangedAttack);
		});

	}

	if (!IsCompleted(EUIHintType::Teleport) && !isLobby) {
		FarFromPlayers.AddListener([this] {
			ShowHintTimed(EUIHintType::Teleport, std::chrono::seconds(10));
		});

		TeleportActivated.AddListener([this] {
			CompleteHintIfVisible(EUIHintType::Teleport);
		});
	}

	if (!IsCompleted(EUIHintType::LowHealth)) {
		DamageReceivedListeners.AddListener([this](float dealtDamage, const UHealthComponent* healthComponent) {
			const auto halfValue = healthComponent->GetMaximumHealth() / 2;
			const auto currentHealth = healthComponent->GetCurrentHealth();
			// health went below half
			if (currentHealth < halfValue && currentHealth + dealtDamage >= halfValue) {
				ShowHint(EUIHintType::LowHealth);
			}
		});
		ItemActivatedListeners.AddListener([this](UItemSlot* itemSlot) {
			if (const auto& item = itemSlot->GetItem()) {
				if (item->GetItemId() == game::item::type::HealthPotion.getId()) {
					CompleteHint(EUIHintType::LowHealth);
				}
			}
		});
	}

	if (!IsCompleted(EUIHintType::Enchanting_ItemEnchanted)) {
		if (isLobby || levelName == ELevelNames::squidcoast) {
			LevelUpListeners.AddListener([this, levelName](int32 newLevel) {
				const int levelToShowHint = 3;
				if (newLevel >= levelToShowHint || levelName == ELevelNames::squidcoast) {
					ResetCompletion(EUIHintType::Enchanting_OpenInventory);
					ResetCompletion(EUIHintType::Enchanting_SelectGear);
					ResetCompletion(EUIHintType::Enchanting_SelectEnchantment);

					ShowHint(EUIHintType::Enchanting_OpenInventory);
				}
			});
			InventoryOpenedListeners.AddListener([this](ABasePlayerController*) {
				CompleteHintIfVisible(EUIHintType::Enchanting_OpenInventory);
				if (IsCompleted(EUIHintType::Enchanting_OpenInventory)) {
					ShowHint(EUIHintType::Enchanting_SelectGear);
				}
			});
			GearSelectedListener.AddListener([this](const UInventoryItemSlot* inventoryItemSlot) {
				if (auto item = inventoryItemSlot->Item) {
					if (item->Item.Enchantments.Num() > 0) {
						if (IsHintVisible(EUIHintType::Enchanting_SelectGear)) {
							CompleteHint(EUIHintType::Enchanting_SelectGear);
							ShowHint(EUIHintType::Enchanting_SelectEnchantment);
						}
					}
				}
			});
			EnchantmentSelectedListener.AddListener([this]() {
				CompleteHintIfVisible(EUIHintType::Enchanting_SelectEnchantment);
			});
			InventoryClosedListeners.AddListener([this](ABasePlayerController*) {
				HideHintIfVisible(EUIHintType::Enchanting_SelectGear);
				HideHintIfVisible(EUIHintType::Enchanting_SelectEnchantment);
			});
		}
		ItemEnchantedListener.AddListener([this]() {
			CompleteHint(EUIHintType::Enchanting_ItemEnchanted);
		});

		if (isLobby) {
			for (auto playerCharacter : playerCharacters) {
				if (auto itemStashComponent = playerCharacter->FindComponentByClass<UItemStashComponent>()) {
					if (itemStashComponent->InvestedEnchantmentPointsTotal() > 0) {
						CompleteHint(EUIHintType::Enchanting_ItemEnchanted);
						break;
					}
				}
				if (auto experienceComponent = playerCharacter->FindComponentByClass<UPlayerExperienceComponent>()) {
					OnLevelUp(experienceComponent->GetLocalCurrentLevel());
				}
			}
		}
	}

	if (!IsCompleted(EUIHintType::MissionSelect_Marker)) {
		if (isLobby) {
			ShowHint(EUIHintType::MissionSelect_Marker);
			ShowHint(EUIHintType::MissionSelect_Popup);

			MissionSelectOpened.AddListener([this]() {
				HideHintIfVisible(EUIHintType::MissionSelect_Popup);
			});
		}
		if (levelName != ELevelNames::Invalid && levelName != ELevelNames::squidcoast) {
			CompleteHint(EUIHintType::MissionSelect_Marker);
		}
	}

	if (!IsCompleted(EUIHintType::Merchants_Reserve)) {
		MerchantItemReserved.AddListener([this]() {
			ShowHintTimedAndComplete(EUIHintType::Merchants_Reserve, std::chrono::seconds(15));
		});
	}

	if (!IsCompleted(EUIHintType::Merchants_Interact)) {
		if (isLobby) {
			ShowHint(EUIHintType::Merchants_FindMerchants);

			LobbyChestOpened.AddListener([this](ELobbyChestType chestType) {
				if (chestType != ELobbyChestType::Chest) {
					HideHintIfVisible(EUIHintType::Merchants_FindMerchants);
					ShowHint(EUIHintType::Merchants_Interact);
				}
			});

			LobbyChestClosed.AddListener([this](ELobbyChestType chestType, bool success) {
				if (chestType != ELobbyChestType::Chest) {
					CompleteHint(EUIHintType::Merchants_Interact);
				}
			});
		}
	}

	if (!IsCompleted(EUIHintType::Map) && !isLobby) {
		DeadEndEntered.AddListener([this]() {
			ShowHint(EUIHintType::Map);
		});
		MapToggled.AddListener([this]() {
			CompleteHint(EUIHintType::Map);
		});
	}

	// #D11.CM
	if (!IsCompleted(EUIHintType::Popping)) {
		PoppingListeners.AddListener([this](APlayerCharacter* player) {

			// Show the hint on a timer, complete on timeout
			ShowHintTimedAndComplete(EUIHintType::Popping, std::chrono::seconds(10));
		});
	}

	if (levelName == ELevelNames::thestronghold) // << replace with check for stronghold level
	{
		StrongholdEndPortalInteractionListener.AddListener([this](const StrongholdProgressSaveData* strongholdProgress) {
			ShowHintTimed(EUIHintType::Stronghold_EndPortal, std::chrono::seconds(13)); 
		});

		StrongholdSubDungeonDoorInteractionListener.AddListener([this]() {
			ShowHintTimed(EUIHintType::Stronghold_DoorLocked, std::chrono::seconds(13));
		});

		StrongholdSecretLootFoundListener.AddListener([this]() {
			ShowHintTimed(EUIHintType::Stronghold_SecretLootFound, std::chrono::seconds(13));
		});
	}

	if (!IsCompleted(EUIHintType::ChatWheel_Chat)) {
		std::shared_ptr<bool> chatWheelHintsEnabled = std::make_shared<bool>(false);

		ServerPlayerAdded.AddListener([this, chatWheelHintsEnabled](APlayerCharacter* player) {
			if (!player->IsLocallyControlled()) {
				if (!*chatWheelHintsEnabled && !IsCompleted(EUIHintType::ChatWheel_Chat)) {
					*chatWheelHintsEnabled = true;
					ShowHint(EUIHintType::ChatWheel_Open);
				}
			}
		});

		ServerPlayerRemoved.AddListener([this, gameState = game.world().GetGameState(), chatWheelHintsEnabled] {
			if (auto dungeonsGameState = Cast<ADungeonsGameState>(gameState)) {
				if (*chatWheelHintsEnabled && !dungeonsGameState->IsMultiplayer()) {
					*chatWheelHintsEnabled = false;
					HideHintIfVisible(EUIHintType::ChatWheel_Chat);
					HideHintIfVisible(EUIHintType::ChatWheel_Open);
					HideHintIfVisible(EUIHintType::ChatWheel_Select);
					HideHintIfVisible(EUIHintType::Teleport);
				}
			}
		});

		if (!game.world().IsServer()) {
			*chatWheelHintsEnabled = true;
			ShowHint(EUIHintType::ChatWheel_Open);
		}

		ChatWheelVisibilityChangedListeners.AddListener([this, chatWheelHintsEnabled](bool visible) {
			if (!IsCompleted(EUIHintType::ChatWheel_Chat) && *chatWheelHintsEnabled) {
				ResetCompletion(EUIHintType::ChatWheel_Open);
				ResetCompletion(EUIHintType::ChatWheel_Select);

				if (visible) {
					HideHintIfVisible(EUIHintType::ChatWheel_Open);
					ShowHint(EUIHintType::ChatWheel_Select);
				}
				else {
					HideHintIfVisible(EUIHintType::ChatWheel_Select);
					HideHintIfVisible(EUIHintType::ChatWheel_Chat);

					ShowHint(EUIHintType::ChatWheel_Open);
				}
			}
		});

		ChatWheelOptionSelected.AddListener([this, chatWheelHintsEnabled] {
			if (*chatWheelHintsEnabled) {
				CompleteHint(EUIHintType::ChatWheel_Select);
				ShowHint(EUIHintType::ChatWheel_Chat);
			}
		});

		ChatWheelMessageSent.AddListener([this] {
			CompleteHint(EUIHintType::ChatWheel_Chat);
			HideHintIfVisible(EUIHintType::ChatWheel_Select);
			HideHintIfVisible(EUIHintType::ChatWheel_Open);
		});
	}
}

void UHintManager::ResetInitializers(const game::Game& game, TArray<ABasePlayerController*> playerControllers, TArray<APlayerCharacter*> playerCharacters) {

	ObjectiveListeners = { [this, &game]() {
		if (auto objectives = game.objectives()) {
			if (auto missionProgressHandler = objectives->GetMissionProgressHandler()) {
				missionProgressHandler->OnNewObjectiveStarted.AddDynamic(this, &UHintManager::OnObjectiveUpdated);
			}
		}
	} };

	PlayerMovementListeners = { [this, playerControllers]() {
		for (auto playerController : playerControllers) {
			playerController->OnPlayerMovement.AddUObject(this, &UHintManager::OnPlayerMovement);
		}
	} };

	RangedAttackListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto rangedAttackComponent = playerCharacter->FindComponentByClass<URangedAttackComponent>()) {
				rangedAttackComponent->OnPlayerRangedAttack.AddUObject(this, &UHintManager::OnRangedAttack);
			}
		}
	} };

	PoppingListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			playerCharacter->OnPlayerPopped.AddUObject(this, &UHintManager::OnPopped);
		}
	} };

	DamageReceivedListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto healthComponent = playerCharacter->FindComponentByClass<UHealthComponent>()) {
				healthComponent->OnDamageReceived.AddUObject(this, &UHintManager::OnDamageReceived, healthComponent);
			}
		}
	} };

	ItemEquippedListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto equipmentComponent = playerCharacter->FindComponentByClass<UEquipmentComponent>()) {
				for (auto slot : equipmentComponent->GetEquippableSlots()) {
					slot->OnItemInstanceReplicatedInternal.AddUObject(this, &UHintManager::OnSlotUpdated);
				}
			}
		}
	} };

	ItemActivatedListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto equipmentComponent = playerCharacter->FindComponentByClass<UEquipmentComponent>()) {
				for (auto slot : equipmentComponent->GetEquippableSlots()) {
					slot->OnItemSlotActivationCompleted.AddDynamic(this, &UHintManager::OnSlotActivated);
				}
				equipmentComponent->GetSlot(ESlotType::HealthPotion).OnItemSlotActivationCompleted.AddDynamic(this, &UHintManager::OnSlotActivated);
			}
		}
	} };

	ItemPickedUpListeners = { [this, playerControllers]() {
		for (auto playerController : playerControllers) {
			if (auto pickupStorable = playerController->GetPickupStorableComponent()) {
				pickupStorable->OnStorableItemPickedUp.AddUObject(this, &UHintManager::OnItemPickedUp, playerController);
			}
		}
	} };
	
	GearSelectedListener = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto itemStashComponent = playerCharacter->FindComponentByClass<UItemStashComponent>()) {
				itemStashComponent->OnInventoryItemSlotSelected.AddUObject(this, &UHintManager::OnInventoryItemSlotSelected);
			}
		}
	} };

	EnchantmentSelectedListener = { [this]() {
	} };

	ItemEnchantedListener = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto itemStashComponent = playerCharacter->FindComponentByClass<UItemStashComponent>()) {
				itemStashComponent->OnAvailableEnchangmentPointsChanged.AddDynamic(this, &UHintManager::OnAvailableEnchantmentPointsChanged);
			}
		}
	} };

	InventoryOpenedListeners = {};

	InventoryClosedListeners = {};

	LobbyChestOpened = { [this, playerControllers]() {
		for (auto playerController : playerControllers) {
			if (auto lootUnlockedComponent = playerController->FindComponentByClass<ULootUnlockerComponent>()) {
				lootUnlockedComponent->OnAnyUnlockRequested.AddUObject(this, &UHintManager::OnLobbyChestOpened);
			}
		}
	} };
	LobbyChestClosed = { [this, playerControllers]() {
		for (auto playerController : playerControllers) {
			if (auto lootUnlockedComponent = playerController->FindComponentByClass<ULootUnlockerComponent>()) {
				lootUnlockedComponent->OnAnyUnlockCompleted.AddUObject(this, &UHintManager::OnLobbyChestClosed);
			}
		}
	} };

	if (game.settings().levelName == ELevelNames::thestronghold) {
		StrongholdEndPortalInteractionListener = { [this]() {
			if (auto endPortalActor = actorquery::getFirstActor<AStrongholdFrame>(GetWorld()))
			{
				endPortalActor->OnNoEffectEndPortalInteraction.AddUObject(this, &UHintManager::OnStrongholdEndPortalInteration);
			}
		} };

		StrongholdSubDungeonDoorInteractionListener = { [this]() {

			for (auto* subDungeonsDoor : actorquery::getActors<AStrongholdSubdungeonDoor>(GetWorld()))
			{
				subDungeonsDoor->InteractableComponent->OnReplicatedFail.AddDynamic(this, &UHintManager::OnStrongholdLockedSubDungeonDoor);
			}

		} };

		StrongholdSecretLootFoundListener = { [this]() {
			// only trigger for 'thestronghold' chests
			for (auto* lootChest : actorquery::getActors<ALootActor>(GetWorld()))
			{
				if (auto secretComponent = lootChest->FindComponentByClass<USecretComponent>())
				{
					secretComponent->OnSecretFoundRecieved.AddDynamic(this, &UHintManager::OnStrongholdSecretLootFound);
				}
			}

		} };

	} // end if level == thestronghold

	MerchantItemReserved = { [this, playerControllers]() {
		for (auto playerController : playerControllers) {
			if (auto shopperComponent = playerController->FindComponentByClass<UShopperComponent>()) {
				shopperComponent->OnItemReserved.AddUObject(this, &UHintManager::OnMerchantItemReserved);
			}
		}
	} };

	DeadEndEntered = { [this, &game, playerCharacters]() {
		game.tileEvents().tileEnterNew([this, playerCharacters](const game::events::TileState& tileState) {
			for (auto playerCharacter : playerCharacters) {
				if (&tileState.player == playerCharacter) {
					if (tileState.tile.progress().isOnStrayPath()) {
						int remaining = tileState.tile.progress().strayPath().remaining();
						if (remaining == 1) {
							for (const auto& listener : DeadEndEntered.AllListeners()) {
								listener();
							}
						}
					}
				}
			}
		});
	} };

	FarFromPlayers = { [this, playerControllers] {
		for (auto playerController : playerControllers) {
			playerController->InitializeAndGetDistanceNotifier()->OnNotifyFarAway.AddUObject(this, &UHintManager::OnPlayersFarAway);
		}
	}};

	TeleportActivated = { [this, playerControllers] {
		for (auto playerController : playerControllers) {
			playerController->OnPlayerTeleportButton.AddDynamic(this, &UHintManager::OnTeleportPressed);
		}
	}};

	LevelUpListeners = { [this, playerCharacters]() {
		for (auto playerCharacter : playerCharacters) {
			if (auto playerExperienceComponent = playerCharacter->FindComponentByClass<UPlayerExperienceComponent>()) {
				playerExperienceComponent->OnLevelUp.AddDynamic(this, &UHintManager::OnLevelUp);
			}
		}
	}};

	ServerPlayerAdded = { [this, &world = game.world()] {
		if (auto gameMode = Cast<ADungeonsGameMode>(world.GetAuthGameMode())) {
			gameMode->OnPlayerCharacterAdded.AddUObject(this, &UHintManager::OnPlayerCharacterAdded);
		}
	}};

	ServerPlayerRemoved = { [this, &world = game.world()] {
		if (auto gameMode = Cast<ADungeonsGameMode>(world.GetAuthGameMode())) {
			gameMode->OnPlayerCharacterRemoved.AddUObject(this, &UHintManager::OnPlayerCharacterRemoved);
		}
	}};
}

bool UHintManager::ShowHint(EUIHintType hintType) {
	auto& hintState = HintStates[hintType];
	
	if (mUserEnabledHints && !hintState.visible && !hintState.completed) {
		hintState.visible = true;
		OnUIHintVisibility.Broadcast(hintType, EVisibilityReason::Activated);
		
		return true;
	}
	return false;
}

void UHintManager::ShowHintTimed(EUIHintType hintType, std::chrono::milliseconds time) {
	if (ShowHint(hintType))
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UHintManager::HideHintIfVisible, hintType), time.count() / 1000.f, false);
	}
}

void UHintManager::ShowHintTimedAndComplete(EUIHintType hintType, std::chrono::milliseconds time) {
	if (ShowHint(hintType))
	{
		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UHintManager::CompleteHintIfVisible, hintType), time.count() / 1000.f, false);
	}
}

void UHintManager::HideHintIfVisible(EUIHintType hintType) {
	auto& hintState = HintStates[hintType];

	if (hintState.visible) {
		hintState.visible = false;
		OnUIHintVisibility.Broadcast(hintType, EVisibilityReason::Forgot);
	}
}

void UHintManager::CompleteHintIfVisible(EUIHintType hintType) {
	auto& hintState = HintStates[hintType];

	if (!mUserEnabledHints || hintState.visible) {
		CompleteHint(hintType);
	}
}

void UHintManager::CompleteHint(EUIHintType hintType) {
	auto& hintState = HintStates[hintType];
	if (hintState.completed != true) {
		hintState.completed = true;
		SaveHintExpired(hintType);

		if (hintState.visible) {
			hintState.visible = false;
			OnUIHintVisibility.Broadcast(hintType, EVisibilityReason::Completed);
		}
	}
}

bool UHintManager::IsCompleted(EUIHintType hintType) const {
	auto it = HintStates.find(hintType);
	if (it != HintStates.end()) {
		return it->second.completed;
	}
	else {
		return false;
	}
}

void UHintManager::ResetCompletion(EUIHintType hintType) {
	HintStates[hintType].completed = false;
}

void UHintManager::HideAllHints()
{
	for (int i = enum_cast(EUIHintType::ENUM_COUNT) - 1; i > EHintTypeFirstValue; --i)
	{
		EUIHintType hintType = static_cast<EUIHintType>(i);
		HideHintIfVisible(hintType);
	}
}

void UHintManager::OnObjectiveUpdated(const FText& Name, const FString& Id)
{
	for (const auto& listener : ObjectiveListeners.AllListeners()) {
		listener(Id);
	}
}

void UHintManager::OnSlotUpdated(UItemSlot* itemSlot) const
{
	for (const auto& listener : ItemEquippedListeners.AllListeners()) {
		listener(itemSlot);
	}
}

void UHintManager::OnSlotActivated(UItemSlot* itemSlot, bool outcome) {
	for (const auto& listener : ItemActivatedListeners.AllListeners()) {
		listener(itemSlot);
	}
}

void UHintManager::OnAvailableEnchantmentPointsChanged(int NewAmount, UItemStashComponent* itemStashComponent) {
	if (itemStashComponent->InvestedEnchantmentPointsTotal() > 0) {
		for (const auto& listener : ItemEnchantedListener.AllListeners()) {
			listener();
		}
	}
}

void UHintManager::OnInventoryItemSlotSelected(const UInventoryItemSlot* inventoryItemSlot) {
	for (const auto& listener : GearSelectedListener.AllListeners()) {
		listener(inventoryItemSlot);
	}
}

void UHintManager::OnStrongholdSecretLootFound() {
	for (const auto& listener : StrongholdSecretLootFoundListener.AllListeners()) {
		listener();
	}
}

void UHintManager::OnStrongholdLockedSubDungeonDoor() {
	for (const auto& listener : StrongholdSubDungeonDoorInteractionListener.AllListeners()) {
		listener();
	}
}

void UHintManager::OnStrongholdEndPortalInteration(const StrongholdProgressSaveData* strongholdProgress) {
	for (const auto& listener : StrongholdEndPortalInteractionListener.AllListeners()) {
		listener(strongholdProgress);
	}
}

void UHintManager::OnLobbyChestOpened(ELobbyChestType chestType) {
	for (const auto& listener : LobbyChestOpened.AllListeners()) {
		listener(chestType);
	}
}

void UHintManager::OnLobbyChestClosed(ELobbyChestType chestType, bool success) {
	for (const auto& listener : LobbyChestClosed.AllListeners()) {
		listener(chestType, success);
	}
}

void UHintManager::OnLevelUp(int32 newLevel) {
	for (const auto& listener : LevelUpListeners.AllListeners()) {
		listener(newLevel);
	}
}

void UHintManager::OnPlayersFarAway() 
{
	for (const auto& listener : FarFromPlayers.AllListeners()) {
		listener();
	}
}

void UHintManager::OnTeleportPressed()
{
	for (const auto& listener : TeleportActivated.AllListeners()) {
		listener();
	}
}

void UHintManager::OnItemPickedUp(const FItemId& itemType, ABasePlayerController* playerController)
{
	for (const auto& listener : ItemPickedUpListeners.AllListeners()) {
		listener(itemType, playerController);
	}
}

void UHintManager::OnPlayerMovement(const ABasePlayerController*)
{
	for (const auto& listener : PlayerMovementListeners.AllListeners()) {
		listener();
	}
}

void UHintManager::OnRangedAttack(const APlayerCharacter*)
{
	for (const auto& listener : RangedAttackListeners.AllListeners()) {
		listener();
	}
}

void UHintManager::OnDamageReceived(float dealtDamage, UHealthComponent* healthComponent) {
	for (const auto& listener : DamageReceivedListeners.AllListeners()) {
		listener(dealtDamage, healthComponent);
	}
}

void UHintManager::OnPopped(APlayerCharacter* player) {
	for (const auto& listener : PoppingListeners.AllListeners()) {
		listener(player);
	}
}

void UHintManager::OnPlayerCharacterAdded(APlayerCharacter* player) {
	for (const auto& listener : ServerPlayerAdded.AllListeners()) {
		listener(player);
	}
}

void UHintManager::OnPlayerCharacterRemoved() {
	for (const auto& listener : ServerPlayerRemoved.AllListeners()) {
		listener();
	}
}

void UHintManager::OnInventoryOpened(ABasePlayerController* playerController) {
	for (const auto& listener : InventoryOpenedListeners.AllListeners()) {
		listener(playerController);
	}
}

void UHintManager::OnInventoryClosed(ABasePlayerController* playerController) {
	for (const auto& listener : InventoryClosedListeners.AllListeners()) {
		listener(playerController);
	}
}

void UHintManager::OnEnchantmentSelected() {
	for (const auto& listener : EnchantmentSelectedListener.AllListeners()) {
		listener();
	}
}

void UHintManager::OnMissionSelectOpened() {
	for (const auto& listener : MissionSelectOpened.AllListeners()) {
		listener();
	}
}

void UHintManager::OnMerchantItemReserved() {
	for (const auto& listener : MerchantItemReserved.AllListeners()) {
		listener();
	}
}

void UHintManager::OnMapToggled() {
	for (const auto& listener : MapToggled.AllListeners()) {
		listener();
	}
}

void UHintManager::OnInventoryFilterChanged(const FString& itemFilterMode) {
	for (const auto& listener : InventoryFilterChanged.AllListeners()) {
		listener(itemFilterMode);
	}
}

void UHintManager::OnChatWheelVisibilityChanged(bool visible) {
	for (const auto& listener : ChatWheelVisibilityChangedListeners.AllListeners()) {
		listener(visible);
	}
}

void UHintManager::OnChatWheelOptionSelected() {
	for (const auto& listener : ChatWheelOptionSelected.AllListeners()) {
		listener();
	}
}

void UHintManager::OnChatWheelMessageSent() {
	for (const auto& listener : ChatWheelMessageSent.AllListeners()) {
		listener();
	}
}

void UHintManager::SaveHintExpired(EUIHintType hintType)
{
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		auto playerController = Cast<ABasePlayerController>(it->Get());
		if (playerController) {
			if (auto serializer = playerController->GetCharacterSerializeComponent()) {
				if (serializer->HasProfile()) {
					auto& hintsExpired = serializer->GetExpiredUiHints();
					if (algo::none_of(hintsExpired, RETLAMBDA(it.hintType == hintType))) {
						hintsExpired.push_back({ hintType });
					}
				}
			}
		}
	}
}
