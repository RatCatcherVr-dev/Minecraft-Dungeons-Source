#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "CommonTypes.h"
#include "PlayerCharacter.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/CommandConsoleCommands.h"
#include "game/Conversion.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/DamageSelfGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/PermanentInvisibilityEffect.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/MissionSelectorComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/component/TeleportComponent.h"
#include "game/component/OxygenComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/item/instance/BootsOfSwiftnessInstance.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGeneratorLooterState.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/InventoryItemData.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/merchant/MerchantDefs.h"
#include "game/merchant/type/VillageMerchant.h"
#include "game/merchant/MerchantDef.h"
#include "game/util/ActorQuery.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/util/DungeonsTravelUtil.h"
#include "game/skins/SkinsUtil.h"
#include "lovika/LovikaLevelActor.h"
#include "util/EnumUtil.h"
#include <AbilitySystemComponent.h>
#include "game/component/CharacterSerializeComponent.h"
#include "game/component/CharacterLazySaveComponent.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/Texture2D.h"
#include "Sound/SoundWave.h"
#include "Engine/Texture.h"
#include "Engine/TextureDefines.h"
#include "Engine/LocalPlayer.h"
#include "CoreOnline.h"
#include "StatTracker.h"
#include "game/component/WalletComponent.h"
#include "PlayerConsoleCommands.h"
#include "DungeonsUserManagement.h"

namespace {
	FName BootsOfSwiftnessEffectDuration(TEXT("BootsOfSwiftnessDuration"));
	FName BootsOfSwiftnessEffectMagnitude(TEXT("BootsOfSwiftnessMagnitude"));
}

static void ChangeSpeed(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.Speed", world, commands))
		return;

	if (!commands.Num()) {
		out.Log(TEXT("Not enough arguments passed to modify speed."));
		return;
	}

	float playerSpeed = 1.0f;
	float effectDuration = 2000.0f;
	if (!commands[0].IsNumeric()) {
		out.Log(TEXT("Second argument must be a number (if present)"));
		return;
	}
	else {
		playerSpeed = FCString::Atof(*commands[0]);
	}

	for (APlayerCharacter* playerOwner : GetAllPlayerCharacters(world, commands))
	{
		
		TSubclassOf<UBootsOfSwiftnessGameplayEffect> Effect = UBootsOfSwiftnessGameplayEffect::StaticClass();
		auto abilitySystem = playerOwner->GetAbilitySystemComponent();
		if (playerSpeed == 0.0f) {
			abilitySystem->RemoveActiveGameplayEffectBySourceEffect(Effect, abilitySystem);
			return;
		}

		float ItemPower = 1.0f;
		float SpeedBoostAmount = 1.6f;

		FGameplayEffectSpec spec(Cast<UBootsOfSwiftnessGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), ItemPower);
		spec.SetSetByCallerMagnitude(effects::DurationName, effectDuration);
		spec.SetSetByCallerMagnitude(::BootsOfSwiftnessEffectMagnitude, playerSpeed * (SpeedBoostAmount + (SpeedBoostAmount * (ItemPower * 0.1f))));
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	}

}

void SetXpFor(APlayerCharacter* character, int xp) {
	auto serializer = character->GetCharacterSerializeComponent();
	if (!serializer) {
		return;
	}
	if (auto* experience = character->FindComponentByClass<UPlayerExperienceComponent>()) {
		experience->AddXP_OnlyFromServer(FVector::ZeroVector, -experience->CurrentXp());
		experience->AddXP_OnlyFromServer(FVector::ZeroVector, xp);
	}
	serializer->SetXP(xp);

	if (auto* itemStash = character->FindComponentByClass<UItemStashComponent>()) {
		itemStash->DeserializeSaveState();
		itemStash->RefreshCachedPoints();
	}
}

static void DoMakePlayerInvisible(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num()) {
		if (HandleOnServer("Dungeons.Player.Invisibility", world, commands))
			return;

		bool makeInvisible = commands[0].ToBool();

		for (auto player : GetAllPlayerCharacters(world, commands)) {
			auto abilitySystemComponent = player->GetAbilitySystemComponent();

			if (makeInvisible) {
				auto spec = effects::CreateGameplayEffectSpec<UPermanentInvisibilityEffect>(abilitySystemComponent);
				abilitySystemComponent->ApplyGameplayEffectSpecToSelf(spec);
			}
			else {
				FGameplayEffectQuery q;
				q.EffectDefinition = UPermanentInvisibilityEffect::StaticClass();
				abilitySystemComponent->RemoveActiveEffects(q);
			}
		}
	}
}

static void DoAddXP(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.AddXP", world, commands)) {
		return;
	}

	const auto xpToAdd = ArgAsInt(commands, 0);
	if (!xpToAdd) {
		out.Log(TEXT("Missing XP-to-add argument"));
		return;
	}

	for (APlayerCharacter* playerOwner : GetAllPlayerCharacters(world, commands)) {
		UActorComponent* actorComponent = playerOwner->GetComponentByClass(UPlayerExperienceComponent::StaticClass());
		UPlayerExperienceComponent* playerExperienceComp = Cast<UPlayerExperienceComponent>(actorComponent);
		playerExperienceComp->AddXP_OnlyFromServer(FVector::ZeroVector, xpToAdd.GetValue());
	}
}

static void DoDilateTime(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.TimeDilation", world, commands))
		return;

	if (!commands.Num()) {
		out.Log(TEXT("Not enough arguments passed to modify speed."));
		return;
	}

	if (!commands[0].IsNumeric()) {
		out.Log(TEXT("Need time dilation float value"));
		return;
	}
	UGameplayStatics::SetGlobalTimeDilation(world, FCString::Atof(*commands[0]));
}

static const FAutoConsoleCommand TimeDilation(TEXT("Dungeons.Player.TimeDilation")
	, TEXT("Dilates Time")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoDilateTime)
	, ECVF_Cheat);

static const FAutoConsoleCommand Speed(TEXT("Dungeons.Player.Speed")
	, TEXT("Permanently changes player's speed.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ChangeSpeed)
	, ECVF_Cheat);

static const FAutoConsoleCommand Invisibility(TEXT("Dungeons.Player.Invisibility")
	, TEXT("Permanently makes player invisible.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoMakePlayerInvisible)
	, ECVF_Cheat);


static const FAutoConsoleCommand AddXPCommand(TEXT("Dungeons.Player.AddXP")
	, TEXT("Permanently adds XP to player.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAddXP)
	, ECVF_Cheat);



static void DoRagdoll(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	for (auto player : GetAllPlayerCharacters(world)) {
		if (auto ragdoll = player->FindComponentByClass<URagdollOnDeathComponent>()) {
			ragdoll->Ragdoll();
		}
	}
}

static void DoUnragdoll(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	for (auto player : GetAllPlayerCharacters(world)) {
		if (auto ragdoll = player->FindComponentByClass<URagdollOnDeathComponent>()) {
			ragdoll->UnRagdoll();
		}
	}
}


static const FAutoConsoleCommand Ragdoll(TEXT("Dungeons.Player.Ragdoll")
	, TEXT("Makes player Ragdoll")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoRagdoll)
	, ECVF_Cheat);

static const FAutoConsoleCommand Unragdoll(TEXT("Dungeons.Player.Unragdoll")
	, TEXT("Makes player Unragdoll")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnragdoll)
	, ECVF_Cheat);


static void DoMoveTo(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	FVector location(FVector::ZeroVector);

	if (commands.Num() >= 1 && commands[0].IsNumeric()) {
		location.X = FCString::Atoi(*commands[0]);
	}
	if (commands.Num() >= 2 && commands[1].IsNumeric()) {
		location.Y = FCString::Atoi(*commands[1]);
	}
	if (commands.Num() == 3 && commands[2].IsNumeric()) {
		location.Z = FCString::Atoi(*commands[2]);
	}

	for (APlayerCharacter* player : GetAllPlayerCharacters(world, commands))
	{
		ABasePlayerController* controller = player->GetPlayerController();
		controller->MoveToLocation(location);
	}
}

static const FAutoConsoleCommand MovePlayerTo(TEXT("Dungeons.Player.MoveTo")
	, TEXT("Moves player to location specified.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoMoveTo)
	, ECVF_Cheat);

static void DoTeleportBackwards(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.TeleportBackwards", world, commands))
		return;

	for (APlayerCharacter* character : GetAllPlayerCharacters(world, commands)) {
		AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(world);

		int num_doors = 1;
		if (commands.Num() == 1 && commands[0].IsNumeric()) {
			num_doors = FCString::Atoi(*commands[0]);
		}

		for (int i = 0; i < num_doors; i++) {
			FVector pos;
			if (gameBP->GetPreviousTileDoor(character, pos)) {
				character->GetTeleportComponent()->TeleportToPositionWithoutPathfinding(pos);
			}
		}
	}
}

static const FAutoConsoleCommand TeleportBackwards(TEXT("Dungeons.Player.TeleportBackwards")
	, TEXT("Teleports player to the next door {num_doors_to_skip}")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoTeleportBackwards)
	, ECVF_Cheat);

static void DoTeleportForward(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.TeleportForward", world, commands))
		return;

	for (APlayerCharacter* character : GetAllPlayerCharacters(world, commands)) {
		AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(world);

		int num_doors = 1;
		if (commands.Num() == 1 && commands[0].IsNumeric()) {
			num_doors = FCString::Atoi(*commands[0]);
		}

		for (int i = 0; i < num_doors; i++) {
			FVector pos;
			if (gameBP->GetNextTileDoor(character, pos)) {
				character->GetTeleportComponent()->TeleportToPositionWithoutPathfinding(pos);
			}
		}
	}
}

static const FAutoConsoleCommand TeleportForward(TEXT("Dungeons.Player.TeleportForward")
	, TEXT("Teleports player to the next door {num_doors_to_skip}")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoTeleportForward)
	, ECVF_Cheat);

static void DoSkipObjectives(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer(TEXT("Dungeons.Player.SkipObjectives"), world, commands))
		return;

	APlayerCharacter* character = GetPlayerCharacter(world, commands);
	if (character) {
		AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(world);

		int num_objectives = 1;
		if (commands.Num() == 1 && commands[0].IsNumeric()) {
			num_objectives = FCString::Atoi(*commands[0]);
		}

		for (int i = 0; i < num_objectives; i++) {

			bool success = false;
			auto locations = gameBP->GetObjectiveLocations().Locations;
			if (locations.Num() > 0) {
				for (auto loc : locations) {
					if (character->GetTeleportComponent()->TeleportToPositionWithoutPathfinding(loc)) {
						success = true;
						break;
					}
				}
			}

			if (!success) {
				FVector pos;
				if (gameBP->GetNextTileDoor(character, pos)) {
					character->GetTeleportComponent()->TeleportToPositionWithoutPathfinding(pos);
				}
				else if (gameBP->GetNextDoor(character, pos)) {
					character->GetTeleportComponent()->TeleportToPositionWithoutPathfinding(pos);
				}
			}

			gameBP->ForceCompleteCurrentObjective();
		}


	}
}

static const FAutoConsoleCommand SkipObjectives(TEXT("Dungeons.Player.SkipObjectives")
	, TEXT("Skips the current objective {num_objectives_to_skip}")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSkipObjectives)
	, ECVF_Cheat);


static void DoUnlockBonusMission(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto mpc = GetPlayerComponent<UMissionProgressComponent>(world)) {
		if (commands.Num() > 0) {
			TOptional<ELevelNames> level = EnumValueFromString(ELevelNames, commands[0]);
#if !UE_BUILD_SHIPPING
			if (level) {
				mpc->UnlockBonusMission(level.GetValue());
			}
#endif
		}
	}
}
static const FAutoConsoleCommand UnlockBonusMission(TEXT("Dungeons.Player.UnlockBonusMission")
	, TEXT("Unlocks bonus mission")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnlockBonusMission)
	, ECVF_Cheat);

static void DoUnlockAllMissions(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto mpc = GetPlayerComponent<UMissionProgressComponent>(world)) {
		bool unlock = true;
		if (commands.Num() > 0) {
			unlock = FCString::ToBool(*commands[0]);
		}
	#if !UE_BUILD_SHIPPING
		mpc->UnlockAllMissions(unlock);
	#endif
	}
}
static const FAutoConsoleCommand UnlockAllMissions(TEXT("Dungeons.Player.UnlockAllMissions")
	, TEXT("Unlocks all missions in the mission selector")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnlockAllMissions)
	, ECVF_Cheat);



static void DoUnlockAllDLC(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto mpc = GetPlayerComponent<UMissionProgressComponent>(world)) {
		bool unlock = true;
		if (commands.Num() > 0) {
			unlock = FCString::ToBool(*commands[0]);
		}
	#if !UE_BUILD_SHIPPING
		USettingsBlueprintFunctionLibrary::UnlockAllDLC();
		mpc->UnlockAllDLC(unlock);

		for (auto* player : InstanceTracker<APlayerCharacter>::GetList(world)) {
			if (player && player->IsLocallyControlled()) {
				player->GetCharacterSerializeComponent()->AddUnlockKey("EndOwner");
			}
		}
	#endif
	}
}
static const FAutoConsoleCommand UnlockAllDLC(TEXT("Dungeons.Player.UnlockAllDLC")
	, TEXT("Unlocks all dlc in the mission selector")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnlockAllDLC)
	, ECVF_Cheat);



static void DoUnlockAllRealms(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto mpc = GetPlayerComponent<UMissionProgressComponent>(world)) {
		bool unlock = true;
		if (commands.Num() > 0) {
			unlock = FCString::ToBool(*commands[0]);
		}
	#if !UE_BUILD_SHIPPING		
		mpc->UnlockAllRealms(unlock);
	#endif
	}
}

static const FAutoConsoleCommand UnlockAllRealms(TEXT("Dungeons.Player.UnlockAllRealms")
	, TEXT("Unlocks all realms in the mission selector")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnlockAllRealms)
	, ECVF_Cheat);


static const FAutoConsoleCommand UnlockAllProgressCommand = MakeRedirectCommand(
	"Dungeons.Player.UnlockAllProgress",
	"Unlocks all progress in the mission selector",
	"Dungeons.Command.RunCsv Dungeons.Player.UnlockAllMissions 1,Dungeons.Player.UnlockAllRealms 1,Dungeons.Player.UnlockAllDLC 1,Dungeons.Mission.Unlock deserttemple 3 7 20",
	ECVF_Cheat,
	true);

static void DoSuicide(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer(TEXT("Dungeons.Player.Suicide"), world, args))
		return;

	for (APlayerCharacter* character : GetAllPlayerCharacters(world, args)) {
		character->Kill();
	}
}

static const FAutoConsoleCommand Suicide(TEXT("Dungeons.Player.Suicide")
	, TEXT("Kills the current player character")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSuicide)
	, ECVF_Cheat);

static void DoSetSkin(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer(TEXT("Dungeons.Player.Skin"), world, args))
		return;

	bool calledCorrectly = args.Num() >= 1;
	if (!calledCorrectly) {
		out.Log(TEXT("Fail: you must call skin with an Id (string) from The available skin ids:"));
		for (auto skinName : USkinsUtil::GetSkinIds(world)) {
			out.Log(skinName.ToString());
		}
		return;
	}
	FName id = FName(*args[0]);
	if (id.IsNone()) {
		out.Log(TEXT("Fail: Arg 1 is not a valid skin id, please select from the following:"));
		for (auto skinName : USkinsUtil::GetSkinIds(world)) {
			out.Log(skinName.ToString());
		}
		return;
	}
	for (APlayerCharacter* pc : GetAllPlayerCharacters(world, args)) {
		pc->GetPlayerAvatarComponent()->SelectSkin(id, FEntitlement());
	}
}

static const FAutoConsoleCommand Skin(TEXT("Dungeons.Player.Skin")
	, TEXT("Set a skin (number 0-MaxSkins) (optional: playerNumber)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetSkin)
	, ECVF_Cheat);


static void DoSetHealth(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.SetHealth", world, args))
		return;
	bool hasArgument = args.Num() >= 1 && args[0].IsNumeric();
	if (!hasArgument) {
		out.Log(TEXT("Argument 0 is not present or numeric."));
		return;
	}

	float health = FCString::Atof(*args[0]);

	for (APlayerCharacter* character : GetAllPlayerCharacters(world, args)) {
		character->GetAbilitySystemComponent()->SetNumericAttributeBase(UHealthAttributeSet::HealthAttribute(), health);
	}
}

static const FAutoConsoleCommand SetHealth(TEXT("Dungeons.Player.SetHealth")
	, TEXT("Sets health of the player.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetHealth)
	, ECVF_Cheat);

static void DoWin(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (const auto game = actorquery::getGame(world)) {
		game->forceEndGame();
	}
}

static void DoSave(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {

	const auto* instance = world->GetGameInstance<UDungeonsGameInstance>();

	//D11.KS - For now use the initial player for saves.
	int32 initialUserControllerId = instance->GetUserManager()->GetInitialUserSystemId();

	instance->ReadGlobalSaveState(initialUserControllerId)->Save(initialUserControllerId);
}

static void DoSaveCharacter(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (auto* player = GetPlayerCharacter(world)) {
		if (auto* controller = player->GetPlayerController()) {
			const auto startTime = FPlatformTime::Seconds();
			controller->SaveCharacterData_OnlyIfValidSaveDataNum();
			const auto durationMs = 1000 * (FPlatformTime::Seconds() - startTime);
			out.Logf(TEXT("Saved character data. Duration: %.3f ms"), durationMs);
		}
	}
}

static void DoTeleportTo(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Player.TeleportTo", world, args))
		return;

	int playernumber = 1;
	if (args.Num() >= 1 && args[0].IsNumeric()) {
		playernumber = FCString::Atoi(*args[0]);
	}

	auto players = actorquery::getGame(world)->getPlayers();
	if (playernumber >= players.Num())
	{
		out.Log("bad player number! there aren't that many players in the game!");
		return;
	}

	for (APlayerCharacter* player : GetAllPlayerCharacters(world, args)) {
		TWeakObjectPtr<APlayerCharacter> target = players[playernumber];
		player->TeleportToFriend(target.Get());
		//player->Server_TeleportPlayerTo(playerState->UniqueId);
	}
}

static const FAutoConsoleCommand TeleportTo(TEXT("Dungeons.Player.TeleportTo")
	, TEXT("Teleports to selected player (index)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoTeleportTo)
	, ECVF_Cheat);

static const FAutoConsoleCommand Win(TEXT("Dungeons.Player.Win")
	, TEXT("Completed all objectives on the current level")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoWin)
	, ECVF_Cheat);

static const FAutoConsoleCommand Save(TEXT("Dungeons.Player.Save")
	, TEXT("Force trigger a save of global data.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSave)
	, ECVF_Cheat);

static const FAutoConsoleCommand SaveCharacter(TEXT("Dungeons.Player.SaveCharacter")
	, TEXT("Force trigger a save of character data.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSaveCharacter)
	, ECVF_Cheat);


static void DoSetEnchantment(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 0) {
		out.Log(TEXT("Not enough arguments passed to set enchantment."));
		return;
	}

	if (HandleOnServer("Dungeons.Player.SetEnchantment", world, args))
		return;

	for (auto player : GetAllPlayerCharacters(world, args)) {
		if (auto enchantmentComponent = player->FindComponentByClass<UEnchantmentComponent>()) {

			TOptional<EEnchantmentTypeID> enchantmentIdOptional = EnumValueFromString(EEnchantmentTypeID, args[0]);
			if (!enchantmentIdOptional.IsSet()) {
				out.Log(TEXT("Argument 0 is not a valid EEnchantmentTypeID"));
				return;
			}

			EEnchantmentTypeID enchantmentId = enchantmentIdOptional.Get(EEnchantmentTypeID::Unset);

			auto enchantmentType = game::enchantment::type::getEnchantmentType(enchantmentId);
			auto category = enchantmentType.getEnchantmentCategory();

			int level = args.Num() > 1 ? Math::clamp(FCString::Atoi(*args[1]), 1, 3) : 1;
			FEnchantmentData enchantmentData = { enchantmentId, level };
			TArray<FEnchantmentData> enchantmentArray;
			enchantmentArray.Add(enchantmentData);

			enchantmentComponent->AddEnchantments(enchantmentArray);

			for (auto entry : enchantmentArray) {
				if (auto enchantment = enchantmentComponent->GetEnchantment(entry.TypeID)) {
					enchantment->bAlwaysTrigger = true;
				}
			}
		}
	}
}

static const FAutoConsoleCommand SetEnchantment(TEXT("Dungeons.Player.SetEnchantment")
	, TEXT("Force sets a specific Enchantment in the correct category.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetEnchantment)
	, ECVF_Cheat);

static void DoAddHealthOrDamage(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (args.Num() <= 0) {
		out.Log(TEXT("Not enough arguments passed to add health."));
	}

	if (HandleOnServer("Dungeons.Player.AddHealthOrDamage", world, args))
		return;

	for (auto character : GetAllPlayerCharacters(world, args))
	{
		if (auto healthComponent = character->FindComponentByClass<UHealthComponent>()) {
			const int health = FCString::Atoi(*args[0]);
			const auto& characterLocation = character->GetActorLocation();
			const auto characterAbilitySystem = character->GetAbilitySystemComponent();

			if (health > 0) {
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHealSelfGameplayEffect>(characterAbilitySystem, effects::HealthName, health, character, character, characterLocation, 1.f);
				characterAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}
			else if (health < 0) {
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UDamageSelfGameplayEffect>(characterAbilitySystem, effects::HealthName, health, character, character, characterLocation, 1.f);
				characterAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);

			}
		}
	}
}

static const FAutoConsoleCommand AddHealthOrDamage(TEXT("Dungeons.Player.AddHealthOrDamage")
	, TEXT("Add health to player, positive values will apply heal, whereas negative values will apply damage instead")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAddHealthOrDamage)
	, ECVF_Cheat);

static void DoRevivePlayer(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {

	if (HandleOnServer("Dungeons.Player.Revive", world, args))
		return;

	for (auto character : GetAllPlayerCharacters(world, args))
	{
		if (auto healthComponent = character->FindComponentByClass<UHealthComponent>()) {
			const float health = args.Num() > 0 && args[0].IsNumeric() ? FCString::Atof(*args[0]) : healthComponent->GetMaximumHealth();

			const auto& characterLocation = character->GetActorLocation();
			float health_pct = healthComponent->GetMaximumHealth() > 0 ? health / healthComponent->GetMaximumHealth() : 1.0f;
			healthComponent->Revive(health_pct);

		}
	}
}

static const FAutoConsoleCommand Revive(TEXT("Dungeons.Player.Revive")
	, TEXT("Revive downed player, restoring health to full")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoRevivePlayer)
	, ECVF_Cheat);

static size_t countEnchantmentSlots(const FInventoryItemData& item) {
	int slots = 0;
	const int NumCols = 3;
	const int rows = 1 + (item.Enchantments.Num() - 1) / NumCols;

	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < NumCols; ++col) {
			const int i = row * NumCols + col;
			if (i < item.Enchantments.Num() && item.Enchantments[i].TypeID != EEnchantmentTypeID::Unset) {
				slots++;
				break;
			}
		}
	}
	return slots;
}

static void DoProgressToForCharacter(APlayerCharacter* character, const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	// already server-enabled
	auto missionDifficulty = [&]() {
		if (args.Num() <= 0) {
			auto level = actorquery::getFirstActor<ALovikaLevelActor>(world);
			if (level) {
				auto levelSettings = level->getGeneratedLevelSettings();
				if (levelSettings.IsSet()) {
					return TOptional<FMissionDifficulty>({ 
						levelSettings->getLevelName(),
						levelSettings->getDifficulty(),
						levelSettings->getThreatLevel(),
						levelSettings->getEndlessStruggle()
					});
				}
			}
			return TOptional<FMissionDifficulty>();
		}
		else {
			missionconsolehandlers::Unlock(args, world, out);
			return parseMissionDifficulty(args);
		}
	}();

	if (!missionDifficulty.IsSet()) {
		return;
	}

	FMissionDifficulty selectedMissionDifficulty = missionDifficulty.GetValue();
	game::FDifficulty unlockedDifficulty = game::FDifficulty(
		selectedMissionDifficulty.difficulty,
		selectedMissionDifficulty.threatLevel,
		EExtraChallenge::NoExtraChallenge,
		selectedMissionDifficulty.endlessStruggle
	);

	auto serializer = Cast<UCharacterLazySaveComponent>(character->GetCharacterSerializeComponent());
	if (!serializer || !serializer->HasProfile()) {
		return;
	}

	// Clear inventory, and hand out Equipment and Items at the mission@difficulty's power level
	serializer->GetItems().clear();

	const auto rareItemChance = FRareItemChance::GetChanceFromCategory(EItemRarityChanceCategory::ProgressCommand);

	const auto configForRarity = [](itemgen::Pred pred, const EItemRarity rarity) {
		const auto powerrangeCalc = game::item::drop::getPowerRangeCalc(rarity, game::item::generator::ItemSource::Drop);
		const auto enchantmentCalc = itemgen::enchantmentcalcs::Default();
		const auto probabilityCalc = game::item::drop::getProbabilityCalc(rarity);
		return itemgen::Config(std::move(pred), powerrangeCalc, enchantmentCalc, probabilityCalc);
	};

	//Give gear and items to the player.
	const auto ItemPowerSort = [](const FInventoryItemData& a, const FInventoryItemData& b) {
		//Arbitrarily count an enchantment slot as a small amount of item power
		float aQuality = a.ItemPower + 0.2f * static_cast<float>(a.GetNumEnchantmentSlots());
		float bQuality = b.ItemPower + 0.2f * static_cast<float>(b.GetNumEnchantmentSlots());
		return aQuality > bQuality;
	};

	auto looter = itemgen::looters::FromConsolePlayerCharacter(*character);
	EquipmentSlotter equipmentSlotter;
	size_t totalGearEnchantmentSlots = 0;
	int itemIndex = 0;


	//Give gear - they drop often during missions and are often salvaged only keeping the best.
	{
		//Give based on previous difficulty.
		const auto previousDifficulty = difficultyquery::getPreviousDifficulty(unlockedDifficulty);
		const bool IS_BEGINNING = game::FDifficulty::DEFAULT >= unlockedDifficulty;

		const game::FDifficulty difficulty = previousDifficulty.Get(unlockedDifficulty);
		game::Settings settings{ difficulty, 0, ELevelNames::Invalid };

		//Amount of gear loot you would collect per slot during missions.
		const int LOOT_PER_SLOT_PER_MAP = 3;
		const int MAPS_PLAYED_PER_DIFFICULTY = 2;
		const int LOOT_PER_SLOT_TO_ROLL = (IS_BEGINNING ? 1 : LOOT_PER_SLOT_PER_MAP * MAPS_PLAYED_PER_DIFFICULTY);

		//Keep only the best.
		const int GEAR_LOOT_PER_SLOT_TO_KEEP = (IS_BEGINNING ? 1 : 3);


		for (auto slotType : { ESlotType::MeleeWeapon, ESlotType::RangedWeapon, ESlotType::Armor }) {
			TArray<FInventoryItemData> RolledGear;

			for (auto i = 0; i < LOOT_PER_SLOT_TO_ROLL; ++i) {
				const auto randomizedItemRarity = rareItemChance.GetRandomizedItemRarity();
				const auto gearConfig = configForRarity(itemgen::predicates::SlotType(slotType), randomizedItemRarity);
				if (auto item = itemgen::generate(settings, looter, gearConfig)) {
					item.GetValue().Rarity = randomizedItemRarity;
					RolledGear.Add(item.GetValue());
				}
			}

			RolledGear.Sort(ItemPowerSort);

			for (auto i = 0; i < Math::min(RolledGear.Num(), GEAR_LOOT_PER_SLOT_TO_KEEP); ++i) {
				const auto item = RolledGear[i];
				if (i == 0) {
					//Only equip the best one.
					totalGearEnchantmentSlots += countEnchantmentSlots(item);
					serializer->GetItems().push_back(createItem(item, equipmentSlotter(slotType)));
				}
				else {
					serializer->GetItems().push_back(createItem(item, itemIndex));
					itemIndex++;
				}
			}
		}
	}

	{
		//Give items, they drop only at the end of missions and are only salvaged when you have significantly better duplicates.
		TArray<FInventoryItemData> RolledItems;


		if (auto mpc = character->FindComponentByClass<UMissionProgressComponent>()) {

			//If specified, just create a bunch of base items.
			const bool COMPLETED_FIRST_MISSION = mpc->IsMissionDifficultyCompleted(difficultyquery::First, ELevelNames::creeperwoods);
			const auto numItems = ArgAsInt(args, 3).Get(COMPLETED_FIRST_MISSION ? 1 : 0);
			if (numItems > 0) {
				auto previousDifficulty = difficultyquery::getPreviousDifficulty(unlockedDifficulty);
				game::Settings settings{ previousDifficulty.Get(unlockedDifficulty), 0, ELevelNames::Invalid };

				for (auto i = 0; i < numItems; ++i) {
					const auto randomizedItemRarity = rareItemChance.GetRandomizedItemRarity();
					const auto itemConfig = configForRarity(itemgen::predicates::Item(), randomizedItemRarity);
					if (auto item = itemgen::generate(settings, looter, itemConfig)) {
						item.GetValue().Rarity = randomizedItemRarity;
						RolledItems.Add(item.GetValue());
					}
				}
			}

			//Additionally, create items that sort of match simulated progress.
			const int NUM_MAPS_COMPLETED_PER_DIFFICULTY = 2;

			auto previousDifficulty = difficultyquery::getPreviousDifficulty(unlockedDifficulty);
			while (previousDifficulty.IsSet() && previousDifficulty.GetValue() >= game::FDifficulty::DEFAULT) {
				game::Settings settings{ previousDifficulty.GetValue(), 0, ELevelNames::Invalid };
				for (auto i = 0; i < NUM_MAPS_COMPLETED_PER_DIFFICULTY; ++i) {
					const auto randomizedItemRarity = rareItemChance.GetRandomizedItemRarity();
					const auto itemConfig = configForRarity(itemgen::predicates::Item(), randomizedItemRarity);
					if (auto item = itemgen::generate(settings, looter, itemConfig)) {
						item.GetValue().Rarity = randomizedItemRarity;
						RolledItems.Add(item.GetValue());
					}
				}
				previousDifficulty = difficultyquery::getPreviousDifficulty(previousDifficulty.GetValue());
			}
		}

		TMap<FItemId, FInventoryItemData> AddedItemTypes;
		const float DUPLICATED_ITEM_POWER_TRASH_THRESHOLD = 0.5f;

		RolledItems.Sort(ItemPowerSort);
		for (auto i = 0; i < RolledItems.Num(); ++i) {
			const auto item = RolledItems[i];

			if (AddedItemTypes.Contains(item.GetItemId()) && item.ItemPower < AddedItemTypes[item.GetItemId()].ItemPower - DUPLICATED_ITEM_POWER_TRASH_THRESHOLD) {
				//Salvage bad version of duplicate items.
				continue;
			}

			auto slot = equipmentSlotter(ESlotType::ActivePermanent);
			if (slot != EEquipmentSlot::Invalid) {

				serializer->GetItems().push_back(createItem(item, slot));
			}
			else {
				serializer->GetItems().push_back(createItem(item, itemIndex));
				itemIndex++;
			}
			if (!AddedItemTypes.Contains(item.GetItemId())) {
				AddedItemTypes.Add(item.GetItemId(), item);
			}
		}
	}


	// Calculate how much XP is needed to get enough enchantment points to invest, and hand it out (+ write to saveState)
	if (auto experience = character->FindComponentByClass<UPlayerExperienceComponent>()) {

		const float enchantmentPointsMultiplier = [&]() {
			switch (missionDifficulty->difficulty) {
			case EGameDifficulty::Difficulty_1:
				return FloatRange(0.25f, 6.0f).clampedLerp(threatquery::toMissionDifficultyFraction(missionDifficulty->threatLevel));
			case EGameDifficulty::Difficulty_2:
				return FloatRange(7.0f, 14.0f).clampedLerp(threatquery::toMissionDifficultyFraction(missionDifficulty->threatLevel));
			case EGameDifficulty::Difficulty_3:
				return FloatRange(15.0f, 27.0f).clampedLerp(threatquery::toMissionDifficultyFraction(missionDifficulty->threatLevel));
			default:
				return 0.0f;
			}
		}();

		const int experienceLevel = FMath::RoundToInt(totalGearEnchantmentSlots * enchantmentPointsMultiplier);
		const int targetXp = UDungeonsGameInstance::createCharacterLevelForLevel(experienceLevel).currentXp;

		experience->AddXP_OnlyFromServer(FVector::ZeroVector, -experience->CurrentXp());
		experience->AddXP_OnlyFromServer(FVector::ZeroVector, targetXp);
		serializer->SetXP(targetXp);
	}

	if (auto* itemStash = character->FindComponentByClass<UItemStashComponent>()) {
		itemStash->DeserializeSaveState();
		itemStash->RefreshCachedPoints();
	}

	if (auto equipment = character->GetEquipmentComponent()) {
		equipment->ResetDifficultyRecommendation();
	}	

	serializer->FlushCommand();
}

static void DoProgressTo(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (auto character : GetAllPlayerCharacters(world, args)) {
		DoProgressToForCharacter(character, args, world, out);
	}
}

static const FAutoConsoleCommand ProgressTo(TEXT("Dungeons.Player.Progress")
	, TEXT("Approximately progress the player to the given mission and difficulty (unlocking missions, equip gear, items and add XP)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoProgressTo)
	, ECVF_Cheat);

static void DoSetTotalPowerForCharacter(APlayerCharacter* character, const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto serializer = Cast<UCharacterLazySaveComponent>(character->GetCharacterSerializeComponent());
	if (!serializer || !serializer->HasProfile()) {
		return;
	}

	float visualItemPower = ArgAsFloat(args, 0).GetValue();

	if (auto* itemStash = character->FindComponentByClass<UItemStashComponent>()) {
		for (auto slot : itemStash->GetEquipmentSlots()) {
			if (auto itemSlot = slot.Value) {
				if (auto item = itemSlot->Item) {
					item->SetItemPower(UItemPowerUtil::GetItemPowerFromDisplayValue(visualItemPower));
				}
			}
		}

		itemStash->SerializeSaveState();
	}

	serializer->FlushCommand();
}

static void DoSetTotalPower(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (auto character : GetAllPlayerCharacters(world, args)) {
		DoSetTotalPowerForCharacter(character, args, world, out);
	}
}

static const FAutoConsoleCommand SetTotalPower(TEXT("Dungeons.Player.SetTotalPower")
	, TEXT("Sets the equipped gears and artifacts to a given power level")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSetTotalPower)
	, ECVF_Cheat);



void DoContinueHyperLevel(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto* player = GetPlayerCharacter(world, args);
	if (!player) {
		out.Logf(ELogVerbosity::Error, TEXT("No player character selected"));
		return;
	}
	if (auto* selector = GetPlayerControllerComponent<UMissionSelectorComponent>(player)) {
		if (!selector->TryContinueMission(player, ELevelNames::netherhypermission)) {
			out.Logf(ELogVerbosity::Error, TEXT("Could not continue hypermission"));
		}
	};	
}

static const FAutoConsoleCommand ContinueHyperMissionCommand(TEXT("Dungeons.Player.ContinueHyperMission")
	, TEXT("Continue a previously started hypermission.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoContinueHyperLevel)
	, ECVF_Cheat);


static void DoAddCurrencyForCharacter(APlayerCharacter* character, const TArray<FString>& args, UWorld* world, FOutputDevice& out, const FItemId& itemId) {
	float amount = ArgAsInt(args, 0).GetValue();
	if (auto* wallet = character->GetWalletComponent()) {
		wallet->ClientAdd(itemId, amount);
	}
}

static void DoAddCurrency(const TArray<FString>& args, UWorld* world, FOutputDevice& out, const FItemId& itemId) {
	for (auto character : GetAllPlayerCharacters(world, args)) {
		DoAddCurrencyForCharacter(character, args, world, out, itemId);
	}
}

static void DoAddEmeralds(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	DoAddCurrency(args, world, out, game::item::type::Emerald.getId());
}

static const FAutoConsoleCommand AddEmeralds(TEXT("Dungeons.Player.AddEmeralds")
	, TEXT("Adds the specified number of emeralds")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAddEmeralds)
	, ECVF_Cheat);


static void DoAddGold(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	DoAddCurrency(args, world, out, game::item::type::Gold.getId());
}

static const FAutoConsoleCommand AddGold(TEXT("Dungeons.Player.AddGold")
	, TEXT("Adds the specified number of gold")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoAddGold)
	, ECVF_Cheat);



static void BacktrackToMainPath(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (auto playerCharacter : GetAllPlayerCharacters(world, args)) {
		const auto game = actorquery::getGame(world);
		if (!game) {
			return;
		}
		const auto tile = game->tiles().getTile(*playerCharacter);
		if (!tile) {
			return;
		}
		if (const auto backTile = backtrackToMainPath(game->tiles(), *tile)) {
			const auto position = conversion::blockCenterXZToUe(backTile->door.position());
			playerCharacter->TeleportTo(position, FRotator{});
		}
	}
}

static const FAutoConsoleCommand BacktrackToMainPathCommand(TEXT("Dungeons.Level.BacktrackToMainPath")
	, TEXT("Teleports to the first main path position you reach when backtracking tiles")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&BacktrackToMainPath)
	, ECVF_Cheat);

static void StartLevel(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto missionDifficulty = parseMissionDifficulty(args);
	if (!missionDifficulty) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name, difficulty and threat level must be provided"));
		return;
	}

	EMapLoadType mapLoadType = EMapLoadType::StartIngameSession; // used to load e.g. E3 demo level. Can add switch for lobby level later if needed.

	auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	gi->BeginLoadingScreenWithTravel(levelsettingsutil::generateDevelopmentMissionSettings(
		world,
		missionDifficulty->mission,
		args[0],
		missionDifficulty->difficulty,
		missionDifficulty->threatLevel,
		missionDifficulty->endlessStruggle,
		{0, 0},
		ArgAsInt(args, 3).Get(Random().nextInt()))
		, mapLoadType
		, UDungeonsGameInstance::fDefaultFadeOutTime
		, UDungeonsGameInstance::fDefaultFadeInTime);

}

static void StartLevelOffline(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	const auto missionDifficulty = parseMissionDifficulty(args);
	if (!missionDifficulty) {
		out.Logf(ELogVerbosity::Error, TEXT("A level name, difficulty and threat level must be provided"));
		return;
	}

	EMapLoadType mapLoadType = EMapLoadType::OpenIngame;

	auto gi = Cast<UDungeonsGameInstance>(world->GetGameInstance());
	gi->BeginLoadingScreenWithTravel(levelsettingsutil::generateDevelopmentMissionSettings(
		world,
		missionDifficulty->mission,
		args[0],
		missionDifficulty->difficulty,
		missionDifficulty->threatLevel,
		{ArgAsInt(args, 4).Get(0)}, // D11.DB - Endless Struggle
		{0, 0}, // D11.PC - Use Emergent Difficulty
		ArgAsInt(args, 3).Get(Random().nextInt()))
		, mapLoadType
		, UDungeonsGameInstance::fDefaultFadeOutTime
		, UDungeonsGameInstance::fDefaultFadeInTime);

}

static void PrintLevelSeed(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (const auto* game = actorquery::getGame(world)) {
		out.Logf(ELogVerbosity::Display, TEXT("Level seed: %d"), game->settings().randomSeed);
	} else {
		out.Logf(ELogVerbosity::Error, TEXT("Not in a generated level"));
	}
}

static const FAutoConsoleCommand StartLevelCommand(TEXT("Dungeons.Level.Start")
	, TEXT("Start a mission with a given difficulty, threatlevel and an optional seed. (e.g. pumpkinpastures 2 5 1337")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&StartLevel)
	, ECVF_Cheat);

static const FAutoConsoleCommand StartLevelOfflineCommand(TEXT("Dungeons.Level.StartOffline")
	, TEXT("Start a mission with a given difficulty, threatlevel and an optional seed. (e.g. pumpkinpastures 2 5 1337")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&StartLevelOffline)
	, ECVF_Cheat);

static const FAutoConsoleCommand LevelSeedCommand(TEXT("Dungeons.Level.Seed")
	, TEXT("Prints the current level's seed")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&PrintLevelSeed)
	, ECVF_Cheat);

#if WITH_EDITORONLY_DATA
static bool ShouldTextureBeChanged(FString changeType, UTexture2D* texture)
{
	bool xPowerOf2 = (texture->GetSizeX() & (texture->GetSizeX() - 1)) == 0;
	bool yPowerOf2 = (texture->GetSizeY() & (texture->GetSizeY() - 1)) == 0;

	bool bPowerOfTwoChange = changeType.Equals("1") && (!xPowerOf2 || !yPowerOf2);

	TEnumAsByte<enum ETexturePowerOfTwoSetting::Type> powerOfTwoMode = texture->PowerOfTwoMode;
	bool bUndoPowerOfTwo = (changeType.Equals("0") && (powerOfTwoMode != ETexturePowerOfTwoSetting::None || texture->LODBias != 0));

	bool bAdjustCompression = texture->CompressionSettings == TextureCompressionSettings::TC_EditorIcon;

	return bPowerOfTwoChange || bUndoPowerOfTwo || bAdjustCompression;
}

UTexture2D* GetTextureFromAssetData(FAssetData& assetData)
{
	UTexture2D* texture = nullptr;

	FAssetData& AssetData = assetData;
	UObject* Asset = AssetData.GetAsset();
	if (Asset)
	{
		texture = Cast<UTexture2D>(Asset);
	}
	return texture;
}

static void GetAndFixNotPowerOf2Texures(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
#if WITH_EDITOR
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			if (UTexture2D* texture = GetTextureFromAssetData(AssetDatas[i]))
			{
				if (ShouldTextureBeChanged(args[0], texture))
				{
					texture->PowerOfTwoMode = args[0].Equals("1") ? ETexturePowerOfTwoSetting::PadToPowerOfTwo : ETexturePowerOfTwoSetting::None;
					texture->LODBias = 0;
					texture->CompressionSettings = TextureCompressionSettings::TC_Default;
					texture->NeverStream = false;

					UPackage *Package = AssetDatas[i].GetPackage();
					const FString& FileExtension = FPackageName::GetAssetPackageExtension();

					FString AssetPath = FPackageName::LongPackageNameToFilename(Package->GetName(), FileExtension);

					texture->UpdateResource();
					texture->MarkPackageDirty();

					bool bSuccess = UPackage::SavePackage(Package, texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetPath);

					UE_LOG(LogTemp, Warning, TEXT("Saved Package: %s"), bSuccess ? TEXT("True") : TEXT("False"));
				}
			}
		}
	}
#endif
}

static void GetNotPowerOf2Texures(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);


		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			if (UTexture2D* texture = GetTextureFromAssetData(AssetDatas[i]))
			{
				bool xPowerOf2 = (texture->GetSizeX() & (texture->GetSizeX() - 1)) == 0;
				bool yPowerOf2 = (texture->GetSizeY() & (texture->GetSizeY() - 1)) == 0;
				if (!xPowerOf2 || !yPowerOf2)
				{
					Assets.Add(AssetDatas[i].GetAsset());
				}
			}
		}
	}
}

static void GetTexturesInfo(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		TArray<UTexture2D*> AllTextures;
		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			if (UTexture2D* texture = GetTextureFromAssetData(AssetDatas[i]))
			{
				AllTextures.Add(texture);
			}
		}

		AllTextures.Sort([](UTexture2D& LHS, UTexture2D& RHS) { return LHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) > RHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal); });

		FString AllTexturesInfo;
		for (int i = 0; i < AllTextures.Num(); ++i)
		{
			AllTexturesInfo += AllTextures[i]->GetName() + "(" + AllTextures[i]->GetPathName() + ") " + FString::FromInt(AllTextures[i]->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal)) + "Kb \n";
		}

		FFileHelper::SaveStringToFile(AllTexturesInfo, *(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectSavedDir()) + "TexturesInfo/TexturesInfo.txt"));
	}
}

static void GetUserInterfaceTextures(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		TArray<UTexture2D*> AllTextures;
		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			if (UTexture2D* texture = GetTextureFromAssetData(AssetDatas[i]))
			{
				if (texture->CompressionSettings == TextureCompressionSettings::TC_EditorIcon)
				{
					AllTextures.Add(texture);
				}
			}
		}

		AllTextures.Sort([](UTexture2D& LHS, UTexture2D& RHS) { return LHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) > RHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal); });

		FString AllTexturesInfo;
		for (int i = 0; i < AllTextures.Num(); ++i)
		{
			AllTexturesInfo += AllTextures[i]->GetName() + "(" + AllTextures[i]->GetPathName() + ") " + FString::FromInt(AllTextures[i]->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal)) + "Kb \n";
		}

		FFileHelper::SaveStringToFile(AllTexturesInfo, *(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectSavedDir()) + "TexturesInfo/UserInterfaceTexturesInfo.txt"));
	}
}

static void GetAndSaveAllTextures(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UTexture2D::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		TArray<UTexture2D*> AllTextures;
		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			if (UTexture2D* texture = GetTextureFromAssetData(AssetDatas[i]))
			{
				AllTextures.Add(texture);
			}
		}

		AllTextures.Sort([](UTexture2D& LHS, UTexture2D& RHS) { return LHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal) > RHS.GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal); });

		FString AllTexturesInfo;
		for (int i = 0; i < AllTextures.Num(); ++i)
		{
			AllTexturesInfo += AllTextures[i]->GetName() + "(" + AllTextures[i]->GetPathName() + ") " + FString::FromInt(AllTextures[i]->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal)) + "Kb \n";
		}

		FFileHelper::SaveStringToFile(AllTexturesInfo, *(IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectSavedDir()) + "TexturesInfo/AllTexturesInfo.txt"));
	}
}

static const FAutoConsoleCommand GetAllTextures(TEXT("Dungeons.Memory.GetAllTexturesToFile")
	, TEXT("Gets all the textures and saves it to a txt file")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetAndSaveAllTextures)
	, ECVF_Cheat);

static const FAutoConsoleCommand FixNotPowerOf2Texures(TEXT("Dungeons.Memory.FixTextures")
	, TEXT("Fix the textures that are not power of two")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetAndFixNotPowerOf2Texures)
	, ECVF_Cheat);

static const FAutoConsoleCommand GetNotPowerOf2Textures(TEXT("Dungeons.Memory.GetBadTextures")
	, TEXT("Gets the textures that are not power of two")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetNotPowerOf2Texures)
	, ECVF_Cheat);

static const FAutoConsoleCommand GetAllTexturesInfo(TEXT("Dungeons.Project.GetTexturesInfo")
	, TEXT("Gets all the project textures info")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetTexturesInfo)
	, ECVF_Cheat);

static const FAutoConsoleCommand GetAllUserInterfaceTextures(TEXT("Dungeons.Project.GetUserInterfaceTextures")
	, TEXT("Gets all the project user interface textures info")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&GetUserInterfaceTextures)
	, ECVF_Cheat);
#endif

static void SetAllSoundsToStream(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	TArray<UObject*> Assets;

	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(USoundWave::StaticClass(), false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game");
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);


		UObject* Asset;

		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			FAssetData& AssetData = AssetDatas[i];
			Asset = AssetData.GetAsset();
			if (Asset)
			{
				USoundWave* soundWave = Cast<USoundWave>(Asset);
				if (soundWave)
				{
					soundWave->bStreaming = false;
					soundWave->CompressionQuality = 1;
					soundWave->SampleRateQuality = ESoundwaveSampleRateSettings::Low;

					UPackage *Package = AssetData.GetPackage();
					const FString& FileExtension = FPackageName::GetAssetPackageExtension();

					FString AssetPath = FPackageName::LongPackageNameToFilename(Package->GetName(), FileExtension);



					bool bSuccess = UPackage::SavePackage(Package, soundWave, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetPath);

				}
			}
		}
	}
}

static const FAutoConsoleCommand SetSoundsToStream(TEXT("Dungeons.Memory.SetAllSoundsToStream")
	, TEXT("Fix the textures that are not power of two")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetAllSoundsToStream)
	, ECVF_Cheat);

static void LocalPlayerChangedUser(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (args.Num() <= 0)
	{
		return;
	}
	world->GetGameInstance<UDungeonsGameInstance>()->OnControllerPairingChangedCommand(FCString::Atoi(*args[0]));
}

static const FAutoConsoleCommand LocalPlayerChangedToOtherUser(TEXT("Dungeons.LocalPlayer.LocalPlayerChangedUser")
	, TEXT("Used to simulate that a local player has changed user")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&LocalPlayerChangedUser)
	, ECVF_Cheat);


static void KickPlayerToMenu(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (args.Num() > 0)
	{
		world->GetGameInstance<UDungeonsGameInstance>()->KickBackPlayerToMenu((EBootMode)FCString::Atoi(*args[0]));
	}
}

static const FAutoConsoleCommand KickPlayerToMenuLikeSignedOut(TEXT("Dungeons.Player.GoToMenuLikeSignedOut")
	, TEXT("Used to simulate that the user signed out")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&KickPlayerToMenu)
	, ECVF_Cheat);

static void SimulateNetworkError(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	world->GetGameInstance<UDungeonsGameInstance>()->HandleNetworkError(ENetworkFailure::ConnectionLost, false);
}

static const FAutoConsoleCommand SimulateNetworkErrorCommand(TEXT("Dungeons.Player.SimulateNetworkError")
	, TEXT("Used to simulate that the user had a network error")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SimulateNetworkError)
	, ECVF_Cheat);

TAutoConsoleVariable<int32> CVarEnableDodge(
	TEXT("Dungeons.Experimental.EnableDodge"),
	1,
	TEXT("Enables secret dodge moves for the player using the 'SPACEBAR' key.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarPlayerRelativeDodgeRoll(
	TEXT("Dungeons.Player.PlayerRelativeDodgeRoll"),
	0,
	TEXT("Determines whether the right stick dodge roll is performed relative to the camera or the player\n")
	TEXT("0: Camera relative.\n")
	TEXT("1: Player relative.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarMeleeAutoWalk(
	TEXT("Dungeons.AutoTarget.AutoWalk"),
	0,
	TEXT("Enable/Disable automatic walking toward targets while holding the attack button.")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

TAutoConsoleVariable<int32> CVarPriorityTarget(
	TEXT("Dungeons.AutoTarget.PriorityTarget"),
	0,
	TEXT("Enable/Disable prioritisation of the last target attacked as long as it remains within attack range.")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

static void SetSetting(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (args.Num() > 1 && args[1].IsNumeric())
	{
		if (ABasePlayerController* pc = Cast<ABasePlayerController>(world->GetGameInstance()->GetFirstLocalPlayerController(world)))
		{
			USettingsBlueprintFunctionLibrary::SetSettingByName(args[0], FCString::Atoi(*args[1]), pc);
		}
	}
}

static const FAutoConsoleCommand SetSettingCommand(TEXT("Dungeons.Settings.Set")
	, TEXT("Sets save data")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&SetSetting)
	, ECVF_Cheat);

static void TeleportAfterDeath(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetGameInstance()->GetFirstLocalPlayerController(world))) {
		const auto teleportComponent = playerController->GetPawn()->FindComponentByClass<UTeleportComponent>();
		teleportComponent->TeleportAfterDeath();
	}
}

static const FAutoConsoleCommand TeleportAfterDeathCommand(TEXT("Dungeons.Player.TeleportAfterDeath")
	, TEXT("Teleports the player as if they died")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TeleportAfterDeath)
	, ECVF_Cheat);

static void TeleportOffset(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetGameInstance()->GetFirstLocalPlayerController(world))) {
		const auto pawn = playerController->GetPawn();
		const auto teleportComponent = pawn->FindComponentByClass<UTeleportComponent>();

		const auto offset = args.Num() > 1 && args[1].IsNumeric()
			? FCString::Atof(*args[0])
			: 500.f;

		const auto location = pawn->GetActorLocation() + pawn->GetActorForwardVector() * offset;

		DrawDebugSphere(world, location, 50.f, 16, FColor::White, false, 5.f);
		DrawDebugLine(world, location + FVector { 0.f, 0.f, 10000.f }, location - FVector { 0.f, 0.f, 10000.f }, FColor::White, false, 5.f);

		teleportComponent->TeleportToPosition(location, true);
	}
}

static const FAutoConsoleCommand TeleportOffsetCommand(TEXT("Dungeons.Player.TeleportOffset")
	, TEXT("Teleports the player forward")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&TeleportOffset)
	, ECVF_Cheat);

static void AddUnlockKey(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (args.Num() == 0) {
		return;
	}
	FString key = args[0];

	for (auto character : GetAllPlayerCharacters(world, args)) {
		if (auto serializer = character->GetCharacterSerializeComponent()) {
			if (serializer->AddUnlockKey(key)) {
				out.Logf(ELogVerbosity::Display, TEXT("Key added"));
			}
			else {
				out.Logf(ELogVerbosity::Error, TEXT("Key, '%s', already added"), *key);
			}
		}
	}
}

static const FAutoConsoleCommand AddUnlockKeyCommand(TEXT("Dungeons.Player.AddUnlockKey")
	, TEXT("Adds an unlock key")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&AddUnlockKey)
	, ECVF_Cheat);

static void RemoveUnlockKey(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (args.Num() == 0) {
		return;
	}
	FString key = args[0];

	for (auto character : GetAllPlayerCharacters(world, args)) {
		if (auto serializer = character->GetCharacterSerializeComponent()) {
			if (serializer->RemoveUnlockKey(key)) {
				out.Logf(ELogVerbosity::Display, TEXT("Key removed"));
			}
			else {
				out.Logf(ELogVerbosity::Error, TEXT("Key, '%s', didn't exist"), *key);
			}
		}
	}
}

static const FAutoConsoleCommand RemoveUnlockKeyCommand(TEXT("Dungeons.Player.RemoveUnlockKey")
	, TEXT("Removes an unlock key")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&RemoveUnlockKey)
	, ECVF_Cheat);

static void ListUnlockKeys(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	FString filterKey = "";
	if (args.Num() > 0) {
		filterKey = args[0];
	}

	for (auto character : GetAllPlayerCharacters(world, args)) {
		if (auto serializer = character->GetCharacterSerializeComponent()) {
			out.Logf(ELogVerbosity::Display, TEXT("Unlock keys matching '%s':"), *filterKey);

			for (auto&& key : serializer->GetUnlockKeys()) {
				if (filterKey == "" || key.Contains(filterKey, ESearchCase::IgnoreCase)) {
					out.Logf(ELogVerbosity::Display, TEXT("- %s"), *key);
				}
			}
		}
	}
}

static const FAutoConsoleCommand ListUnlockKeysCommand(TEXT("Dungeons.Player.ListUnlockKeys")
	, TEXT("Lists unlock keys, with an optional filter")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ListUnlockKeys)
	, ECVF_Cheat);


static void ResetEyesOfEnder(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	if (auto* player = GetPlayerCharacter(world)) {
		if (auto* controller = player->GetPlayerController()) {
			controller->GetCharacterSerializeComponent()->SetStrongholdData({});
		}
	}
}


static FAutoConsoleCommand ResetEyesOfEnderCommand(TEXT("Dungeons.Items.ResetEyesOfEnder")
	, TEXT("Reset ALL eye of ender progress")
	TEXT("\nUsage: Dungeons.Items.ResetEyesOfEnder")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&ResetEyesOfEnder)
	, ECVF_Cheat);


static void DoIceSlideDebugTextEnable(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
    if (HandleOnServer("Dungeons.Player.IceSlideDebugTextEnable", world, args))
    {
        return;
    }

    FString arg = "";
    if (args.Num() > 0) {
        arg = args[0];

        bool enable = true;
        if (arg == "0") {
            enable = false;
        }
        else if (arg != "1") {
            return;
        }

        for (auto player : GetAllPlayerCharacters(world)) {
            if (UStatTrackerComponent* tracker = player->GetStatTracker()) {
                tracker->showIceSlideDistanceDebugMessage = enable;
            }
        }
    }
    else {
        return;
    }
}

static const FAutoConsoleCommand IceSlideDebugTextEnable(TEXT("Dungeons.Player.IceSlideDebugTextEnable")
	, TEXT("Activates/deactivates debug text to show distance slid on ice in metres")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoIceSlideDebugTextEnable)
	, ECVF_Cheat);

static void DoFoodEatenDebugTextEnable(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (HandleOnServer("Dungeons.Player.FoodEatenDebugTextEnable", world, args))
	{
		return;
	}

	FString arg = "";
	if (args.Num() > 0) {
		arg = args[0];

		bool enable = true;
		if (arg == "0") {
			enable = false;
		}
		else if (arg != "1") {
			return;
		}

		for (auto player : GetAllPlayerCharacters(world)) {
			if (UStatTrackerComponent* tracker = player->GetStatTracker()) {
				tracker->showFoodEatenDebugMessage = enable;
			}
		}
	}
	else {
		return;
	}
}

static const FAutoConsoleCommand FoodEatenDebugTextEnable(TEXT("Dungeons.Player.FoodEatenDebugTextEnable")
	, TEXT("Activates/deactivates debug text to show how many pieces of food the player has eaten")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoFoodEatenDebugTextEnable)
	, ECVF_Cheat);

void DoUnlockAllMerchants(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	for (auto* character : GetAllPlayerCharacters(world, args)) {
		if (character->IsLocallyControlled()) {
			for (auto merchantClass : merchantdefs::getAllEnabledClasses()) {
				character->GetCharacterSerializeComponent()->AddUnlockKey(merchantClass->GetDefaultObject<UMerchantDef>()->GetUnlockProgressKey());
			}
		}
	}
}

static const FAutoConsoleCommand UnlockAllMerchants(TEXT("Dungeons.Player.Merchant.UnlockAll")
	, TEXT("Unlocks all merchant permanently.")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoUnlockAllMerchants)
	, ECVF_Cheat);

namespace merchantconsolehandlers {
	static void DoIncrementWinMissionHypermission(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
		merchantconsolehandlers::DoIncrementProgressStats(args, world, out, {
			EProgressStat::WIN_MISSIONS,
			EProgressStat::WIN_MISSIONS_DEFAULT,
			EProgressStat::WIN_HYPERMISSIONS
			});
	}

	static void DoIncrementWinMissionDefault(const TArray<FString>& args, UWorld* world, FOutputDevice& out){
		merchantconsolehandlers::DoIncrementProgressStats(args, world, out, {
			EProgressStat::WIN_MISSIONS,
			EProgressStat::WIN_MISSIONS_DEFAULT
			});
	}

	static void DoIncrementWinMissionAdventure(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
		merchantconsolehandlers::DoIncrementProgressStats(args, world, out, {
			EProgressStat::WIN_MISSIONS,
			EProgressStat::WIN_MISSIONS_DEFAULT,
			EProgressStat::WIN_MISSIONS_ADVENTURE
			});
	}

	static void DoIncrementWinMissionApocalypse(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
		merchantconsolehandlers::DoIncrementProgressStats(args, world, out, {
			EProgressStat::WIN_MISSIONS,
			EProgressStat::WIN_MISSIONS_DEFAULT,
			EProgressStat::WIN_MISSIONS_ADVENTURE,
			EProgressStat::WIN_MISSIONS_APOCALYPSE
			});
	}

	static void DoIncrementWinMissionApocalypsePlus(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
		merchantconsolehandlers::DoIncrementProgressStats(args, world, out, {
			EProgressStat::WIN_MISSIONS,
			EProgressStat::WIN_MISSIONS_DEFAULT,
			EProgressStat::WIN_MISSIONS_ADVENTURE,
			EProgressStat::WIN_MISSIONS_APOCALYPSE,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER0,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER1,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER2,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER3,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER4,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER5,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER6,
			EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER7
			});
	}
}

static const FAutoConsoleCommand IncrementWinMissionDefault(TEXT("Dungeons.Player.Merchant.IncrementWinMission.Default")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementWinMission.Default [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementWinMissionDefault)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementWinHyperMissionDefault(TEXT("Dungeons.Player.Merchant.IncrementWinMission.Hypermission")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementWinMission.Hypermission [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementWinMissionHypermission)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementWinMissionAdventure(TEXT("Dungeons.Player.Merchant.IncrementWinMission.Adventure")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementWinMission.Adventure [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementWinMissionAdventure)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementWinMissionApocalypse(TEXT("Dungeons.Player.Merchant.IncrementWinMission.Apocalypse")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementWinMission.Apocalypse [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementWinMissionApocalypse)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementWinMissionApocalypsePlus(TEXT("Dungeons.Player.Merchant.IncrementWinMission.ApocalypsePlus")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementWinMission.ApocalypsePlus [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementWinMissionApocalypsePlus)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementDefeatedEnchantedMobs(TEXT("Dungeons.Player.Merchant.IncrementDefeatedEnchantedMobs")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementDefeatedEnchantedMobs [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementProgressStat<EProgressStat::DEFEAT_ENCHANTED_MOBS>)
	, ECVF_Cheat);

static const FAutoConsoleCommand IncrementDefeatedEventMobs(TEXT("Dungeons.Player.Merchant.IncrementDefeatedEventMobs")
	, TEXT("Increments a player progress stat: Dungeons.Player.Merchant.IncrementDefeatedEventMobs [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementProgressStat<EProgressStat::DEFEAT_EVENT_MOBS>)
	, ECVF_Cheat);

namespace merchantconsolehandlers {
	void DoIncrementObjectiveTag(const TArray<FString>& args, UWorld* world, FOutputDevice& out, const std::string& objectiveTag) {
		auto count = ArgAsInt(args, 0).Get(1);
		for (auto character : GetAllPlayerCharacters(world, args)) {
			if (character->IsLocallyControlled()) {
				character->GetCharacterSerializeComponent()->IncrementObjectiveTag(objectiveTag, count);
			}
		}
	}

	void DoIncrementRescuedVillagers(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
		DoIncrementObjectiveTag(args, world, out, AVillageMerchant::RescueVillagerObjectiveTag);
	}
}

static const FAutoConsoleCommand IncrementRescuedVillagers(TEXT("Dungeons.Player.Merchant.IncrementRescuedVillagers")
	, TEXT("Increments a player progress stat: Dungeons.Player.IncrementRescuedVillagers [count]")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&merchantconsolehandlers::DoIncrementRescuedVillagers)
	, ECVF_Cheat);

static void DoOxygenToggle(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
    if (HandleOnServer("Dungeons.Player.EnableWaterBreathing", world, args)) {
        return;
    }

    FString arg = "";
    if (args.Num() > 0) {
        arg = args[0];

        bool enable = true;
        if (arg == "0") {
            enable = false;
        }
        else if (arg != "1") {
            return;
        }

        for (auto player : GetAllPlayerCharacters(world, args)) {
            if (auto oxygen = player->FindComponentByClass<UOxygenComponent>()) {
                oxygen->EnableWaterBreathing(enable);
            }
        }
    }
    else {
        return;
    }
}

static const FAutoConsoleCommand OxygenToggle(TEXT("Dungeons.Player.EnableWaterBreathing")
	, TEXT("Activates/deactivates this character's oxygen component")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoOxygenToggle)
	, ECVF_Cheat);

static void DoOxygenToggleDebugText(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (HandleOnServer("Dungeons.Player.OxygenToggleDebugText", world, args))
	{
		return;
	}

	for (auto player : GetAllPlayerCharacters(world)) {
		if (auto oxygen = player->FindComponentByClass<UOxygenComponent>()) {
			oxygen->ShowDebugMessage = !oxygen->ShowDebugMessage;
		}
	}
}

static const FAutoConsoleCommand OxygenToggleDebugText(TEXT("Dungeons.Player.OxygenToggleDebugText")
	, TEXT("Activates/deactivates debug text to show character's oxygen metre")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoOxygenToggleDebugText)
	, ECVF_Cheat);

static void DoOxygenRefill(const TArray<FString>& args, UWorld* world, FOutputDevice& out)
{
	if (HandleOnServer("Dungeons.Player.OxygenRefill", world, args))
	{
		return;
	}

	for (auto player : GetAllPlayerCharacters(world)) {
		if (auto oxygen = player->FindComponentByClass<UOxygenComponent>()) {
			oxygen->FillOxygen();
		}
	}
}

static const FAutoConsoleCommand OxygenRefill(TEXT("Dungeons.Player.OxygenRefill")
	, TEXT("Refills a character's oxygen metre")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoOxygenRefill)
	, ECVF_Cheat);
