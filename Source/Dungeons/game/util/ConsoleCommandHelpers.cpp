#include "Dungeons.h"
#include "ConsoleCommandHelpers.h"
#include "game/Game.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/item/ItemType.h"
#include "game/mission/MissionDefs.h"
#include "game/util/ActorQuery.h"
#include "util/StringUtil.h"


TWeakObjectPtr<APlayerCharacter> GetPlayerFromUniqueId(UWorld* world, const FString& PlayerUniqueNetId) {
	if (const auto game = actorquery::getGame(world)) {
		const auto players = game->getPlayers();
		if (auto it = players.FindByPredicate([&PlayerUniqueNetId](const TWeakObjectPtr<APlayerCharacter>& p) {
			if (!p.IsValid()) {
				return false;
			}
			ensure(p->GetDungeonsBasePlayerState());
			return PlayerUniqueNetId == p->GetDungeonsBasePlayerState()->UniqueId.ToString();
		}))
		{
			return *it;
		}
	}

	return nullptr;
}

bool HandleOnServer(const FString& command, UWorld* world, TArray<FString> args)
{
	if (APlayerController* controller = world->GetFirstPlayerController()) {
		if (!controller->HasAuthority())
		{
			if (APlayerCharacter* character = Cast<APlayerCharacter>(controller->GetCharacter())) {
				auto* state = character->GetDungeonsBasePlayerState();
				FString uniqueNetIdAsText = state->UniqueId->ToString();
				UE_LOG(LogDungeons, Log, TEXT("Deferring command: %s to server @%s"), *command, *uniqueNetIdAsText);

				// queue for handling on dedicated server
				if (auto base_controller = Cast<ABasePlayerController>(controller)) //  && controller->Role == ROLE_AutonomousProxy
				{
					base_controller->Server_DeferredConsoleCommand(TEXT("" + command + " " + FString::Join(args, TEXT(" ")) + " @" + uniqueNetIdAsText));
					return true;
				}
			}
		}
	}
	return false;
}

APlayerCharacter* GetPlayerCharacter(UWorld* world, const TArray<FString>& args)
{
	if (args.Num())
	{
		if (auto it = args.FindByPredicate([](const FString& arg) {
			return arg.StartsWith("@", ESearchCase::CaseSensitive);
		}))
		{
			FString playerIdString = *it;
			playerIdString.RemoveAt(0);
			TWeakObjectPtr<APlayerCharacter> player = GetPlayerFromUniqueId(world, playerIdString);
			return player.Get();
		}
	}

	if (APlayerController* controller = world->GetFirstPlayerController()) {
		if (APlayerCharacter* validCharacter = Cast<APlayerCharacter>(controller->GetCharacter())) {
			return validCharacter;
		}
	}
	return nullptr;
}

TArray<APlayerCharacter*> GetAllPlayerCharacters(UWorld* world, const TArray<FString>& args) {
	FString filter = "";
	if (auto it = args.FindByPredicate([](const FString& arg) {
		return arg.StartsWith("@", ESearchCase::CaseSensitive);
	})) {
		filter = it->Mid(1);
	}

	// Was a unique id used as the filter argument?
	if (auto character = GetPlayerFromUniqueId(world, filter).Get()) {
		return { character };
	}

	// Try to parse the filter argument as either "all" or a 1-based index
	TArray<APlayerCharacter*> characters;
	for (FConstPlayerControllerIterator it = world->GetPlayerControllerIterator(); it; ++it) {
		if (APlayerCharacter* validCharacter = Cast<APlayerCharacter>(it->Get()->GetCharacter())) {
			characters.Add(validCharacter);
		}
	}

	if (filter == "" && characters.Num() > 0) {
		return { characters[0] };
	}
	else if (filter == "all") {
		return characters;
	}
	else {
		int index = FCString::Atoi(*filter) - 1;
		if (characters.IsValidIndex(index)) {
			return { characters[index] };
		}
		else {
			return {};
		}
	}
}

TOptional<FMissionDifficulty> parseMissionDifficulty(const FString& mission, const FString& difficulty, const FString& threatLevel, const FString& endlessStruggle) {
	auto level = EnumValueFromString(ELevelNames, mission);
	auto diff = EnumValueFromString(EGameDifficulty, "Difficulty_" + difficulty);
	auto threat = EnumValueFromString(EThreatLevel, "Threat_" + threatLevel);
	FEndlessStruggle struggle;
	struggle.Value = endlessStruggle.IsNumeric() ? FCString::Atoi( *endlessStruggle ) : 0;
	if (level.IsSet() && diff.IsSet() && threat.IsSet()) {
		return FMissionDifficulty{ level.GetValue(), diff.GetValue(), threat.GetValue(), struggle };
	}
	return {};
}

TOptional<FMissionDifficulty> parseMissionDifficulty(const TArray<FString>& arr, int startIndex /*= 0*/) {
	if (arr.Num() > startIndex + 3) {
		return parseMissionDifficulty(arr[startIndex], arr[startIndex + 1], arr[startIndex + 2], arr[startIndex + 3]);
	}
	else if (arr.Num() > startIndex + 2) {
		return parseMissionDifficulty(arr[startIndex], arr[startIndex + 1], arr[startIndex + 2], FString("0"));
	}
	return {};
}

TOptional<int> ArgAsInt(FString s) {
	s.TrimStartAndEndInline();
	return FCString::IsNumeric(*s) ? FCString::Atoi(*s) : TOptional<int>{};
}

TOptional<int> ArgAsInt(const TArray<FString>& arguments, int index) {
	return arguments.IsValidIndex(index) ? ArgAsInt(arguments[index]) : TOptional<int>{};
}

TOptional<float> ArgAsFloat(FString s) {
	s.TrimStartAndEndInline();
	return FCString::IsNumeric(*s) ? FCString::Atof(*s) : TOptional<float>{};
}

TOptional<float> ArgAsFloat(const TArray<FString>& arguments, int index) {
	return arguments.IsValidIndex(index) ? ArgAsFloat(arguments[index]) : TOptional<float>{};
}

TOptional<FString> ArgAsFString(const TArray<FString>& arguments, int index) {
	return arguments.IsValidIndex(index) ? arguments[index] : TOptional<FString>{};
}

TOptional<LevelArg> ArgAsLevel(const TArray<FString>& arguments, int index) {
	if (const auto levelName = ArgAsEnum<ELevelNames>(arguments, index)) {
		return LevelArg(levelName.GetValue());
	}
	return {};
}

TOptional<FItemId> ArgAsItemId(const TArray<FString>& arguments, int index) {
	return arguments.IsValidIndex(index)? GetItemRegistry().Request(*arguments[index]) : TOptional<FItemId>{};
}

TOptional<EntityType> ArgAsEntityType(const TArray<FString>& arguments, int index) {
	return arguments.IsValidIndex(index) ? MaybeEntityTypeFromString(stringutil::toStdString(arguments[index])) : TOptional<EntityType>{};
}

LevelArg::LevelArg(ELevelNames level, FString filename)
	: level(level)
	, filename(filename) {
}

LevelArg::LevelArg(ELevelNames level)
	: level(level)
	, filename(missions::get(level).levelFilename()) {
}

ItemSaveData createItem(FInventoryItemData data, TOptional<EEquipmentSlot> slot, TOptional<int> slotIndex) {
	check(slot.IsSet() ^ slotIndex.IsSet()); 

	return	ItemSaveData(
			data.GetItemId(),
			data.ItemPower,
			data.Rarity,
			data.bIsUpgraded,
			data.bIsGifted,
			data.bIsModified,
			slot.IsSet(),
			true,
			false,
			slot.Get(EEquipmentSlot::Invalid),
			slotIndex.Get(-1),
			algo::map_vector(data.Enchantments, [](const auto& entry) ->EnchantmentSaveData { return { entry.TypeID, entry.Level }; }),
			algo::map_vector(data.ArmorProperties, [](const auto& entry) ->ArmorPropertySaveData { return { entry.ID }; }), 
			data.IsNetherite() ? EnchantmentSaveData { data.NetheriteEnchant().TypeID, data.NetheriteEnchant().Level } : TOptional<EnchantmentSaveData>()
		);
}

ItemSaveData createItem(FInventoryItemData data, int slotIndex) {
	return createItem(std::move(data), {}, slotIndex);
}

ItemSaveData createItem(FInventoryItemData data, EEquipmentSlot slot) {
	return createItem(std::move(data), slot, {});
}
