#pragma once

#include "ItemRarity.h"
#include "ItemRarityChance.generated.h"

UENUM(BlueprintType)
enum class EItemRarityChanceCategory : uint8 {
	Mob,
	Boss,
	WoodenChest,
	FancyChest,
	DeluxeChest,
	LobbyChest,
	MissionEnd,
	PiggyBank,
	ProgressCommand,
	Default,
	EnchantedMobGroup,
	EventMob,
	VillageMerchant,
	LuxuryMerchant,
	PiglinMerchant,
	MysteryBox,
	OnlyCommon,
	AncientMob
};
ENUM_NAME(EItemRarityChanceCategory)

struct FItemRarityChance {
	EItemRarity Rarity;
	float Weight;

	float getWeight() const {
		return Weight;
	}
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FRareItemChance {
	GENERATED_BODY()

	FRareItemChance();
	explicit FRareItemChance(std::vector<FItemRarityChance>);

	EItemRarity GetRandomizedItemRarity(int boost = 0) const;
	TSet<EItemRarity> GetPossibleRarities() const;

	static FRareItemChance GetChanceFromCategory(EItemRarityChanceCategory);
	static FRareItemChance GetAdjustedChanceFromCategory(EItemRarityChanceCategory, AActor* = nullptr);
	void ModifyRarityWeights(float rareToUniqueFraction);

	std::vector<FItemRarityChance> RarityChances;
};
