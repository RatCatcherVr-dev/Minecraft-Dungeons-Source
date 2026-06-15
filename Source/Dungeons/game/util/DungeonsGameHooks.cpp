#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "game/component/ItemStashComponent.h"
#include "game/util/ActorQuery.h"
#include "game/GameProgress.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"
#include "SharedConstants.h"
#include "platform/GameVersion.h"
//D11.PS - PLATFORM_WINDOWS is defined on XB1 so need to be more specific.
#if (!PLATFORM_PS4 && !PLATFORM_SWITCH) && (PLATFORM_XBOXONE || PLATFORM_WINDOWS)
#include "UnrealToolsFrameworkPCH.h"
#endif

#ifdef ENABLE_GAMESTEST_RPC

#include "UnrealEngineHelperFunctions.h"

#include "RpcArchive.h"
#include "RpcArchiveExtensions.h"
#include "RpcErrorCodes.h"
#include "RpcHookCode.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"

#include "BotAutomationPCH.h"

#ifdef ENABLE_BOTAUTOMATION

static HRESULT GetDefaultActionInputs(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, int* outPlayerIndex, FString* outActionName)
{
	HRESULT Result = S_OK;

	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), outPlayerIndex));
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("Name"), Input, outActionName));

	return Result;
}

template<typename Callback>
static HRESULT CheckLoadingScreen(int playerIndex, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output, Callback callback)
{
	HRESULT Result = S_OK;
	if (auto localPlayer = Microsoft::Internal::GamesTest::UnrealToolsFramework::RetrieveLocalPlayerByIndex(playerIndex))
	{
		if (auto gameInstance = localPlayer->GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {
			if (gameInstance->IsLoadingScreenActive()) {
				return BA_E_LOADING_SCREEN_ACTIVE;
			}
		}

		return callback(localPlayer);
	}

	UNREFERENCED_PARAMETER(Output);
	return UTF_E_PLAYER_WAS_NOT_FOUND;
}

template<typename AddActionCallback>
static HRESULT AddAutomationAction(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output, AddActionCallback addActionCallback)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	FString ActionName;
	HRCHK(GetDefaultActionInputs(Input, &PlayerIndex, &ActionName));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		return addActionCallback(PlayerIndex, ActionName, localPlayer);
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode AddAutomation(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		BotAutomation::AddAutomatorToPlayer(PlayerIndex, localPlayer);
		return S_OK;
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode RemoveAutomation(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		BotAutomation::RemoveAutomatorFromPlayer(PlayerIndex);
		return S_OK;
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode ClearActions(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		BotAutomation::ClearActions(PlayerIndex);
		return S_OK;
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode KeyPressAction(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString KeyString;
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("Key"), Input, &KeyString));

	bool Viewport = false;
	HRCHK(Input.GetNamedValue(TEXT("Viewport"), &Viewport));

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		FKey Key = FKey(*KeyString);
		if (Key.IsValid()) {
			if (BotAutomation::AddPlayerKeyPressActionToBot(playerIndex, localPlayer, actionName, Key, Viewport)) {
				return S_OK;
			}

			RETURN_FUNCFAILED("AddPlayerKeyPressActionToBot")
		}

		RETURN_INVALIDPARAM("Key")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode OnFunctionAction(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	uint8 ActionIndex = 0;
	HRCHK(Input.GetNamedValue(TEXT("Action"), &ActionIndex));
	EPlayerOnFunctionType Action = static_cast<EPlayerOnFunctionType>(ActionIndex);

	int Index;
	if (FAILED(Input.GetNamedValue(TEXT("Index"), &Index))) Index = 0;

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		if (BotAutomation::AddPlayerOnFunctionActionToBot(playerIndex, localPlayer, actionName, Action, Index)) {
			return S_OK;
		}

		RETURN_FUNCFAILED("AddPlayerMeleeAttackActionToBot")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode MoveToLocationAction(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive LocationArchive;
	HRCHK(Input.GetNamedValue(TEXT("Location"), &LocationArchive));

	Microsoft::Internal::GamesTest::UnrealToolsFramework::FRpcVector Location(LocationArchive);
	HRCHK(Location.Result);

	bool UsePathFinding = true;
	HRCHK(Input.GetNamedValue(TEXT("PathFinding"), &UsePathFinding));

	float AcceptanceRadius = -1.f;
	HRCHK(Input.GetNamedValue(TEXT("AcceptanceRadius"), &AcceptanceRadius));

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		if (BotAutomation::AddPlayerMoveToLocationBot(playerIndex, localPlayer, actionName, Location, UsePathFinding, AcceptanceRadius)) {
			return S_OK;
		}

		RETURN_FUNCFAILED("AddPlayerMoveToLocationBot")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode MoveToActorAction(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString ClassName;
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("ClassName"), Input, &ClassName));

	FString ObjectName;
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("ObjectName"), Input, &ObjectName));

	bool UsePathFinding = true;
	HRCHK(Input.GetNamedValue(TEXT("PathFinding"), &UsePathFinding));

	float AcceptanceRadius = -1.f;
	HRCHK(Input.GetNamedValue(TEXT("AcceptanceRadius"), &AcceptanceRadius));

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		const auto fullNameMatch = ClassName + " " + ObjectName;
		const auto actors = actorquery::getActors<AActor>(localPlayer->GetWorld(), [&](AActor* candidate) {
			return candidate->GetFullName() == fullNameMatch;
			});

		if (actors.Num() == 0) {
			RETURN_ELEMNOTFND("AActor")
		}
		if (BotAutomation::AddPlayerMoveToActorBot(playerIndex, localPlayer, actionName, actors[0], UsePathFinding, AcceptanceRadius)) {
			return S_OK;
		}

		RETURN_FUNCFAILED("AddPlayerMoveToActorBot")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode PlayerAttackMob(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString ClassName;
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("ClassName"), Input, &ClassName));

	FString ObjectName;
	HRCHK(Microsoft::Internal::GamesTest::UnrealToolsFramework::GetStringValue(TEXT("ObjectName"), Input, &ObjectName));

	bool AllowRanged = true;
	HRCHK(Input.GetNamedValue(TEXT("AllowRanged"), &AllowRanged));

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		const auto fullNameMatch = ClassName + " " + ObjectName;
		const auto actors = actorquery::getActors<ABaseCharacter>(localPlayer->GetWorld(), [&](ABaseCharacter* candidate) {
			return candidate->GetFullName() == fullNameMatch;
			});

		if (actors.Num() == 0) {
			RETURN_ELEMNOTFND("ABaseCharacter")
		}
		if (BotAutomation::AddPlayerAttackMobBot(playerIndex, localPlayer, actionName, actors[0], AllowRanged)) {
			return S_OK;
		}

		RETURN_FUNCFAILED("AddPlayerAttackMobBot")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode PlayerAttackRadius(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive LocationArchive;
	HRCHK(Input.GetNamedValue(TEXT("Location"), &LocationArchive));

	Microsoft::Internal::GamesTest::UnrealToolsFramework::FRpcVector Location(LocationArchive);
	HRCHK(Location.Result);

	uint8 AttackableTeamInt;
	HRCHK(Input.GetNamedValue(TEXT("Team"), &AttackableTeamInt));
	ETeamName AttackableTeam = static_cast<ETeamName>(AttackableTeamInt);

	float AttackRadius = true;
	HRCHK(Input.GetNamedValue(TEXT("Radius"), &AttackRadius));

	bool AllowRanged = true;
	HRCHK(Input.GetNamedValue(TEXT("AllowRanged"), &AllowRanged));

	return AddAutomationAction(Input, Output, [&](int playerIndex, FString actionName, ULocalPlayer* localPlayer) -> HRESULT {
		if (BotAutomation::AddPlayerAttackRadiusBot(playerIndex, localPlayer, actionName, Location, AttackableTeam, AttackRadius, AllowRanged)) {
			return S_OK;
		}

		RETURN_FUNCFAILED("AddPlayerAttackRadiusBot")
		});
}

#endif // ENABLE_BOTAUTOMATION

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode SetGamepadActive(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	bool GamepadActive;
	HRCHK(Input.GetNamedValue(TEXT("Active"), &GamepadActive));

	if (auto localPlayer = Microsoft::Internal::GamesTest::UnrealToolsFramework::RetrieveLocalPlayerByIndex(PlayerIndex))
	{
		if (auto gameInstance = localPlayer->GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {
			if (gameInstance->IsLoadingScreenActive()) {
				return BA_E_LOADING_SCREEN_ACTIVE;
			}

			if (auto Controller = Cast<APlayerControllerBase>(localPlayer->PlayerController)) {
				Controller->OnGamepadActiveChanged(GamepadActive);
			}
			else {
				RETURN_ELEMNOTFND("APlayerControllerBase")
			}

			if (auto controllerTypeManager = gameInstance->GetControllerTypeManager()) {
				if (GamepadActive) {
					controllerTypeManager->OnControllerTypeChanged.Broadcast();
				}
				else {
					controllerTypeManager->OnKeyboardInputUsed.Broadcast();
				}

				return S_OK;
			}

			RETURN_ELEMNOTFND("UControllerTypeManager")
		}

		RETURN_ELEMNOTFND("UDungeonsGameInstance")
	}

	UNREFERENCED_PARAMETER(Output);
	return UTF_E_PLAYER_WAS_NOT_FOUND;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetNextTileDoor(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		if (auto playerController = Cast<ABasePlayerController>(localPlayer->PlayerController))
		{
			if (auto playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter()))
			{
				if (auto gameBP = playerController->GetCachedGameBP())
				{
					FVector Position;
					if (gameBP->GetNextTileDoor(playerCharacter, Position))
					{
						HRCHK(Output.SetNamedValue(TEXT("Location"), Microsoft::Internal::GamesTest::UnrealToolsFramework::FRpcVector(Position)));
						return S_OK;
					}

					RETURN_FUNCFAILED("GetNextTileDoor")
				}

				RETURN_ELEMNOTFND("AGameBP")
			}

			RETURN_ELEMNOTFND("APlayerCharacter")
		}

		RETURN_ELEMNOTFND("ABasePlayerController")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetNextDoor(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		if (auto playerController = Cast<ABasePlayerController>(localPlayer->PlayerController))
		{
			if (auto playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter()))
			{
				if (auto gameBP = playerController->GetCachedGameBP())
				{
					FVector Position;
					if (gameBP->GetNextDoor(playerCharacter, Position))
					{
						HRCHK(Output.SetNamedValue(TEXT("Location"), Microsoft::Internal::GamesTest::UnrealToolsFramework::FRpcVector(Position)));
						return S_OK;
					}

					RETURN_FUNCFAILED("GetNextDoor")
				}

				RETURN_ELEMNOTFND("AGameBP")
			}

			RETURN_ELEMNOTFND("APlayerCharacter")
		}

		RETURN_ELEMNOTFND("ABasePlayerController")
		});
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetNextDoorIndicatorDoor(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	int PlayerIndex;
	HRCHK(Input.GetNamedValue(TEXT("PlayerIndex"), &PlayerIndex));

	return CheckLoadingScreen(PlayerIndex, Output, [&](ULocalPlayer* localPlayer) -> HRESULT {
		if (auto playerController = Cast<ABasePlayerController>(localPlayer->PlayerController))
		{
			if (auto playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter()))
			{
				if (auto gameBP = playerController->GetCachedGameBP())
				{
					FVector Position;
					if (gameBP->GetNextDoorIndicatorDoor(playerCharacter, Position))
					{
						HRCHK(Output.SetNamedValue(TEXT("Location"), Microsoft::Internal::GamesTest::UnrealToolsFramework::FRpcVector(Position)));
						return S_OK;
					}

					RETURN_FUNCFAILED("GetNextDoorIndicatorDoor")
				}

				RETURN_ELEMNOTFND("AGameBP")
			}

			RETURN_ELEMNOTFND("APlayerCharacter")
		}

		RETURN_ELEMNOTFND("ABasePlayerController")
		});
}

HRESULT GetEquipmentSlots(TMap<EEquipmentSlot, UInventoryItemSlot*>& equipmentSlotMap)
{
	HRESULT Result = S_OK;

	UWorld* world = Microsoft::Internal::GamesTest::UnrealToolsFramework::GetWorld();

	if (!world)
	{
		return UTF_E_WORLD_WAS_NOT_FOUND;
	}

	APlayerController* playerController = world->GetFirstPlayerController();

	if (!playerController)
	{
		return UTF_E_PLAYER_WAS_NOT_FOUND;
	}

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(playerController->GetPawn());

	if (!playerCharacter)
	{
		return UTF_E_PLAYER_WAS_NOT_FOUND;
	}

	UItemStashComponent* itemStash = playerCharacter->FindComponentByClass<UItemStashComponent>();

	if (!itemStash)
	{
		return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
	}

	equipmentSlotMap = itemStash->GetEquipmentSlots();

	return Result;
}

HRESULT GetLevelSettings(FLevelSettings& levelSettings)
{
	HRESULT Result = S_OK;

	UWorld* world = Microsoft::Internal::GamesTest::UnrealToolsFramework::GetWorld();

	if (!world)
	{
		return UTF_E_WORLD_WAS_NOT_FOUND;
	}

	UDungeonsGameInstance* instance = world->GetGameInstanceChecked<UDungeonsGameInstance>();

	if (!instance)
	{
		return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
	}

	if (!instance->Configuration.GetLevelSettings().IsSet())
	{
		return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
	}

	levelSettings = instance->Configuration.GetLevelSettings().GetValue();

	return S_OK;
}

static bool VerifyEquipmentSlotItem(UInventoryItemSlot* itemSlot)
{
	if (itemSlot != NULL && itemSlot->Item != NULL)
	{
		return true;
	}

	return false;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetCurrentTileName(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	UWorld* world = Microsoft::Internal::GamesTest::UnrealToolsFramework::GetWorld();

	if (!world)
	{
		return UTF_E_WORLD_WAS_NOT_FOUND;
	}

	APlayerController* playerController = world->GetFirstPlayerController();

	if (!playerController)
	{
		return UTF_E_PLAYER_WAS_NOT_FOUND;
	}

	const auto playerCharacter = Cast<APlayerCharacter>(playerController->GetCharacter());

	if (!playerCharacter)
	{
		return UTF_E_PLAYER_WAS_NOT_FOUND;
	}

	game::Game* game = actorquery::getGame(world);

	if (!game)
	{
		return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
	}

	const auto& tile = game->progress().current(*playerCharacter);

	HRCHK(Output.SetNamedStringValue(TEXT("TileName"), UTF8_TO_TCHAR(tile.tile().id().c_str())));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetEquippedItems(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	TMap<EEquipmentSlot, UInventoryItemSlot*> equipmentSlots;
	HRCHK(GetEquipmentSlots(equipmentSlots));

	TArray<const wchar_t*> hotBarSlots;
	FString hotbarItem1 = VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::HotbarSlot1]) ? equipmentSlots[EEquipmentSlot::HotbarSlot1]->Item->Item.GetItemId().ToString() : TEXT("Unset");
	FString hotbarItem2 = VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::HotbarSlot2]) ? equipmentSlots[EEquipmentSlot::HotbarSlot2]->Item->Item.GetItemId().ToString() : TEXT("Unset");
	FString hotbarItem3 = VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::HotbarSlot3]) ? equipmentSlots[EEquipmentSlot::HotbarSlot3]->Item->Item.GetItemId().ToString() : TEXT("Unset");
	hotBarSlots.Add(*hotbarItem1);
	hotBarSlots.Add(*hotbarItem2);
	hotBarSlots.Add(*hotbarItem3);

	HRCHK(Output.SetNamedStringCollection(TEXT("HotBarSlots"), hotBarSlots.GetData(), hotBarSlots.Num()));
	HRCHK(Output.SetNamedStringValue(TEXT("MeleeGear"), VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::MeleeGear]) ? *equipmentSlots[EEquipmentSlot::MeleeGear]->Item->Item.GetItemId().ToString() : TEXT("Unset")));
	HRCHK(Output.SetNamedStringValue(TEXT("RangedGear"), VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::RangedGear]) ? *equipmentSlots[EEquipmentSlot::RangedGear]->Item->Item.GetItemId().ToString() : TEXT("Unset")));
	HRCHK(Output.SetNamedStringValue(TEXT("ArmorGear"), VerifyEquipmentSlotItem(equipmentSlots[EEquipmentSlot::ArmorGear]) ? *equipmentSlots[EEquipmentSlot::ArmorGear]->Item->Item.GetItemId().ToString() : TEXT("Unset")));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetMapName(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;
	FLevelSettings levelSettings;
	HRCHK(GetLevelSettings(levelSettings));

	HRCHK(Output.SetNamedStringValue(TEXT("MapName"), *GetEnumValueToStringStripped(levelSettings.getLevelName())));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetDifficulty(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;
	FLevelSettings levelSettings;
	HRCHK(GetLevelSettings(levelSettings));

	HRCHK(Output.SetNamedStringValue(TEXT("Difficulty"), *GetEnumValueToStringStripped(levelSettings.getDifficulty())));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetThreatLevel(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;
	FLevelSettings levelSettings;
	HRCHK(GetLevelSettings(levelSettings));

	HRCHK(Output.SetNamedStringValue(TEXT("ThreatLevel"), *GetEnumValueToStringStripped(levelSettings.getThreatLevel())));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetMapSeed(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;
	FLevelSettings levelSettings;
	HRCHK(GetLevelSettings(levelSettings));

	HRCHK(Output.SetNamedValue(TEXT("Seed"), levelSettings.getSeed()));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetLocLanguage(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString languageString = FInternationalization::Get().GetCurrentLanguage()->GetName();

	HRCHK(Output.SetNamedStringValue(TEXT("Language"), *languageString));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetEquippedEnchantments(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	// Get which equipment slot to get the enchantments for.
	uint8 numericEquipmentSlot = -1;

	HRCHK(Input.GetNamedValue(TEXT("EquipmentSlot"), &numericEquipmentSlot));

	if (numericEquipmentSlot >= (uint8)EEquipmentSlot::Invalid)
	{
		RETURN_INVALIDPARAM("Invalid equipment slot.");
	}

	EEquipmentSlot equipmentSlot = EEquipmentSlot(numericEquipmentSlot);

	TMap<EEquipmentSlot, UInventoryItemSlot*> equipmentSlots;
	HRCHK(GetEquipmentSlots(equipmentSlots));

	if (!equipmentSlots.Contains(equipmentSlot) || !VerifyEquipmentSlotItem(equipmentSlots[equipmentSlot]))
	{
		return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
	}

	// Grab the enchantments and put them into a collection in the output archive.
	TArray<Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive> enchantmentDataItems;
	enchantmentDataItems.Reserve(equipmentSlots[equipmentSlot]->Item->Item.Enchantments.Num());

	for (const FEnchantmentData& enchantmentData : equipmentSlots[equipmentSlot]->Item->Item.Enchantments)
	{
		Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive EnchantmentArchive;

		HRCHK(EnchantmentArchive.SetNamedStringValue(TEXT("Name"), *GetEnumValueToStringStripped(enchantmentData.TypeID)));
		HRCHK(EnchantmentArchive.SetNamedValue(TEXT("Level"), enchantmentData.Level));

		enchantmentDataItems.Add(EnchantmentArchive);
	}

	HRCHK(Output.SetNamedCollection(TEXT("Enchantments"), enchantmentDataItems.GetData(), enchantmentDataItems.Num()));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetBuildNumber(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	HRCHK(Output.SetNamedStringValue(TEXT("BuildNumber"), *UGameVersion::BuildVersion()));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetOS(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString osVersion = FPlatformMisc::GetOSVersion();

	HRCHK(Output.SetNamedStringValue(TEXT("OSVersion"), *osVersion));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetGPU(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	HRCHK(Output.SetNamedStringValue(TEXT("GPU"), *GRHIAdapterName));

	HRCHK(Output.SetNamedStringValue(TEXT("GraphicsDriverVersion"), *GRHIAdapterUserDriverVersion));

	FString gpuVendor = TEXT("");

	if (IsRHIDeviceAMD())
	{
		gpuVendor = TEXT("AMD");
	}
	else if (IsRHIDeviceIntel())
	{
		gpuVendor = TEXT("Intel");
	}
	else if (IsRHIDeviceNVIDIA())
	{
		gpuVendor = TEXT("NVIDIA");
	}

	HRCHK(Output.SetNamedStringValue(TEXT("GPUVendor"), *gpuVendor));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetCPU(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	FString CPU;

	CPU = FPlatformMisc::GetCPUBrand();
	if (CPU.Equals(TEXT("GenericCPUBrand")))
	{
		CPU.Empty();
	}

	HRCHK(Output.SetNamedStringValue(TEXT("CPU"), *CPU));

	HRCHK(Output.SetNamedValue(TEXT("CPUCount"), FPlatformMisc::NumberOfCores()));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetRAM(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	wchar_t ram[60];
	FPlatformMemoryConstants memoryConstants = FPlatformMemory::GetConstants();
	_ui64tow_s(memoryConstants.TotalPhysical / (1024 * 1024), ram, _countof(ram), 10);

	HRCHK(Output.SetNamedStringValue(TEXT("RAM"), ram));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetVRAM(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	wchar_t vram[60];
	FPlatformMemoryConstants memoryConstants = FPlatformMemory::GetConstants();
	_ui64tow_s(memoryConstants.TotalVirtual / (1024 * 1024), vram, _countof(vram), 10);

	HRCHK(Output.SetNamedStringValue(TEXT("VRAM"), vram));

	return Result;
}

static Microsoft::Internal::GamesTest::Rpc::Server::RpcHookCode GetScreenResolution(Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Input, Microsoft::Internal::GamesTest::Rpc::Server::RpcArchive& Output)
{
	HRESULT Result = S_OK;

	wchar_t screenResolution[60];
	FDisplayMetrics displayMetrics;
	FDisplayMetrics::GetDisplayMetrics(displayMetrics);
	swprintf_s(screenResolution, _countof(screenResolution), L"%d x %d", displayMetrics.PrimaryDisplayWidth, displayMetrics.PrimaryDisplayHeight);

	HRCHK(Output.SetNamedStringValue(TEXT("ScreenResolution"), screenResolution));

	return Result;
}

void RegisterGameHooks()
{
#ifdef ENABLE_BOTAUTOMATION
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("AddAutomation"), AddAutomation);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("RemoveAutomation"), RemoveAutomation);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("ClearActions"), ClearActions);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("KeyPressAction"), KeyPressAction);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("OnFunctionAction"), OnFunctionAction);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("MoveToLocationAction"), MoveToLocationAction);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("MoveToActorAction"), MoveToActorAction);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("PlayerAttackMob"), PlayerAttackMob);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("PlayerAttackRadius"), PlayerAttackRadius);
#endif // ENABLE_BOTAUTOMATION

	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("SetGamepadActive"), SetGamepadActive);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetNextTileDoor"), GetNextTileDoor);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetNextDoor"), GetNextDoor);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetNextDoorIndicatorDoor"), GetNextDoorIndicatorDoor);

	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetCurrentTileName"), GetCurrentTileName);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetEquippedItems"), GetEquippedItems);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetEquippedEnchantments"), GetEquippedEnchantments);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetLocLanguage"), GetLocLanguage);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetBuildNumber"), GetBuildNumber);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetOS"), GetOS);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetGPU"), GetGPU);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetCPU"), GetCPU);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetRAM"), GetRAM);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetVRAM"), GetVRAM);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetScreenResolution"), GetScreenResolution);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetMapName"), GetMapName);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetDifficulty"), GetDifficulty);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetThreatLevel"), GetThreatLevel);
	Microsoft::Internal::GamesTest::UnrealToolsFramework::FUTFHookManager::RegisterGameHook(TEXT("GetMapSeed"), GetMapSeed);
}

#endif // ENABLE_GAMESTEST_RPC
