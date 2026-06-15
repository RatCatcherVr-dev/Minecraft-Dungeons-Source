#pragma once

#include "GameplayTagContainer.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/ItemType.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "world/entity/EntityTypes.h"
#include "ItemDropData.generated.h"

class APlayerCharacter;

UENUM(BlueprintType)
enum class EDropSpawnType : uint8 {
	//Roll once on server, spawn for all clients.
	All,
	//Roll for each individual, spawn on owning client/server
	Individual,
	//Spawn replicated on server
	Shared
};
ENUM_NAME(EDropSpawnType)

UENUM(BlueprintType)
enum class EDropCategory : uint8 {
	Food,
	Consumable,
	ConsumableNoWaterBreathing,
	Potion,
	Arrow,
	Emerald,
	Gold,
	Gear,
	Token,
	None,
	PermanentItem
};
ENUM_NAME(EDropCategory)


UENUM()
enum class EDropGeneratorCategory : uint8 {
	Food,
	Consumable,
	ConsumableNoWaterBreathing,
	Arrow,
	Emerald,
	Gold,
	Gear,
	Token,
	PermanentItem,
	LootTable,
	LootTableIncludeChildren //Uses loot table data, but includes the child ItemTypes (i.e uniques) of the base items defined in the table
};
ENUM_NAME(EDropGeneratorCategory)

USTRUCT(BlueprintType)
struct DUNGEONS_API FItemDrop {
	GENERATED_BODY()

	FItemDrop() : Category(EDropCategory::None) {}

	FItemDrop(
			const EDropCategory dropCategory,
			const int32 minDropCount = 1,
			const int32 maxDropCount = 1,
			const float probability = 1)
		: Category(dropCategory)
		, MinAmount(minDropCount)
		, MaxAmount(maxDropCount)
		, Probability(probability) {
	}

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	EDropCategory Category;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"), Category = "Dungeons")
	int32 MinAmount = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"), Category = "Dungeons")
	int32 MaxAmount = 1;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"), Category = "Dungeons")
	float Probability = 1.0f;
};

struct FItemDropChance {
	FItemId ItemType;
	float Weight;

	float getWeight() const {
		return Weight;
	}
};

USTRUCT()
struct FDropGeneratorData {
	GENERATED_BODY()

	FDropGeneratorData() {};

	FDropGeneratorData(EDropGeneratorCategory categrory, TArray<FSerializableItemId> table = {});

	UPROPERTY()
	EDropGeneratorCategory GeneratorCategory = EDropGeneratorCategory::LootTable;

	UPROPERTY()
	TArray<FSerializableItemId> DropTable;

	const game::item::generator::Pred pred() const;
};

USTRUCT()
struct FNetworkedItemDropData {

	GENERATED_BODY()

	FNetworkedItemDropData() {};
	FNetworkedItemDropData(EDropGeneratorCategory, TArray<FSerializableItemId>, EDropSpawnType, int32 min, int32 max, float probability);
	static TOptional<FNetworkedItemDropData> FromFItemDrop(const FItemDrop&);

protected:
	UPROPERTY()
	FDropGeneratorData DropData;

	UPROPERTY()
	EDropSpawnType SpawnType = EDropSpawnType::Individual;

	UPROPERTY()
	int32 MinAmount = 1;

	UPROPERTY()
	int32 MaxAmount = 1;

	UPROPERTY()
	float Probability = 1.0f;
public:

	const FDropGeneratorData& GetDropData() const { return DropData; }
	EDropSpawnType GetSpawnType() const { return SpawnType; }
	bool ShouldDropRandomized(float probabilityMultiplier) const;
	int32 GetDropCount(UWorld* world) const;
	float GetProbability() const;
};

USTRUCT()
struct DUNGEONS_API FDropCategoryDescription {
	GENERATED_BODY()

	FDropCategoryDescription()
		: Category(EDropCategory::None)
		, MinProbability(0)
		, MaxProbability(0)
		, MinAmount(0)
		, MaxAmount(0)
		, TimeToMaxSeconds(0) {
	}

	FDropCategoryDescription(
			const EDropCategory category,
			const float minProbability,
			const float maxProbability,
			const int32 minAmount,
			const int32 maxAmount,
			const float timeToMaxSeconds,
			const bool exclusive = true)
		: Category(category)
		, MinProbability(minProbability)
		, MaxProbability(maxProbability)
		, MinAmount(minAmount)
		, MaxAmount(maxAmount)
		, TimeToMaxSeconds(timeToMaxSeconds)
		, Exclusive(exclusive) {
	}

	TOptional<FNetworkedItemDropData> GetDropData() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	EDropCategory Category;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float MinProbability;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float MaxProbability;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int32 MinAmount;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int32 MaxAmount;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float TimeToMaxSeconds;

	UPROPERTY(EditAnywhere)
	bool Exclusive = true;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FItemDropSource {
	GENERATED_BODY()

	FItemDropSource()
		: TriggeringPlayer(nullptr) {
	}

	FItemDropSource(
			const FVector dropLocation,
			AActor* sourceActor,
			APlayerCharacter* triggeringPlayer);

	UPROPERTY()
	FVector DropLocation;

	UPROPERTY()
	APlayerCharacter* TriggeringPlayer;

	UPROPERTY()
	FGameplayTag TriggeringDamageType;

	UPROPERTY()
	EntityType MobType = EntityType::Undefined;
};
