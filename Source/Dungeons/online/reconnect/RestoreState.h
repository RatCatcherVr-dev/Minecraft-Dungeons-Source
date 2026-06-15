#pragma once
#include "game/actor/character/player/PlayerCharacter.h"

enum class ESlotType : uint8;
enum class EItemType : uint8;

struct RestoreState {
	static RestoreState CreateFromPlayerCharacter(const APlayerCharacter* pc);
	void ApplyToPlayerCharacter(APlayerCharacter* pc) const;

private:
	RestoreState(float health, int missionStartingGearPower, bool hasHealthPotion, TArray<float> cooldowns);

	void StoreQuiver(const class UEquipmentComponent*, ESlotType, const FItemId&);

	struct Quiver {
		Quiver(ESlotType slotType, const FItemId& itemType, int count) : SlotType(slotType), ItemType(itemType), Count(count) {}
		ESlotType SlotType;
		FItemId ItemType;
		int Count;
	};

	float HealthPercentage;
	TArray<Quiver> Quivers;
	int MissionStartingGearPower;

	bool HasHealthPotion;
	TArray<float> Cooldowns;
};
