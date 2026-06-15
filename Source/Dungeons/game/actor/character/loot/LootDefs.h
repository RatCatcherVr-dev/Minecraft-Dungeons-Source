#pragma once

#include "CoreMinimal.h"
#include "game/difficulty/Difficulty.h"
#include "game/levels.h"
#include "game/item/ItemTypeDefs.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"
#include "LootDefs.generated.h"

class ULootUnlockerComponent;

UENUM(BlueprintType)
enum class EMissionDifficultyCompletionLogic : uint8 {
	DISABLED,
	Any,
	UseCount,
	UseLevelID
};

UENUM(BlueprintType)
enum class ELootType : uint8 {
	WoodenChest,
	FancyChest
};

UENUM(BlueprintType)
enum class ELobbyChestLootType : uint8 {
	Static,
	RandomItem,
	RandomGear,
	Emeralds
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FItemMetaData
{
	GENERATED_USTRUCT_BODY()

	FItemMetaData();
	FItemMetaData(const FItemId& itemType);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	float DefaultCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool IsGear;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TWeakObjectPtr<UTexture2D> Image;

};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionDifficultyCompletion {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMissionDifficultyCompletionLogic CompletionLogic = EMissionDifficultyCompletionLogic::DISABLED;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameDifficulty DifficultyLevel = EGameDifficulty::Difficulty_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ELevelNames OptionalLevelName = ELevelNames::Invalid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OptionalCount = 1;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMobKillsCompletion {
	GENERATED_BODY()

	/** 
	 * Name of the mob to track for this goal. Must match an entity type (e.g. to track Cave Spiders use "cave_spider").
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MobEntityTypeName; // Currently the EntityType enum can't be converted to a UENUM, so using a string for the entity type name is a workaround.

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KillCount = -1;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FChestDynamicProperties {
	GENERATED_BODY()

	FChestDynamicProperties() {};
	FChestDynamicProperties(int32 cost, const FItemId& lootType, ELobbyChestLootType chestType, int32 emeraldReward)
		:EmeraldCost(cost)
		, RewardItemId(lootType)
		, ChestLootType(chestType)
		, EmeraldReward(emeraldReward)
	{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EmeraldCost = -1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FSerializableItemId RewardItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ELobbyChestLootType ChestLootType = ELobbyChestLootType::Static;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EmeraldReward = 0;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FUnlockRequirements {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 lootID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 playerLevel = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumberOfGems = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMissionDifficultyCompletion CompletedLevel = FMissionDifficultyCompletion();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMobKillsCompletion CompletedMobKills = FMobKillsCompletion();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canBeReused = false;

	int32 GetEmeraldCost() const
	{
		return NumberOfGems;		
	}
};
