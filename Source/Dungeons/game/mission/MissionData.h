#pragma once

#include "game/dlc/DLCName.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/SerializableItemId.h"
#include "world/entity/EntityTypes.h"
#include "MissionData.generated.h"

class Random;

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionItemTypeChance {
	GENERATED_BODY()

	FMissionItemTypeChance(const FItemId& type, EItemRarity rarity);
	FMissionItemTypeChance();

	UPROPERTY(BlueprintReadOnly)
	FSerializableItemId ItemId;

	UPROPERTY(BlueprintReadOnly)
	EItemRarity guaranteedRarity = EItemRarity::Common;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionItemTagChance {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ItemTag itemTag = ItemTag::Unset;

	UPROPERTY(BlueprintReadOnly)
	EItemRarity guaranteedRarity = EItemRarity::Common;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionItemRarityChance {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EItemRarity guaranteedRarity = EItemRarity::Common;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionMobChance {
	GENERATED_BODY()
	
	EntityType mob = EntityType::Undefined;
	TOptional<EDLCName> missingDLC;
};


UENUM(BlueprintType)
enum class EInfoChanceIcon : uint8 {
	None,
	Average,
};
ENUM_NAME(EInfoChanceIcon);

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionProbability {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EInfoChanceIcon icon;
	UPROPERTY(BlueprintReadOnly)
	FText display;
	UPROPERTY(BlueprintReadOnly)
	FText tooltip;
};



struct FMissionItemChance {
	TOptional<FMissionItemTypeChance>   typeChance;
	TOptional<FMissionItemTagChance>    tagChance;
	TOptional<FMissionItemRarityChance> rarityChance;
	TOptional<itemgen::Config> customConfig;

	float getWeight() const { return weight; }

	float weight = 1.0f;
};

struct PredRarity {
	itemgen::Pred pred;
	FRareItemChance rarityChance;
};
PredRarity createPredRarity(const FMissionItemTypeChance&);
PredRarity createPredRarity(const FMissionItemTagChance&);
PredRarity createPredRarity(const FMissionItemRarityChance&);
PredRarity createPredRarity(const FMissionItemChance&); // take an EItemRarityChanceCategory?


class MissionItemChances {
public:
	MissionItemChances(TArray<FMissionItemChance>);
	MissionItemChances(itemgen::Config itemTypes);

	const TArray<FMissionItemChance>& itemChances() const;

	TArray<FMissionItemTypeChance> getPossibleItemRarityTypes(const struct FMissionDifficulty&) const;
private:
	TArray<FMissionItemChance> mItemChances;
};