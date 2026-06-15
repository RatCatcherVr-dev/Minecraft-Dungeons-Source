#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/item/ItemRarity.h"
#include "EnchantmentData.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FEnchantmentData {
	GENERATED_USTRUCT_BODY()
public:
	FEnchantmentData(EEnchantmentTypeID type, int level = 0, EEnchantmentCategory category = EEnchantmentCategory::Unset, EEnchantmentSource source = EEnchantmentSource::Unset);
	FEnchantmentData();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	EEnchantmentTypeID TypeID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	EEnchantmentCategory Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	EEnchantmentSource Source;

	bool operator==(const FEnchantmentData&) const;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEnchantmentDataWithRarity {
	GENERATED_USTRUCT_BODY()
public:
	FEnchantmentDataWithRarity(FEnchantmentData enchantment, EItemRarity rarity = EItemRarity::Common);
	FEnchantmentDataWithRarity();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FEnchantmentData Enchantment = FEnchantmentData();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	EItemRarity Rarity;
};
