#pragma once

#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ThreatLevel.h"
#include "game/mission/difficulty/MissionDifficulty.h"
#include "save/CharacterSaveData.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"

class APlayerCharacter;

#define MISSION_DIFFICULTY_ARG_EXAMPLE TEXT("\"pumpkinpastures 1 2\"")

TWeakObjectPtr<APlayerCharacter> GetPlayerFromUniqueId(UWorld*, const FString& PlayerUniqueNetId);
APlayerCharacter* GetPlayerCharacter(UWorld*, const TArray<FString>& args = {});
TArray<APlayerCharacter*> GetAllPlayerCharacters(UWorld*, const TArray<FString>& args = {});
template <typename Component>
Component* GetPlayerComponent(UWorld*, const TArray<FString>& args = {});
template <typename Component>
Component* GetPlayerControllerComponent(const APlayerCharacter*);
template <typename Component>
Component* GetPlayerControllerComponent(UWorld*, const TArray<FString>& args = {});

bool HandleOnServer(const FString& command, UWorld*, TArray<FString> args = {});

TOptional<FMissionDifficulty> parseMissionDifficulty(const FString& mission, const FString& difficulty, const FString& threatLevel, const FString& endlessStruggle);
TOptional<FMissionDifficulty> parseMissionDifficulty(const TArray<FString>& args, int startIndex = 0);

struct LevelArg {
	LevelArg(ELevelNames);
	LevelArg(ELevelNames, FString filename);

	ELevelNames level;
	FString filename;
};

TOptional<int>      ArgAsInt(FString);
TOptional<int>	    ArgAsInt(const TArray<FString>& arguments, int index);
TOptional<float>    ArgAsFloat(FString);
TOptional<float>    ArgAsFloat(const TArray<FString>& arguments, int index);
TOptional<FString>  ArgAsFString(const TArray<FString>& arguments, int index);
template <typename Enum>
TOptional<Enum>     ArgAsEnum(const TArray<FString>& arguments, int index);
template <typename Enum>
TOptional<Enum>     ArgAsEnum(const TArray<FString>& arguments, int index, std::initializer_list<FString> prefixes);
TOptional<LevelArg> ArgAsLevel(const TArray<FString>& arguments, int index);
TOptional<FItemId>  ArgAsItemId(const TArray<FString>& arguments, int index);
TOptional<EntityType> ArgAsEntityType(const TArray<FString>& arguments, int index);


// Moved here temporarily while GameSave file is being worked on in another branch
ItemSaveData createItem(FInventoryItemData, int slotIndex);
ItemSaveData createItem(FInventoryItemData, EEquipmentSlot);


template <typename Component>
Component* GetPlayerComponent(UWorld* world, const TArray<FString>& args /*= {}*/) {
	if (APlayerCharacter* character = GetPlayerCharacter(world, args)) {
		return character->FindComponentByClass<Component>();
	}
	return nullptr;
}

template <typename Component>
Component* GetPlayerControllerComponent(const APlayerCharacter* character) {
	auto* controller = character->GetPlayerController();
	if (!controller) {
		return nullptr;
	}
	return controller->FindComponentByClass<Component>();
}

template <typename Component>
Component* GetPlayerControllerComponent(UWorld* world, const TArray<FString>& args /*= {}*/) {
	const auto* character = GetPlayerCharacter(world, args);
	if (!character) {
		return nullptr;
	}
	return GetPlayerControllerComponent<Component>(character);
}

template <typename Enum>
TOptional<Enum> ArgAsEnum(const TArray<FString>& arguments, int index) {
	if (auto s = ArgAsFString(arguments, index)) {
		return GetEnumValueFromStringT<Enum>(s.GetValue());
	}
	return {};
}

template <typename Enum>
TOptional<Enum> ArgAsEnum(const TArray<FString>& arguments, int index, std::initializer_list<FString> prefixes) {
	if (auto s = ArgAsFString(arguments, index)) {
		if (auto e = GetEnumValueFromStringT<Enum>(s.GetValue())) {
			return e;
		}
		for (auto& prefix : prefixes) {
			if (auto e = GetEnumValueFromStringT<Enum>(prefix + s.GetValue())) {
				return e;
			}
		}
	}
	return {};
}

//
// ArgAsEnum specializations
//
template <> inline TOptional<EGameDifficulty> ArgAsEnum(const TArray<FString>& arguments, int index) { return ArgAsEnum<EGameDifficulty>(arguments, index, { FString("Difficulty_") }); }
template <> inline TOptional<EThreatLevel>    ArgAsEnum(const TArray<FString>& arguments, int index) { return ArgAsEnum<EThreatLevel>(arguments, index, { FString("Threat_") }); }
