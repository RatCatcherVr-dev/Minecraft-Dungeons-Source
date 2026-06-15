#pragma once

UENUM(BlueprintType)
enum class EUIHintType : uint8 {
	None,
	Movement,
	Objective,
	DefeatZombie,
	LowHealth,
	PickupArrow,
	RangedAttack,
	Artifact_Pickup,
	Artifact_OpenInventory,
	Artifact_Equip,
	Artifact_Activate,
	Artifact_RangedAttack,
	Enchanting_OpenInventory,
	Enchanting_SelectGear,
	Enchanting_SelectEnchantment,
	Enchanting_ItemEnchanted,
	MissionSelect_Marker,
	MissionSelect_Popup,
	Merchants_FindMerchants,
	Merchants_Interact,
	Map, 
	Teleport,
	Popping,
	ChatWheel_Open,
	ChatWheel_Select,
	ChatWheel_Chat,
	Merchants_Reserve,
	Stronghold_EndPortal,
	Stronghold_DoorLocked,
	Stronghold_SecretLootFound,
	ENUM_COUNT	// End Value
};
ENUM_NAME(EUIHintType);

const size_t EHintTypeFirstValue = (size_t)EUIHintType::None;
