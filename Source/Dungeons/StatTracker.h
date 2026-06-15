#pragma once

#if DUNGEONS_OSS_ENABLED
#include "Interfaces/OnlineStatsInterface.h"
#endif

#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "OnlineSubsystem.h"
#include "Components/ActorComponent.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "world/entity/EntityClassTree.h"
#include "game/item/ItemTypeDefs.h"
#include "game/levels.h"
#include "game/difficulty/Difficulty.h"
#include "game/component/EnchantmentComponent.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/ItemType.h"
#include "game/item/instance/AItemInstance.h"
#include "game/mission/variation/LevelVariationType.h"
#include "online/crossplay/Achievements.h"
#include "online/crossplay/Identity.h"
#include "online/crossplay/Stats.h"
#include "game/TitleDefs.h"
#include "StatTracker.generated.h"

namespace DungeonsQA {
	extern void TriggerAchievement(const TArray<FString>& commands, UWorld* world, FOutputDevice& out);
}

// D11.SSN
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTitleUnlocked, ETitle, title);

UENUM()
enum class EAchievement : uint8 {
	INVALID,
	REVIVE_20,
	KILL_50_MOBS,
	KILL_2500_MOBS,
	KILL_50_PASSIVE,
	KILL_50_ENCHANTED,
	LVL_10,
	LVL_25,
	LVL_50,
	FANCY_CHEST,
	CHEST_100,
	EMERALD_1000,
	EMERALD_5000,
	EAT_200_FOOD,
	COMPLETE_SQUID_COAST,
	COMPLETE_CREEPER_WOODS,
	COMPLETE_LEVEL_GROUP1,
	COMPLETE_LEVEL_GROUP2,
	COMPLETE_LEVEL_GROUP3,
	DEFEAT_ARCH_ILLAGER_ANY,
	DEFEAT_ARCH_ILLAGER_HARD,
	TNT_KILL_10,
	FULL_ENCHANTED_GEAR_SET,
	FULLY_UPGRADE_ENCHANTMENT,
	BEAST_MASTER,
	DEFEAT_JUNGLE_ABOMINATION_ADVENTURE,
	COMPLETE_PANDA_PLATEAU,
	OCELOT_ARMOUR,
	COMPLETE_DINGY_JUNGLE_POTION,
	DEFEAT_MOOSHROOM_MONSTROSITY_ITEMS,
	DEFEAT_WRETCHED_WRAITH_ADVENTURE,
	COMPLETE_LOST_SETTLEMENT,
	ICE_WAND_KILL_5,
	ICE_SLIDE_500,
	ICE_NO_WEAPONS,
	COMPLETE_GALE_SANCTUM_ADVENTURE,
	COMPLETE_COLOSSAL_RAMPART,
	KILL_10_MOUNTAINEER_TEMPEST_KNIFE_TIMED,
	UPDRAFT_WINDCALLERS_3,
	LLAMA_OFFERING,
	KILL_RAVAGER_SQUALL_GOLEM_TIMED,
	DEFEAT_TEMPEST_GOLEM_PETS,
	COMPLETE_OBSIDIAN_PINNACLE_APOCALYPSE5,
	COMPLETE_DAILY_TRIAL_EMERALD_ARMOUR,
	DEATH_BARTER_2,
	COMPLETE_NETHER_WASTES_APOCALYPSE,
	COMPLETE_NETHER_SECRET_APOCALYPSE_PLUS,
	DEFEAT_ANCIENT_15,
	COLLECT_500_GOLD,
	MERCHANTS_6,
	GONG_6,
	SPINWHEEL_70,
	HYPERMISSIONS_4,
	HOMECOMING,
	EFFECTS_TRIGGER_5,
	COMPLETE_ABYSSAL_MONUMENT_APOCALYPSE,
	COMPLETE_RADIANT_RAVINE_APOCALYPSE_PLUS,
	GLOWSQUID_ICEWAND,
	FISHING_ROD,
	LOST_CONDUIT,
	RELAX_BUBBLE_COLUMN,
	NO_OXYGEN_60,
	TRIDENT,
	RAID_CAPTAIN_10,
	BUBBLE_BOW,
	TRAVEL_THROUGH_END_PORTAL, //DLC6
	ENTER_STRONGHOLD_VAULT,
	DEFEAT_BROKEN_HEART_OF_ENDER,
	UNLOCK_ALL_ITEMS_BASE_AND_DLC,
	REACH_TOP_OF_BROKEN_CITADEL_NO_HIT_SHULKERS_BULLET,
	DEFEAT_1000_ENDERLINGS_WITH_OBSIDIAN_BLADE,
	SURVIVE_AFFLICTED_BY_VOIDED_X999,
	ELYTRA_DIVE_ATTACK_12_ENEMIES_AT_ONCE,
	BURNING_BREW_DEFEAT_MOB_WITH_FIRE_ENCHANTMENT,
	SQUID_COAST_EASTER_EGG,
	ENUM_END
};
ENUM_NAME(EAchievement);
enum_bitwise_operators(EAchievement, int);

// D11.SSN
UENUM()
enum class EFeatureStat : uint8 {
	INVALID,
	TIME,
	KILLS,
	DEATHS,
	DISTANCE,
	CHESTS,
	HEAL,
	EMERALDS,
	ARTEFACTS,
	REVIVES,
	LEVEL
};

// D11.SSN
struct TrackedStats {
	// achievements
	std::bitset<128> achievements = 0;
	std::bitset<128> cachedAchievements = 0;
	TArray<EntityType> ancientKills;
	TArray<EntityType> underwaterHooks;
	TArray<TPair<bool, TPair<int32, int32>>> coopMissions;
	uint32 passiveKills = 0;
	uint32 enchantedKills = 0;
	uint32 foodEaten = 0;
	uint32 gold = 0;
	uint32 bannerMissions = 0;
	uint32 endlerlingKills = 0;
	float blocksSlid = 0;
	bool wolf = false;
	bool llama = false;
	bool golem = false;
	bool pumpkinPastures = false;
	bool soggySwamp = false;
	bool redstoneMines = false;
	bool cactiCanyon = false;
	bool desertTemple = false;
	bool fieryForge = false;
	bool highblockHalls = false;
	bool obsidianPinnacle = false;
	bool crimsonForest = false;
	bool soulsandValley = false;
	bool netherFortress = false;
	bool wolfHome = false;
	bool batHome = false;
	bool wraithHome = false;
	bool stronghold = false;
	bool brokenCitadel = false;
	// feature
	float healed = 0;
	float distanceTravelled = 0; // kilometres
	float timePlayed = 0;
	uint32 deaths = 0;
	uint32 usedArtefacts = 0;
	// titles
	std::bitset<128> titles = 1;
	uint64 TNTKills = 0;
	uint64 creeperKills = 0;
	// shared
	uint32 revives = 0;
	uint32 mobKills = 0;
	uint32 level = 0;
	uint32 chestsOpened = 0;
	uint32 emeralds = 0;
};

class UDungeonsGameInstance;

// D11.SSN
UCLASS()
class DUNGEONS_API UStatTrackerComponent : public UActorComponent {
	GENERATED_BODY()

	// tracking
public:
	UStatTrackerComponent();

	bool TryInit(APlayerController* controller);

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Save();
	const TrackedStats& GetStats() const;

	// achievements
	void CompleteLevel(ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, EExtraChallenge extraChallenge, bool hyperMission, ELevelVariationType levelVariation, int collectedTokens);
	void StartLevel(ELevelNames name);
	void Enchant(int level);
	void GearChange(const TArray<FEnchantmentDataWithRarity>& enchantments);
	void OpenFancyChest(EItemRarityChanceCategory rarity);
	void TNT(int kills);
	void OcelotArmour(bool tracking);
	void IceWand(int kills);
	void IceWeapon(bool tracking);
	void IceSliding(bool sliding);
	void OnMeleeAttack(TArray<EntityType> killedMobs);
	void UpdraftWindCaller(int count);
	void LlamaOffering();
	void EmeraldArmour(bool tracking);
	void DeathBarter();
	void CheckUnlockedMerchants();
	void SpinWheel(int hits);
	void TileChange(const std::string& tileId, bool equipmentChange = false);
	void EquipmentChange();
	void GlowSquid();
	void Hooked(EntityType& mobEntityType);
	void BubbleColumn(bool entered);
	void Drowning(bool isDrowning);
	void Trident(int kills);
	void Bubbled(bool bubbled);
	void ShulkerHit(bool hit);
	void LostConduit();
	void ElytraAttack(int hits);
	void SquidEasterEgg();
	void InitAllItems();
	void UnlockedItemsTracking();
	void GotItem(const FItemId &item);
	void Voided(float magnitude);
	void BurningBrew(bool isActive);

	// feature
	void Heal(float amount);
	void OnDeath();
	void UpdateTime(float deltaSeconds);
	void UpdateTravel(float distance);
	void UseItem(const ItemType& type);
	// titles

	// shared
	void GotCurrency(const FItemId& type, int amount);
	void LevelUp(int level);
	void OpenChest();
	void RegisterKill(EntityType mobEntityType, bool enchanted);
	void ReviveFriend();

	bool showFoodEatenDebugMessage = false;
	// other
	UFUNCTION()
	void OnGameOver(bool isGameOver);
	bool showIceSlideDistanceDebugMessage = false;

protected:
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnTitleUnlocked OnTitleUnlocked;

private:
	TrackedStats* stats;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	// achievements
	void OnQueryAchievementsComplete(const FUniqueNetId& playerId, bool bWasSuccessful);
	void WriteAchievements(EAchievement achievement, float percentage);
	void WriteAchievementsPlatform(EAchievement achievement, float percentage);
	void WriteCachedAchievements();
	void OnAchievementWritten(bool success, TSharedPtr<const FUniqueNetId> playerID, int achievementID, FString achievementName, float percentage, FString achievementURL, FString achievementTitle, FString achievementDesc);

	TSharedPtr<const FUniqueNetId> netID;

	TMap<EAchievement, float> userAchievements;
	TMap<EAchievement, float> cachedAchievements;
	float timeSinceLastCallAttempt = 0;
	bool retryAchievementQuery = false;

	bool ocelotTracking = false;
	bool junglePotionTracking = true;
	bool iceWeaponTracking = true;
	bool iceSliding = false;
	bool emeraldArmourTracking = false;
	bool bubbleColumnTracking = false;
	bool drowningTracking = false;
	float timeInBubbleColumn = 0.0f;
	float timeDrowning = 0.0f;
	int deathBarterCount = 0;
	bool shulkerHit = false;
	bool burningBrewPotionTracking = false;
	TArray<TPair<EntityType, int64>> killedGiantSlayerEnemies;
	TArray<int64> killedTempestKnifeMountaineers;
	TArray<int64> gongActivations;
	std::string currentTile = "";
	FTimerHandle tileCheckTimerHandle;
	int bubbledEnemiesCount = 0;
	TMap<FItemId, bool> unlockedItems;
	uint32 unlockedItemsCount = 0;
	uint32 retrievedUnlockedItemsCount = 0;

	// friend function for WriteAchivementsAccess//QA?!
	friend void DungeonsQA::TriggerAchievement(const TArray<FString>& commands, UWorld* world, FOutputDevice& out);

	// feature stats
#if DUNGEONS_OSS_ENABLED
	void WriteStats();
	TSharedPtr<const FOnlineStatsUserStats> userStats;
#endif

	// titles
	void UnlockTitle(ETitle title, float percentage);
};
