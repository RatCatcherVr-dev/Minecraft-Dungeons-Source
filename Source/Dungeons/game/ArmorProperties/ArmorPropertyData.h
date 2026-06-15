#pragma once
#include "game/item/ItemRarity.h"
#include "ArmorPropertyEnumTypes.h"
#include "ArmorPropertyData.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FArmorPropertyData {
	GENERATED_USTRUCT_BODY()
public:
	FArmorPropertyData(EArmorPropertyID id, EItemRarity Rarity);
	FArmorPropertyData(EArmorPropertyID id);
	FArmorPropertyData();

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons")
	EArmorPropertyID ID;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EItemRarity Rarity;

	bool operator==(const FArmorPropertyData&) const;
};