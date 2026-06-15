#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/component/cosmetics/CosmeticsComponent.h"

UCosmeticsComponent* GetCosmeticsComponent(const TArray<FString>& commands, UWorld* world) {
	if (const auto* validCharacter = GetPlayerCharacter(world, commands)) {
		return validCharacter->FindComponentByClass<UCosmeticsComponent>();
	}
	return nullptr;
}

void EquipCosmetic(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() > 0) {
		if (auto* cosmeticsComponent = GetCosmeticsComponent(commands, world)) {
			cosmeticsComponent->Equip(FName(*commands[0]));
		}
	} else {
		out.Log(TEXT("First argument must be cosmetic item id."));
	}
}

void UnequipCosmetic(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (auto* cosmeticsComponent = GetCosmeticsComponent(commands, world)) {
		if (commands.Num() > 0) {
			cosmeticsComponent->Unequip(game::cosmetics::fromString(commands[0]));
		} else {
			cosmeticsComponent->UnequipAll();
		}		
	}
}

static FAutoConsoleCommand Equip(TEXT("Dungeons.Cosmetics.Equip")
	, TEXT("Equips player with specified cosmetic item") TEXT("\nUsage: Dungeons.Cosmetic.Equip <cosmetic ID>")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&EquipCosmetic)
	, ECVF_Cheat);

static FAutoConsoleCommand Unequip(TEXT("Dungeons.Cosmetics.Unequip")
	, TEXT("Unequips player cosmetic items") TEXT("\nUsage: Dungeons.Cosmetic.Unequip <slot>")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&UnequipCosmetic)
	, ECVF_Cheat);