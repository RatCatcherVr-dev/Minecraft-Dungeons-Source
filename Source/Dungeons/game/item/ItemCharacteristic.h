#pragma once

#include "UObject/ObjectMacros.h"
#include "ItemRarity.h"
#include "enchantment/EnchantmentData.h"
#include "ItemCharacteristic.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FItemCharacteristic
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	bool bIsNetherite = false;

	TOptional<FEnchantmentData> mEnchantment;

	FItemCharacteristic(FText Text, EItemRarity Rarity, bool Netherite, TOptional<FEnchantmentData> Enchantment)
		: Text(Text)
		, Rarity(Rarity)
		, bIsNetherite(Netherite)
		, mEnchantment(Enchantment)
	{}

	FItemCharacteristic(FText Text, EItemRarity Rarity, TOptional<FEnchantmentData> Enchantment)
		: FItemCharacteristic(std::move(Text), Rarity, false, Enchantment)
	{}

	FItemCharacteristic(FText Text, EItemRarity Rarity)
		: FItemCharacteristic(std::move(Text), Rarity, {})
	{}

	FItemCharacteristic(FText Text) 
		: FItemCharacteristic(std::move(Text), EItemRarity::Common)
	{}

	FItemCharacteristic()
		: FItemCharacteristic(FText())
	{}


	FItemCharacteristic(FText Text, bool bIsNetherite, const FEnchantmentData& enchantment)
		: FItemCharacteristic(std::move(Text), EItemRarity::Common, bIsNetherite, enchantment)
	{}
};