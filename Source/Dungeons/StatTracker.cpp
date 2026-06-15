#include "StatTracker.h"
#include "DungeonsGameInstance.h"
#include "Dungeons.h"
#include "DungeonsGameState.h"
#include "OnlineSubsystemUtils.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/MissionProgresscomponent.h"
#include "game/item/instance/AItemInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "game/merchant/MerchantDefs.h"
#include "world/entity/MobTags.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGenerator.h"

#if PLATFORM_XBOXONE
#include "XboxOne/XboxOneApplication.h"
#include "XboxOne/XboxOneInputInterface.h"
#endif

//const FString TIME_STAT = "1"; no longer used
const FString KILLS_STAT = "2";
const FString DEATHS_STAT = "3";
const FString DISTANCE_STAT = "4";
const FString CHESTS_STAT = "5";
const FString HEAL_STAT = "6";
const FString EMERALD_STAT = "7";
const FString ARTEFACTS_STAT = "8";
const FString REVIVES_STAT = "9";
const FString LEVEL_STAT = "10";

const std::string WOLF_TILE("cw_wolf_achievement");
const std::string BAT_TILE("sc_bat_achievement");
const std::string SOUL_TILE("sv_soulentity_achievement");

const TArray<FItemId>& consumableItems() {
	static const TArray<FItemId> consumableItems = {
		game::item::type::HealthPotion.getId(),
		game::item::type::SwiftnessPotion.getId(),
		game::item::type::BackstabbersBrew.getId(),
		game::item::type::StrengthPotion.getId(),
		game::item::type::DefensePotion.getId(),
		game::item::type::IcePotion.getId(),
		game::item::type::DenseBrewPotion.getId(),
		game::item::type::WaterBreathingPotion.getId(),
		game::item::type::BurningBrewPotion.getId(),
		game::item::type::Emerald.getId(),
		game::item::type::Gold.getId(),
		game::item::type::Arrow.getId(),
		game::item::type::BurningArrow.getId(),
		game::item::type::FireworksArrow.getId(),
		game::item::type::TormentProjectile.getId(),
		game::item::type::ThunderingArrow.getId(),
		game::item::type::HeavyHarpoon.getId(),
		game::item::type::TNTBox.getId(),
		game::item::type::Conduit.getId(),
		game::item::type::Trident.getId(),
		game::item::type::Conduit.getId(),
		game::item::type::VoidArrow.getId(),
		game::item::type::PoisonArrow.getId(),
	};
	return consumableItems;
}

// listed separately as they're needed for an achievement
const TArray<FItemId>& foodItems() {
	static const TArray<FItemId> foodItems = {
		game::item::type::Food1.getId(),
		game::item::type::Food2.getId(),
		game::item::type::Food3.getId(),
		game::item::type::Food4.getId(),
		game::item::type::Food5.getId(),
		game::item::type::Food6.getId(),
		game::item::type::Food7.getId(),
		game::item::type::Food8.getId(),
	};
	return foodItems;
}

const TArray<FItemId>& jungleItems() {
	static const TArray<FItemId> jungleItems = {
		game::item::type::OcelotArmor.getId(),
		game::item::type::OcelotArmor_Unique1.getId(),
		game::item::type::Whip.getId(),
		game::item::type::Whip_Unique1.getId(),
		game::item::type::CorruptedSeeds.getId(),
	};
	return jungleItems;
}

const TArray<EntityType>& underwaterMobs() {
	static const TArray<EntityType> underwaterMobs = {
		EntityType::TropicalSlimeLarge,
		EntityType::Squid,
		EntityType::GlowSquid,
		EntityType::Turtle,
		EntityType::Pufferfish,
		EntityType::Drowned,
		EntityType::Guardian,
		EntityType::Dolphin,
		EntityType::SunkenSkeleton,
		EntityType::BabyDrowned,
		EntityType::WaveWhisperer,
	};
	return underwaterMobs;
}
 
const TArray<ELevelNames>& allLevels() {
	static const TArray<ELevelNames> allLevels = {
		ELevelNames::squidcoast,
		ELevelNames::creeperwoods,
		ELevelNames::pumpkinpastures,
		ELevelNames::soggyswamp,
		ELevelNames::mooncorecaverns,
		ELevelNames::fieryforge,
		ELevelNames::deserttemple,
		ELevelNames::highblockhalls,
		ELevelNames::obsidianpinnacle,
		ELevelNames::cacticanyon,
		ELevelNames::creepycrypt,
		ELevelNames::soggycave,
		ELevelNames::underhalls,
		ELevelNames::archhaven,
		ELevelNames::lowertemple,
		ELevelNames::mooshroomisland,
		ELevelNames::dingyjungle,
		ELevelNames::overgrowntemple,
		ELevelNames::bamboobluff,
		ELevelNames::frozenfjord,
		ELevelNames::lonelyfortress,
		ELevelNames::lostsettlement,
		ELevelNames::crimsonforest,
		ELevelNames::basaltdeltas,
		ELevelNames::warpedforest,
		ELevelNames::soulsandvalley,
		ELevelNames::netherwastes,
		ELevelNames::netherfortress,
		ELevelNames::windsweptpeaks,
		ELevelNames::galesanctum,
		ELevelNames::endlessrampart,
		ELevelNames::gauntletgales,
		ELevelNames::coralrise,
		ELevelNames::abyssalmonument,
		ELevelNames::radiantravine,
		ELevelNames::enderwilds,
		ELevelNames::blightedcitadel,
		ELevelNames::thestronghold
	};
	return allLevels;
}

const TMap<EAchievement, uint32> achievementTarget() {
	static const TMap<EAchievement, uint32> achievementTarget = {
		{EAchievement::REVIVE_20, 20},
		{EAchievement::KILL_50_MOBS, 50},
		{EAchievement::KILL_2500_MOBS, 2500},
		{EAchievement::KILL_50_PASSIVE, 50},
		{EAchievement::KILL_50_ENCHANTED, 50},
		{EAchievement::LVL_10, 10},
		{EAchievement::LVL_25, 25},
		{EAchievement::LVL_50, 50},
		{EAchievement::CHEST_100, 100},
		{EAchievement::EMERALD_1000, 1000},
		{EAchievement::EMERALD_5000, 5000},
		{EAchievement::EAT_200_FOOD, 200},
		{EAchievement::TNT_KILL_10, 10},
		{EAchievement::FULL_ENCHANTED_GEAR_SET, 9},
		{EAchievement::FULLY_UPGRADE_ENCHANTMENT, 3},
		{EAchievement::ICE_WAND_KILL_5, 5},
		{EAchievement::ICE_SLIDE_500, 500},
		{EAchievement::KILL_10_MOUNTAINEER_TEMPEST_KNIFE_TIMED, 10},
		{EAchievement::UPDRAFT_WINDCALLERS_3, 3},
		{EAchievement::DEFEAT_TEMPEST_GOLEM_PETS, 4},
		{EAchievement::COMPLETE_OBSIDIAN_PINNACLE_APOCALYPSE5, 5},
		{EAchievement::DEATH_BARTER_2, 2},
		{EAchievement::DEFEAT_ANCIENT_15, 15},
		{EAchievement::COLLECT_500_GOLD, 500},
		{EAchievement::MERCHANTS_6, 6},
		{EAchievement::GONG_6, 6},
		{EAchievement::SPINWHEEL_70, 70},
		{EAchievement::HYPERMISSIONS_4, 4},
		{EAchievement::EFFECTS_TRIGGER_5, 5},
		{EAchievement::RELAX_BUBBLE_COLUMN, 300},
		{EAchievement::NO_OXYGEN_60, 60},
		{EAchievement::TRIDENT, 10},
		{EAchievement::RAID_CAPTAIN_10, 10},
		{EAchievement::BUBBLE_BOW, 10},
		{EAchievement::ELYTRA_DIVE_ATTACK_12_ENEMIES_AT_ONCE, 12},
		{EAchievement::DEFEAT_1000_ENDERLINGS_WITH_OBSIDIAN_BLADE, 1000},
		{EAchievement::SURVIVE_AFFLICTED_BY_VOIDED_X999, 999},
	};
	return achievementTarget;
}

static TArray<FItemId> allItems;

const TMap<ETitle, uint32> titleTarget() {
	static const TMap<ETitle, uint32> titleTarget = {
		{ETitle::BOOMER, 1000},
		{ETitle::CREEPERS_BANE, 1000},
	};
	return titleTarget;
}

uint64 AchievementToBitValue(EAchievement achievement) {
	return (1LL << static_cast<uint64>(achievement));
}

float PercentageCompleted(int value, int targetValue) {
	return FMath::Clamp((static_cast<float>(value) / static_cast<float>(targetValue)) * 100.0f, 0.0f, 100.0f);
}

float BoolPercentage(const TArray<bool>& booleans) {
	int count = algo::count(booleans, true);
	return PercentageCompleted(count, booleans.Num());
}

uint32 PercentToValue(float percent, int targetValue) {
	return FMath::RoundToInt((percent / 100.f) * static_cast<float>(targetValue));
}

// D11.SSN
UStatTrackerComponent::UStatTrackerComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1.0f;
}

// check if online interface is connected properly
bool UStatTrackerComponent::TryInit(APlayerController* controller) {
#if ACHIEVEMENTS_ENABLED && !WITH_EDITOR
	UDungeonsGameInstance* gameInstance = GetOwner()->GetGameInstance<UDungeonsGameInstance>();
	if (UDungeonsLocalPlayer* localPlayer = Cast<UDungeonsLocalPlayer>(controller->GetLocalPlayer())) {
		const int controllerId = localPlayer->GetControllerId();
		const int saveID = Cast<APlayerControllerBase>(controller)->GetSaveLocalUserNum();
		stats = &gameInstance->EditGlobalSaveState(saveID)->GetTrackedStats();
		auto onlineIdentity = online::getIdentityInterface();
		auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF();

		if (onlineAchievements.IsValid() && onlineIdentity.IsValid()) {
			if (netID = onlineAchievements->GetUniqueId(controllerId)) {
				// check if the player is a guest
				if (localPlayer != gameInstance->GetFirstGamePlayer() && onlineIdentity->GetSponsorUniquePlayerId(controllerId)) {
					return false;
				}

				InitAllItems();

				if (onlineAchievements.IsValid()) {
					onlineAchievements->QueryAchievements(*netID.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UStatTrackerComponent::OnQueryAchievementsComplete));
#ifdef HAS_ACHIEVEMENT_WRITTEN_TRACKER_DELEGATE
					onlineAchievements->OnAchievementWrittenTrackerDelegates.AddUObject(this, &UStatTrackerComponent::OnAchievementWritten);
#endif
				}

				if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
					playerCharacter->OnPlayerDeath.AddUObject(this, &UStatTrackerComponent::OnDeath);
				}


				if (ADungeonsGameState* gameState = GetWorld()->GetGameState<ADungeonsGameState>()) {
					gameState->OnIsGameOver.AddDynamic(this, &UStatTrackerComponent::OnGameOver);
				}

				RegisterComponent();
				SetComponentTickEnabled(true);
				CheckUnlockedMerchants();
				return true;
			}
		}
	}
#endif
	// disable the stat tracker if the user is a guest or platform has no achievements
	return false;
}

void UStatTrackerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (GetWorld()->GetTimerManager().IsTimerActive(tileCheckTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(tileCheckTimerHandle);
	}
	Save();
}

void UStatTrackerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTime(DeltaTime);

	//try to re-query stats & achievements if necessary
	//and rewrite failed achievements, if any
	timeSinceLastCallAttempt += DeltaTime;
	if (timeSinceLastCallAttempt >= 30) {
#if ACHIEVEMENTS_ENABLED
		auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF();
		if (retryAchievementQuery && onlineAchievements.IsValid()) {
			onlineAchievements->QueryAchievements(*netID.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UStatTrackerComponent::OnQueryAchievementsComplete));
			timeSinceLastCallAttempt = 0;
		}
		else if (cachedAchievements.Num() > 0) {
			WriteCachedAchievements();
			timeSinceLastCallAttempt = 0;
		}
#endif
	}

	// secret achievement - no progress writing
	if (bubbleColumnTracking) {
		timeInBubbleColumn += DeltaTime;
		if (timeInBubbleColumn >= achievementTarget()[EAchievement::RELAX_BUBBLE_COLUMN]) {
			WriteAchievements(EAchievement::RELAX_BUBBLE_COLUMN, 100.0f);
			BubbleColumn(false);
		}
	}

	// secret achievement - no progress writing
	if (drowningTracking) {
		timeDrowning += DeltaTime;
		if (timeDrowning >= achievementTarget()[EAchievement::NO_OXYGEN_60]) {
			WriteAchievements(EAchievement::NO_OXYGEN_60, 100.0f);
			Drowning(false);
		}
	}
}

void UStatTrackerComponent::CompleteLevel(ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, EExtraChallenge extraChallenge, bool hyperMission, ELevelVariationType levelVariation, int collectedTokens) {
	// individual levels
	if (name == ELevelNames::squidcoast) {
		WriteAchievements(EAchievement::COMPLETE_SQUID_COAST, 100.0f);
	}
	if (name == ELevelNames::creeperwoods) {
		WriteAchievements(EAchievement::COMPLETE_CREEPER_WOODS, 100.0f);
	}
	if (name == ELevelNames::obsidianpinnacle) {
		stats->obsidianPinnacle = true;
		WriteAchievements(EAchievement::DEFEAT_ARCH_ILLAGER_ANY, 100.0f);
		if (difficulty == EGameDifficulty::Difficulty_3) {
			WriteAchievements(EAchievement::DEFEAT_ARCH_ILLAGER_HARD, 100.0f);
			WriteAchievements(EAchievement::COMPLETE_OBSIDIAN_PINNACLE_APOCALYPSE5, PercentageCompleted(endlessStruggle.Value, achievementTarget()[EAchievement::COMPLETE_OBSIDIAN_PINNACLE_APOCALYPSE5]));
		}
	}
	if (name == ELevelNames::overgrowntemple && (difficulty == EGameDifficulty::Difficulty_2 || difficulty == EGameDifficulty::Difficulty_3)) {
		WriteAchievements(EAchievement::DEFEAT_JUNGLE_ABOMINATION_ADVENTURE, 100.0f);
	}
	if (name == ELevelNames::bamboobluff) {
		WriteAchievements(EAchievement::COMPLETE_PANDA_PLATEAU, 100.0f);
	}
	if (name == ELevelNames::lonelyfortress && (difficulty == EGameDifficulty::Difficulty_2 || difficulty == EGameDifficulty::Difficulty_3)) {
		WriteAchievements(EAchievement::DEFEAT_WRETCHED_WRAITH_ADVENTURE, 100.0f);
	}
	if (name == ELevelNames::lostsettlement) {
		WriteAchievements(EAchievement::COMPLETE_LOST_SETTLEMENT, 100.0f);
	}
	if (name == ELevelNames::galesanctum && (difficulty == EGameDifficulty::Difficulty_2 || difficulty == EGameDifficulty::Difficulty_3)) {
		WriteAchievements(EAchievement::COMPLETE_GALE_SANCTUM_ADVENTURE, 100.0f);
	}
	if (name == ELevelNames::endlessrampart) {
		WriteAchievements(EAchievement::COMPLETE_COLOSSAL_RAMPART, 100.0f);
	}
	if (name == ELevelNames::netherwastes && difficulty == EGameDifficulty::Difficulty_3) {
		WriteAchievements(EAchievement::COMPLETE_NETHER_WASTES_APOCALYPSE, 100.0f);
	}
	if (name == ELevelNames::abyssalmonument && difficulty == EGameDifficulty::Difficulty_3) {
		WriteAchievements(EAchievement::COMPLETE_ABYSSAL_MONUMENT_APOCALYPSE, 100.0f);
	}
	if (name == ELevelNames::radiantravine && endlessStruggle.Value > 0) {
		WriteAchievements(EAchievement::COMPLETE_RADIANT_RAVINE_APOCALYPSE_PLUS, 100.0f);
	}
	if (name == ELevelNames::thestronghold) {
		stats->stronghold = true;
	}
	if (name == ELevelNames::blightedcitadel) {
		stats->brokenCitadel = true;
		WriteAchievements(EAchievement::DEFEAT_BROKEN_HEART_OF_ENDER, 100.0f);
	}

	// level group 1
	if (!stats->pumpkinPastures && name == ELevelNames::pumpkinpastures) {
		stats->pumpkinPastures = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Pumpkin Pastures"));
	}
	if (!stats->soggySwamp && name == ELevelNames::soggyswamp) {
		stats->soggySwamp = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Soggy Swamp"));
	}
	if (stats->pumpkinPastures || stats->soggySwamp) {
		TArray<bool> booleans = { stats->pumpkinPastures, stats->soggySwamp };
		WriteAchievements(EAchievement::COMPLETE_LEVEL_GROUP1, BoolPercentage(booleans));
	}

	// level group 2
	if (!stats->redstoneMines && name == ELevelNames::mooncorecaverns) {
		stats->redstoneMines = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Redstone Mines"));
	}
	if (!stats->cactiCanyon && name == ELevelNames::cacticanyon) {
		stats->cactiCanyon = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Cacti Canyon"));
	}
	if (stats->redstoneMines || stats->cactiCanyon) {
		TArray<bool> booleans = { stats->redstoneMines, stats->cactiCanyon };
		WriteAchievements(EAchievement::COMPLETE_LEVEL_GROUP2, BoolPercentage(booleans));
	}

	// level group 3
	if (!stats->desertTemple && name == ELevelNames::deserttemple) {
		stats->desertTemple = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Desert Temple"));
	}
	if (!stats->fieryForge && name == ELevelNames::fieryforge) {
		stats->fieryForge = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Fiery Forge"));
	}
	if (!stats->highblockHalls && name == ELevelNames::highblockhalls) {
		stats->highblockHalls = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Highblock Halls"));
	}
	if (stats->desertTemple || stats->fieryForge || stats->highblockHalls) {
		TArray<bool> booleans = { stats->desertTemple, stats->fieryForge, stats->highblockHalls };
		WriteAchievements(EAchievement::COMPLETE_LEVEL_GROUP3, BoolPercentage(booleans));
	}

	// nether group
	if (!stats->crimsonForest && name == ELevelNames::crimsonforest && endlessStruggle.Value > 0) {
		stats->crimsonForest = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Crimson Forest Apocalypse Plus"));
	}
	if (!stats->soulsandValley && name == ELevelNames::soulsandvalley && endlessStruggle.Value > 0) {
		stats->soulsandValley = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Soulsand Valley Apocalypse Plus"));
	}
	if (!stats->netherFortress && name == ELevelNames::netherfortress && endlessStruggle.Value > 0) {
		stats->netherFortress = true;
		UE_LOG(LogTemp, Log, TEXT("Completed Nether Fortress Apocalypse Plus"));
	}
	if (stats->crimsonForest || stats->soulsandValley || stats->netherFortress) {
		TArray<bool> booleans = { stats->crimsonForest, stats->soulsandValley, stats->netherFortress };
		WriteAchievements(EAchievement::COMPLETE_NETHER_SECRET_APOCALYPSE_PLUS, BoolPercentage(booleans));
	}

	if (ocelotTracking) {
		if (name == ELevelNames::dingyjungle || name == ELevelNames::overgrowntemple || name == ELevelNames::bamboobluff) {
			WriteAchievements(EAchievement::OCELOT_ARMOUR, 100.0f);
		}
	}

	if (junglePotionTracking) {
		if (name == ELevelNames::dingyjungle) {
			WriteAchievements(EAchievement::COMPLETE_DINGY_JUNGLE_POTION, 100.0f);
		}
	}

	if (iceWeaponTracking) {
		if (name == ELevelNames::frozenfjord) {
			WriteAchievements(EAchievement::ICE_NO_WEAPONS, 100.0f);
		}
	}

	if (emeraldArmourTracking) {
		if (levelVariation == ELevelVariationType::daily && extraChallenge == EExtraChallenge::Challenge_3) {
			WriteAchievements(EAchievement::COMPLETE_DAILY_TRIAL_EMERALD_ARMOUR, 100.0f);
		}
	}

	// Complete two hypermissions & two daily trails in two-player co-op on the second day of the month.
	if (hyperMission || levelVariation == ELevelVariationType::daily) {
		UDungeonsGameInstance* gameInstance = GetOwner()->GetGameInstance<UDungeonsGameInstance>();
		const FDateTime dateTime = dateTime.Now();
		// is today the second day of the month and are there exactly two players in the game?
		if (dateTime.GetDay() == 2 && gameInstance->GetAllPlayerCharacters().Num() == 2) {
			TPair<bool, TPair<int32, int32>> mission;
			mission.Key = hyperMission;
			mission.Value.Key = dateTime.GetYear();
			mission.Value.Value = dateTime.GetMonth();
			stats->coopMissions.Emplace(mission);

			TArray<TPair<bool, TPair<int32, int32>>> toRemove;
			int hyperCount = 0;
			int dailyCount = 0;
			for (TPair<bool, TPair<int32, int32>>& entry : stats->coopMissions) {
				// if mission was not completed today, remove entry.
				if (entry.Value.Key != dateTime.GetYear() || entry.Value.Value != dateTime.GetMonth()) {
					toRemove.Emplace(mission);
				}
				else {
					// is mission a hypermission?
					if (entry.Key) {
						UE_LOG(LogTemp, Log, TEXT("HyperMission Complete - Year: %d, Month: %d"), mission.Value.Key, mission.Value.Value);
						hyperCount++;
						// if more than two hypermissions done today, remove extras.
						if (hyperCount > 2) {
							toRemove.Emplace(entry);
						}
					}
					// is mission a daily trail?
					else {
						UE_LOG(LogTemp, Log, TEXT("DailyMission Complete - Year: %d, Month: %d"), mission.Value.Key, mission.Value.Value);
						dailyCount++;
						// if more than two daily trails done today, remove extras.
						if (dailyCount > 2) {
							toRemove.Emplace(entry);
						}
					}
				}
			}

			// remove any extra or old mission entries.
			for (TPair<bool, TPair<int32, int32>>& entry : toRemove) {
				stats->coopMissions.RemoveSingle(entry);
			}

			WriteAchievements(EAchievement::HYPERMISSIONS_4, PercentageCompleted(stats->coopMissions.Num(), achievementTarget()[EAchievement::HYPERMISSIONS_4]));
		}
	}

	if (collectedTokens > 0) {
		stats->bannerMissions++;
		WriteAchievements(EAchievement::RAID_CAPTAIN_10, PercentageCompleted(stats->bannerMissions, achievementTarget()[EAchievement::RAID_CAPTAIN_10]));
	}

	//D11.GM 
	if (name == ELevelNames::blightedcitadel && !shulkerHit) {
		WriteAchievements(EAchievement::REACH_TOP_OF_BROKEN_CITADEL_NO_HIT_SHULKERS_BULLET, 100.f);
	}

	Save();
}

void UStatTrackerComponent::StartLevel(ELevelNames name) {
	if (name == ELevelNames::enderwilds) {
		WriteAchievements(EAchievement::TRAVEL_THROUGH_END_PORTAL, 100.0f);
	}
}

void UStatTrackerComponent::Enchant(int level) {
	WriteAchievements(EAchievement::FULLY_UPGRADE_ENCHANTMENT, PercentageCompleted(level, achievementTarget()[EAchievement::FULLY_UPGRADE_ENCHANTMENT]));
}

void UStatTrackerComponent::GearChange(const TArray<FEnchantmentDataWithRarity>& enchantments) {
	// replaced retlambda to make count work with duplicate enchantments as internally these enchantments are stacked and represented by a single enchantment.
	int count = 0;
	for (const auto& enchantment : enchantments) {
		if (enchantment.Enchantment.Source == EEnchantmentSource::Generated) {
			count += enchantment.Enchantment.Level / 3;
		}
	}
	WriteAchievements(EAchievement::FULL_ENCHANTED_GEAR_SET, PercentageCompleted(count, achievementTarget()[EAchievement::FULL_ENCHANTED_GEAR_SET]));

	auto counter = [&enchantments](EEnchantmentCategory category) {
		int count = 0;
		for (const auto& enchantment : enchantments) {
			if (enchantment.Enchantment.Category == category && enchantment.Enchantment.Source == EEnchantmentSource::Generated) {
				count += enchantment.Enchantment.Level / 3;
			}
		}
		return count +
			(algo::any_of(enchantments, RETLAMBDA(it.Enchantment.Category == category && it.Enchantment.Source == EEnchantmentSource::Netherite)) ? 1 : 0) +
			(algo::any_of(enchantments, RETLAMBDA(it.Enchantment.Category == category && it.Rarity == EItemRarity::Unique)) ? 1 : 0);
	};
	WriteAchievements(EAchievement::EFFECTS_TRIGGER_5, PercentageCompleted(std::max(counter(EEnchantmentCategory::Melee), counter(EEnchantmentCategory::Ranged)), achievementTarget()[EAchievement::EFFECTS_TRIGGER_5]));

	TileChange(currentTile, true);
}

void UStatTrackerComponent::OpenFancyChest(EItemRarityChanceCategory rarity) {
	if (rarity == EItemRarityChanceCategory::FancyChest) {
		WriteAchievements(EAchievement::FANCY_CHEST, 100.0f);
	}
}

void UStatTrackerComponent::TNT(int kills) {
	stats->TNTKills += kills;
	UnlockTitle(ETitle::BOOMER, PercentageCompleted(stats->TNTKills, titleTarget()[ETitle::BOOMER]));
	WriteAchievements(EAchievement::TNT_KILL_10, PercentageCompleted(kills, 10));
}

void UStatTrackerComponent::OcelotArmour(bool tracking) {
	ocelotTracking = tracking;
}

void UStatTrackerComponent::IceWeapon(bool tracking) {
	iceWeaponTracking = tracking;
}

void UStatTrackerComponent::IceWand(int kills) {
	WriteAchievements(EAchievement::ICE_WAND_KILL_5, PercentageCompleted(kills, achievementTarget()[EAchievement::ICE_WAND_KILL_5]));
}

void UStatTrackerComponent::IceSliding(bool sliding) {
	iceSliding = sliding;
}

void UStatTrackerComponent::OnMeleeAttack(TArray<EntityType> killedMobs) {
	int64 timeStamp = FDateTime::UtcNow().ToUnixTimestamp();
	bool equippedTempestKnife = false;

	if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
		if (UEquipmentComponent* equipmentComponent = character->GetEquipmentComponent()) {
			UItemSlot& meleeWeaponSlot = equipmentComponent->GetSlot(ESlotType::MeleeWeapon);
			if (AItemInstance* meleeWeapon = meleeWeaponSlot.GetItem()) {
				if (meleeWeapon->GetItemType().getId() == game::item::type::TempestKnife.getId()) {
					equippedTempestKnife = true;
				}
			}
		}

		//D11.RR - 'Burning Up' Achievement
		if (UEnchantmentComponent* enchanmentComponent = character->GetEnchantmentComponent()) {
			if (enchanmentComponent->GetEnchantment(EEnchantmentTypeID::FireAspect) && burningBrewPotionTracking && killedMobs.Num() > 0) {
				WriteAchievements(EAchievement::BURNING_BREW_DEFEAT_MOB_WITH_FIRE_ENCHANTMENT, 100.0f);
				burningBrewPotionTracking = false;
			}
		}				
	}

	bool checkTempestKnife = false;
	for (EntityType mobType : killedMobs) {
		if (equippedTempestKnife && (mobType == EntityType::Mountaineer || mobType == EntityType::MountaineerVariant0 || mobType == EntityType::MountaineerVariant1 || mobType == EntityType::MountaineerVariant2 || mobType == EntityType::RampartCaptain)) {
			killedTempestKnifeMountaineers.Emplace(timeStamp);
			checkTempestKnife = true;
		}
	}

	if (checkTempestKnife) {
		TArray<int64> toRemove;
		int count = 0;
		for (int64 time : killedTempestKnifeMountaineers) {
			if (time >= timeStamp - 10) {
				count++;
			}
			else {
				toRemove.Emplace(time);
			}
		}

		for (int64 time : toRemove) {
			killedTempestKnifeMountaineers.Remove(time);
		}

		WriteAchievements(EAchievement::KILL_10_MOUNTAINEER_TEMPEST_KNIFE_TIMED, PercentageCompleted(count, achievementTarget()[EAchievement::KILL_10_MOUNTAINEER_TEMPEST_KNIFE_TIMED]));
	}
}

void UStatTrackerComponent::UpdraftWindCaller(int count) {
	WriteAchievements(EAchievement::UPDRAFT_WINDCALLERS_3, PercentageCompleted(count, achievementTarget()[EAchievement::UPDRAFT_WINDCALLERS_3]));
}

void UStatTrackerComponent::LlamaOffering() {
	if (currentTile == "gs_sub_llamas_shrine") {
		WriteAchievements(EAchievement::LLAMA_OFFERING, 100.0f);
	}
}

void UStatTrackerComponent::EmeraldArmour(bool tracking) {
	emeraldArmourTracking = tracking;
}

void UStatTrackerComponent::DeathBarter() {
	deathBarterCount++;
	WriteAchievements(EAchievement::DEATH_BARTER_2, PercentageCompleted(deathBarterCount, achievementTarget()[EAchievement::DEATH_BARTER_2]));
}

void UStatTrackerComponent::CheckUnlockedMerchants() {
	if (UCharacterSerializeComponent* CSComponent = Cast<APlayerCharacter>(GetOwner())->GetCharacterSerializeComponent()) {
		int count = algo::count_if(merchantdefs::getAllEnabledMerchants(), RETLAMBDA(CSComponent->HasUnlockedMerchant(it)));
		WriteAchievements(EAchievement::MERCHANTS_6, PercentageCompleted(count, achievementTarget()[EAchievement::MERCHANTS_6]));
	}
}

void UStatTrackerComponent::SpinWheel(int hits) {
	UE_LOG(LogTemp, Log, TEXT("Spinwheel hits: %d"), hits);
	WriteAchievements(EAchievement::SPINWHEEL_70, PercentageCompleted(hits, achievementTarget()[EAchievement::SPINWHEEL_70]));
}

void UStatTrackerComponent::TileChange(const std::string& tileId, bool equipmentChange) {
	if (equipmentChange) {
		if (!GetWorld()->GetTimerManager().IsTimerActive(tileCheckTimerHandle)) {
			GetWorld()->GetTimerManager().SetTimer(tileCheckTimerHandle, this, &UStatTrackerComponent::EquipmentChange, 1.0f, false);
		}
		return;
	}

	if (currentTile != tileId) {
		currentTile = tileId;
		UE_LOG(LogTemp, Log, TEXT("Tile ID = %s"), UTF8_TO_TCHAR(tileId.c_str()));

		ABaseCharacter* character = Cast<ABaseCharacter>(GetOwner());
		auto hasPet = [character](EntityType type) {
			return algo::any_of(actorquery::getActors<AMobCharacter>(character->GetWorld()).FilterByPredicate([character](AMobCharacter* mob) {return mob->GetMaster() == character;}), RETLAMBDA(it->EntityType == type));
		};

		if (currentTile == WOLF_TILE && hasPet(EntityType::Wolf)) {
			stats->wolfHome = true;
		}
		else if (currentTile == BAT_TILE && hasPet(EntityType::Bat)) {
			stats->batHome = true;
		}
		else if (currentTile == SOUL_TILE && hasPet(EntityType::SoulWizard)) {
			stats->wraithHome = true;
		}

		TArray<bool> booleans = { stats->wolfHome, stats->batHome, stats->wraithHome };
		WriteAchievements(EAchievement::HOMECOMING, BoolPercentage(booleans));

		if (currentTile == "sh_sub_svault_vault") {
			WriteAchievements(EAchievement::ENTER_STRONGHOLD_VAULT, 100.0f);
		}

		if (GetWorld()->GetTimerManager().IsTimerActive(tileCheckTimerHandle)) {
			GetWorld()->GetTimerManager().ClearTimer(tileCheckTimerHandle);
		}
	}
}

void UStatTrackerComponent::EquipmentChange() {
	ABaseCharacter* character = Cast<ABaseCharacter>(GetOwner());
	auto hasPet = [character](EntityType type) {
		return algo::any_of(actorquery::getActors<AMobCharacter>(character->GetWorld()).FilterByPredicate([character](AMobCharacter* mob) {return mob->GetMaster() == character;}), RETLAMBDA(it->EntityType == type));
	};

	if (currentTile == WOLF_TILE && hasPet(EntityType::Wolf)) {
		stats->wolfHome = true;
	}
	else if (currentTile == BAT_TILE && hasPet(EntityType::Bat)) {
		stats->batHome = true;
	}
	else if (currentTile == SOUL_TILE && hasPet(EntityType::SoulWizard)) {
		stats->wraithHome = true;
	}

	TArray<bool> booleans = { stats->wolfHome, stats->batHome, stats->wraithHome };
	WriteAchievements(EAchievement::HOMECOMING, BoolPercentage(booleans));
	tileCheckTimerHandle.Invalidate();
}

void UStatTrackerComponent::GlowSquid() {
	WriteAchievements(EAchievement::GLOWSQUID_ICEWAND, 100.0f);
}

void UStatTrackerComponent::Hooked(EntityType& mobEntityType) {
	if (hasMobTag(mobEntityType, MobTags::HashTag_Underwater)) {
		switch (mobEntityType) {
		case EntityType::TropicalSlimeLarge:
		case EntityType::TropicalSlimeMedium:
		case EntityType::TropicalSlimeSmall:
			mobEntityType = EntityType::TropicalSlimeLarge;
			break;
		case EntityType::Drowned:
		case EntityType::DrownedVariant0:
		case EntityType::DrownedVariant1:
		case EntityType::DrownedVariant2:
		case EntityType::TridentDrowned:
		case EntityType::TridentDrownedVariant0:
		case EntityType::TridentDrownedVariant1:
		case EntityType::TridentDrownedVariant2:
			mobEntityType = EntityType::Drowned;
			break;
		case EntityType::SunkenSkeleton:
		case EntityType::SunkenSkeletonVariant0:
		case EntityType::SunkenSkeletonVariant1:
		case EntityType::SunkenSkeletonVariant2:
			mobEntityType = EntityType::SunkenSkeleton;
			break;
		case EntityType::Turtle:
		case EntityType::BabyTurtle:
			mobEntityType = EntityType::Turtle;
			break;
		}

		if (underwaterMobs().Contains(mobEntityType)) {
			stats->underwaterHooks.AddUnique(mobEntityType);
			stats->underwaterHooks.RemoveAll([](const EntityType& type) {return !underwaterMobs().Contains(type);});
			WriteAchievements(EAchievement::FISHING_ROD, PercentageCompleted(stats->underwaterHooks.Num(), underwaterMobs().Num()));
		}
	}
}

void UStatTrackerComponent::BubbleColumn(bool entered) {
	bubbleColumnTracking = entered;
	timeInBubbleColumn = 0.0f;
}

void UStatTrackerComponent::Drowning(bool isDrowning) {
	drowningTracking = isDrowning;
	timeDrowning = 0.0f;
}

void UStatTrackerComponent::Trident(int kills) {
	WriteAchievements(EAchievement::TRIDENT, PercentageCompleted(kills, achievementTarget()[EAchievement::TRIDENT]));
}

void UStatTrackerComponent::Bubbled(bool bubbled) {
	if (bubbled) {
		bubbledEnemiesCount++;
		WriteAchievements(EAchievement::BUBBLE_BOW, PercentageCompleted(bubbledEnemiesCount, achievementTarget()[EAchievement::BUBBLE_BOW]));
	}
	else {
		bubbledEnemiesCount = std::max(0, bubbledEnemiesCount - 1);
	}
}

void UStatTrackerComponent::ShulkerHit(bool hit) {
	shulkerHit = hit;
}

void UStatTrackerComponent::LostConduit() {
	if (currentTile == "rr_darkconduit_finale") {
		WriteAchievements(EAchievement::LOST_CONDUIT, 100.0f);
	}
}

void UStatTrackerComponent::ElytraAttack(int hits) {
	WriteAchievements(EAchievement::ELYTRA_DIVE_ATTACK_12_ENEMIES_AT_ONCE, PercentageCompleted(hits, achievementTarget()[EAchievement::ELYTRA_DIVE_ATTACK_12_ENEMIES_AT_ONCE]));
}

void UStatTrackerComponent::SquidEasterEgg() {
	WriteAchievements(EAchievement::SQUID_COAST_EASTER_EGG, 100.f);
}

void UStatTrackerComponent::InitAllItems() {
	if (allItems.Num() == 0) {
		TSet<FItemId> uniqueItems;
		for (int idx = 0; idx < allLevels().Num(); ++idx) {
			const ELevelNames level = static_cast<ELevelNames>(allLevels()[idx]);
			const game::Settings settings = game::Settings(EGameDifficulty::Difficulty_3, 0, level);

			for (const FItemId& item : game::item::generator::getPossibleItemTypeIds(settings, itemgen::looters::Unknown(), itemgen::configs::MissionInspectorGear())) {
				uniqueItems.Add(GetItemRegistry().Get(item).getRoot()->getId());
			}

			for (const FMissionItemTypeChance& reward : missions::get(level).rewards().getPossibleItemRarityTypes(FMissionDifficulty{ level, EGameDifficulty::Difficulty_3 })) {
				const ItemType* rootType = GetItemRegistry().Get(reward.ItemId).getRoot();
				if (!rootType->isConsumable() && !rootType->isInstant()) {
					uniqueItems.Add(rootType->getId());
				}
			}
		}

		allItems = uniqueItems.Array();
	}
}

void UStatTrackerComponent::UnlockedItemsTracking() {
	for (const FItemId& item : allItems) {
		unlockedItems.Add(item, false);
	}

	if (const UCharacterSerializeComponent* characterSave = Cast<APlayerCharacter>(GetOwner())->GetCharacterSerializeComponent()) {
		for (const FItemId& item : characterSave->ReadItemsFound().Array()) {
			if (bool* foundVal = unlockedItems.Find(GetItemRegistry().Get(item).getRoot()->getId())) {
				if (!(*foundVal)) {
					*foundVal = true;
					unlockedItemsCount++;
				}
			}
		}
	}

	if (unlockedItemsCount > retrievedUnlockedItemsCount) {
		WriteAchievements(EAchievement::UNLOCK_ALL_ITEMS_BASE_AND_DLC, PercentageCompleted(unlockedItemsCount, allItems.Num()));
	}
}

void UStatTrackerComponent::GotItem(const FItemId &item) {
	if (bool* foundVal = unlockedItems.Find(GetItemRegistry().Get(item).getRoot()->getId())) {
		if (!(*foundVal)) {
			*foundVal = true;
			unlockedItemsCount++;
			if (unlockedItemsCount > retrievedUnlockedItemsCount) {
				WriteAchievements(EAchievement::UNLOCK_ALL_ITEMS_BASE_AND_DLC, PercentageCompleted(unlockedItemsCount, allItems.Num()));
			}
		}
	}
}

void UStatTrackerComponent::Voided(float magnitude)
{
	WriteAchievements(EAchievement::SURVIVE_AFFLICTED_BY_VOIDED_X999, PercentageCompleted(magnitude, achievementTarget()[EAchievement::SURVIVE_AFFLICTED_BY_VOIDED_X999]));
}

void UStatTrackerComponent::BurningBrew(bool isActive)
{
	burningBrewPotionTracking = isActive;
}

void UStatTrackerComponent::Heal(float amount) {
	stats->healed += amount;
	UE_LOG(LogTemp, Log, TEXT("healed %d health points"), stats->healed);
}

void UStatTrackerComponent::OnDeath() {
	stats->deaths++;
	junglePotionTracking = false;
	UE_LOG(LogTemp, Log, TEXT("died %d times"), stats->deaths);
}

void UStatTrackerComponent::UpdateTravel(float distance) {
	stats->distanceTravelled += (distance / 1000.0f); // convert to kilometres
	if (iceSliding) {
		stats->blocksSlid += distance;
		if (showIceSlideDistanceDebugMessage) {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Distance Slid (metres): %f"), stats->blocksSlid));
		}
		WriteAchievements(EAchievement::ICE_SLIDE_500, PercentageCompleted(stats->blocksSlid, achievementTarget()[EAchievement::ICE_SLIDE_500]));
	}
}

void UStatTrackerComponent::UpdateTime(float deltaSeconds) {
	stats->timePlayed += deltaSeconds;
}

void UStatTrackerComponent::UseItem(const ItemType& typeInfo) {
	const FItemId type = typeInfo.getId();

	if (!consumableItems().Contains(type) && !foodItems().Contains(type)) {
		stats->usedArtefacts++;
		UE_LOG(LogTemp, Log, TEXT("used %d artifacts"), stats->usedArtefacts);
	}

	if (!stats->wolf && type == game::item::type::TastyBone.getId()) {
		stats->wolf = true;
		UE_LOG(LogTemp, Log, TEXT("Summoned a wolf"));
	}
	if (!stats->llama && type == game::item::type::WonderfulWheat.getId()) {
		stats->llama = true;
		UE_LOG(LogTemp, Log, TEXT("Summoned a llama"));
	}
	if (!stats->golem && type == game::item::type::GolemKit.getId()) {
		stats->golem = true;
		UE_LOG(LogTemp, Log, TEXT("Summoned a golem"));
	}
	if (stats->wolf || stats->llama || stats->golem) {
		TArray<bool> booleans = { stats->wolf, stats->llama, stats->golem };
		WriteAchievements(EAchievement::BEAST_MASTER, BoolPercentage(booleans));
	}

	if (foodItems().Contains(type)) {
		stats->foodEaten++;
		UE_LOG(LogTemp, Log, TEXT("%d food items eaten"), stats->foodEaten);
		if (showFoodEatenDebugMessage) {
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::White, FString::Printf(TEXT("Food Eaten: %d"), stats->foodEaten));
		}
		WriteAchievements(EAchievement::EAT_200_FOOD, PercentageCompleted(stats->foodEaten, achievementTarget()[EAchievement::EAT_200_FOOD]));
	}

	if (junglePotionTracking && type == game::item::type::HealthPotion.getId()) {
		junglePotionTracking = false;
	}

	if (type == game::item::type::GongOfWeakening.getId()) {
		int64 timeStamp = FDateTime::UtcNow().ToUnixTimestamp();
		gongActivations.Emplace(timeStamp);

		int count = 0;
		TArray<int64> toRemove;
		for (int64 time : gongActivations) {
			if (time >= timeStamp - 5) {
				count++;
			}
			else {
				toRemove.Emplace(time);
			}
		}

		for (int64 time : toRemove) {
			gongActivations.Remove(time);
		}

		WriteAchievements(EAchievement::GONG_6, PercentageCompleted(count, achievementTarget()[EAchievement::GONG_6]));
	}

	if (type == game::item::type::BurningBrewPotion.getId()) {
		burningBrewPotionTracking = true;
	}

	TileChange(currentTile, true);
}

void UStatTrackerComponent::GotCurrency(const FItemId& type, int amount) {
	if (type == game::item::type::Emerald.getId()) {
		stats->emeralds += amount;
		UE_LOG(LogTemp, Log, TEXT("%d emeralds collected"), stats->emeralds);
		WriteAchievements(EAchievement::EMERALD_1000, PercentageCompleted(stats->emeralds, achievementTarget()[EAchievement::EMERALD_1000]));
		WriteAchievements(EAchievement::EMERALD_5000, PercentageCompleted(stats->emeralds, achievementTarget()[EAchievement::EMERALD_5000]));
	}
	if (type == game::item::type::Gold.getId()) {
		stats->gold += amount;
		UE_LOG(LogTemp, Log, TEXT("%d gold collected"), stats->gold);
		WriteAchievements(EAchievement::COLLECT_500_GOLD, PercentageCompleted(stats->gold, achievementTarget()[EAchievement::COLLECT_500_GOLD]));
	}
}

void UStatTrackerComponent::LevelUp(int level) {
	if (level <= 0 || uint32(level) <= stats->level) {
		return;
	}

	stats->level = uint32(level);
	UE_LOG(LogTemp, Log, TEXT("level %d achieved"), stats->level);

	WriteAchievements(EAchievement::LVL_10, PercentageCompleted(stats->level, achievementTarget()[EAchievement::LVL_10]));
	WriteAchievements(EAchievement::LVL_25, PercentageCompleted(stats->level, achievementTarget()[EAchievement::LVL_25]));
	WriteAchievements(EAchievement::LVL_50, PercentageCompleted(stats->level, achievementTarget()[EAchievement::LVL_50]));
}

void UStatTrackerComponent::OpenChest() {
	stats->chestsOpened++;
	UE_LOG(LogTemp, Log, TEXT("%d chests opened"), stats->chestsOpened);
	WriteAchievements(EAchievement::CHEST_100, PercentageCompleted(stats->chestsOpened, achievementTarget()[EAchievement::CHEST_100]));
}

void UStatTrackerComponent::RegisterKill(EntityType mobEntityType, bool enchanted) {
	stats->mobKills++;
	UE_LOG(LogTemp, Log, TEXT("%d mob kills"), stats->mobKills);

	WriteAchievements(EAchievement::KILL_50_MOBS, PercentageCompleted(stats->mobKills, achievementTarget()[EAchievement::KILL_50_MOBS]));
	WriteAchievements(EAchievement::KILL_2500_MOBS, PercentageCompleted(stats->mobKills, achievementTarget()[EAchievement::KILL_2500_MOBS]));

	if (mobEntityType == EntityType::Endling || mobEntityType == EntityType::Blastling || mobEntityType == EntityType::Snareling)
	{
		if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
			if (UEquipmentComponent* equipmentComponent = character->GetEquipmentComponent()) {
				UItemSlot& meleeWeaponSlot = equipmentComponent->GetSlot(ESlotType::MeleeWeapon);
				if (AItemInstance* meleeWeapon = meleeWeaponSlot.GetItem()) {
					if (meleeWeapon->GetItemType().getId() == game::item::type::ObsidianClaymore.getId())
					{
						WriteAchievements(EAchievement::DEFEAT_1000_ENDERLINGS_WITH_OBSIDIAN_BLADE, PercentageCompleted(++stats->endlerlingKills, achievementTarget()[EAchievement::DEFEAT_1000_ENDERLINGS_WITH_OBSIDIAN_BLADE]));
					}
				}
			}
		}
	}

	if (hasMobTag(mobEntityType, MobTags::HashTag_Passive)) {
		stats->passiveKills++;
		UE_LOG(LogTemp, Log, TEXT("%d passive mob kills"), stats->passiveKills);
		WriteAchievements(EAchievement::KILL_50_PASSIVE, PercentageCompleted(stats->passiveKills, achievementTarget()[EAchievement::KILL_50_PASSIVE]));
	}

	if (enchanted) {
		stats->enchantedKills++;
		UE_LOG(LogTemp, Log, TEXT("%d enchanted mob kills"), stats->enchantedKills);
		WriteAchievements(EAchievement::KILL_50_ENCHANTED, PercentageCompleted(stats->enchantedKills, achievementTarget()[EAchievement::KILL_50_ENCHANTED]));
	}

	if (mobEntityType == EntityType::MooshroomMonstrosity) {
		if (auto* equipmentComponent = Cast<APlayerCharacter>(GetOwner())->GetEquipmentComponent()) {
			TArray<UItemSlot*> itemSlots = equipmentComponent->GetEquippableSlots();
			int count = algo::count_if(itemSlots, RETLAMBDA(it->GetItem() && !jungleItems().Contains(it->GetItem()->GetItemId())));
			if (count <= 0) {
				WriteAchievements(EAchievement::DEFEAT_MOOSHROOM_MONSTROSITY_ITEMS, 100.0f);
			}
		}
	}

	if (ocelotTracking && mobEntityType == EntityType::Ocelot) {
		ocelotTracking = false;
	}

	if (mobEntityType == EntityType::Ravager || mobEntityType == EntityType::SquallGolem) {
		int64 timeStamp = FDateTime::UtcNow().ToUnixTimestamp();
		killedGiantSlayerEnemies.Emplace(mobEntityType, timeStamp);

		bool ravager = false;
		bool golem = false;
		TArray<TPair<EntityType, int64>> toRemove;
		for (TPair<EntityType, int64> enemy : killedGiantSlayerEnemies) {
			if (enemy.Value >= timeStamp - 5) {
				switch (enemy.Key) {
				case EntityType::Ravager: ravager = true; break;
				case EntityType::SquallGolem: golem = true; break;
				}
			}
			else {
				toRemove.Emplace(enemy);
			}
		}

		for (TPair<EntityType, int64> enemy : toRemove) {
			killedGiantSlayerEnemies.Remove(enemy);
		}

		TArray<bool> booleans = { ravager, golem };
		WriteAchievements(EAchievement::KILL_RAVAGER_SQUALL_GOLEM_TIMED, BoolPercentage(booleans));
	}

	if (mobEntityType == EntityType::TempestGolem) {
		ABaseCharacter* character = Cast<ABaseCharacter>(GetOwner());
		TArray<AMobCharacter*> pets = actorquery::getActors<AMobCharacter>(GetWorld()).FilterByPredicate([character](AMobCharacter* mob){return mob->GetMaster() == character;});
		WriteAchievements(EAchievement::DEFEAT_TEMPEST_GOLEM_PETS, PercentageCompleted(pets.Num(), achievementTarget()[EAchievement::DEFEAT_TEMPEST_GOLEM_PETS]));
	}

	if (mobEntityType == EntityType::Creeper || mobEntityType == EntityType::ChargedCreeper || mobEntityType == EntityType::IcyCreeper) {
		stats->creeperKills++;
		UnlockTitle(ETitle::CREEPERS_BANE, PercentageCompleted(stats->creeperKills, titleTarget()[ETitle::CREEPERS_BANE]));
	}

	if (hasMobTag(mobEntityType, MobTags::HashTag_Ancient)) {
		stats->ancientKills.AddUnique(mobEntityType);
		WriteAchievements(EAchievement::DEFEAT_ANCIENT_15, PercentageCompleted(stats->ancientKills.Num(), achievementTarget()[EAchievement::DEFEAT_ANCIENT_15]));
	}
}

void UStatTrackerComponent::ReviveFriend() {
	stats->revives++;
	UE_LOG(LogTemp, Log, TEXT("%d player revives"), stats->revives);
	WriteAchievements(EAchievement::REVIVE_20, PercentageCompleted(stats->revives, achievementTarget()[EAchievement::REVIVE_20]));
}

void UStatTrackerComponent::OnGameOver(bool isGameOver) {
	if (isGameOver) {
		if (APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner())) {
			if (UCharacterSerializeComponent* serializeComponent = character->GetCharacterSerializeComponent()) {
				if (serializeComponent->GetLegendaryStatus() == ELegendaryStatus::LEGENDARY) {
					serializeComponent->SetLegendaryStatus(ELegendaryStatus::MYTHICAL);
				}
			}
		}
	}
}

void UStatTrackerComponent::Save() {
	if (auto* controller = Cast<APlayerCharacter>(GetOwner())->GetPlayerController()) {
		int systemUserID = controller->GetSaveLocalUserNum();
		if (UGlobalStateData* saveData = GetOwner()->GetGameInstance<UDungeonsGameInstance>()->EditGlobalSaveState(systemUserID))
		{
			saveData->Save(systemUserID);
		}
#if PLATFORM_XBOXONE
		if (!online::getIdentityInterface().IsValid()) {
			return;
		}
		FXboxOneApplication* xboxOneApp = FXboxOneApplication::GetXboxOneApplication();
		if (xboxOneApp == nullptr) {
			return;
		}

		auto xboxInputInterface = xboxOneApp->GetXboxInputInterface();

		FPlatformUserId platformUserId = online::getIdentityInterface()->GetPlatformUserIdFromUniqueNetId(*netID);
		Windows::Xbox::System::User^ requestingUser = xboxInputInterface->GetXboxUserFromPlatformUserId(platformUserId);

		if (!ensure(requestingUser != nullptr)) {
			UE_LOG_ONLINE_EXTERNALUI(Warning, TEXT("Passed in invalider requester to"));
			return;
		}
#endif
		WriteStats();
	}
}

const TrackedStats& UStatTrackerComponent::GetStats() const { 
	return *stats; 
}

// Check if query completed successfully & set achievement flags according to backend
void UStatTrackerComponent::OnQueryAchievementsComplete(const FUniqueNetId& playerId, bool bWasSuccessful) {
#if ACHIEVEMENTS_ENABLED
	auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF();
	if (bWasSuccessful && onlineAchievements.IsValid()) {
		retryAchievementQuery = false;
		UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Loaded achievements"));
		userAchievements.Empty();
		TArray<FOnlineAchievement> playerAchievements;
		if (onlineAchievements->GetCachedAchievements(*netID.Get(), playerAchievements) != EOnlineCachedResult::Success || playerAchievements.Num() == 0) {
			UE_LOG_ONLINE_ACHIEVEMENTS(Warning, TEXT("LOAD FAILED: Either GetCachedAchievements() failed or number of achievements is 0"));
			for (uint8 i = static_cast<uint8>(EAchievement::INVALID) + 1; i < static_cast<uint8>(EAchievement::ENUM_END); i++) {
				EAchievement achievement = static_cast<EAchievement>(i);
				if (stats->cachedAchievements.test(static_cast<uint64>(achievement))) {
					cachedAchievements.Add(achievement, 100);
				}
			}
			return;
		}

		UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Number of Achievements: %d"), playerAchievements.Num());
		int count = 0;
		for (auto& achievement : playerAchievements) {
			count++;
			EAchievement achievementEnum = static_cast<EAchievement>(count);
			userAchievements.Add(achievementEnum, static_cast<float>(achievement.Progress));

			UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("EAchievement : %s"), *achievement.ToDebugString());
			if (FMath::IsNearlyEqual(static_cast<float>(achievement.Progress), 100.0f)) {
				stats->achievements.set(static_cast<uint64>(achievementEnum), 1);
				stats->cachedAchievements.set(static_cast<uint64>(achievementEnum), 0);
			}
			else {
				// if achievement marked as completed or cached but is not unlocked in backend, previous write must have failed
				if (stats->achievements.test(static_cast<uint64>(achievementEnum)) || stats->cachedAchievements.test(static_cast<uint64>(achievementEnum))) {
					stats->achievements.set(static_cast<uint64>(achievementEnum), 1);
					stats->cachedAchievements.set(static_cast<uint64>(achievementEnum), 1);
					cachedAchievements.Add(achievementEnum, 100);
				}

				// use achievement progress to retrieve rough stat values in event of save data loss
				if (achievementTarget().Contains(achievementEnum)) {
					switch (achievementEnum) {
					case EAchievement::REVIVE_20: stats->revives = std::max(stats->revives, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::KILL_2500_MOBS: stats->mobKills = std::max(stats->mobKills, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::KILL_50_PASSIVE: stats->passiveKills = std::max(stats->passiveKills, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::KILL_50_ENCHANTED: stats->enchantedKills = std::max(stats->enchantedKills, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::LVL_50: stats->level = std::max(stats->level, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::CHEST_100: stats->chestsOpened = std::max(stats->chestsOpened, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::EMERALD_5000: stats->emeralds = std::max(stats->emeralds, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::EAT_200_FOOD: stats->foodEaten = std::max(stats->foodEaten, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::ICE_SLIDE_500: stats->blocksSlid = std::max(stats->blocksSlid, float(PercentToValue(achievement.Progress, achievementTarget()[achievementEnum]))); break;
					case EAchievement::COLLECT_500_GOLD: stats->gold = std::max(stats->gold, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					case EAchievement::RAID_CAPTAIN_10: stats->bannerMissions = std::max(stats->bannerMissions, PercentToValue(achievement.Progress, achievementTarget()[achievementEnum])); break;
					}
				}
				else if (achievementEnum == EAchievement::DEFEAT_ARCH_ILLAGER_ANY) {
					stats->obsidianPinnacle = true;
				}
				else if (achievementEnum == EAchievement::UNLOCK_ALL_ITEMS_BASE_AND_DLC) {
					retrievedUnlockedItemsCount = std::max(retrievedUnlockedItemsCount, PercentToValue(achievement.Progress, allItems.Num()));
				}
			}
		}
		WriteCachedAchievements();
		UnlockedItemsTracking();
	}
	else {
		retryAchievementQuery = true;
		UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Failed to Load Achievements"));
	}
#endif
}

// write achievements to backend
void UStatTrackerComponent::WriteAchievements(EAchievement achievement, float percentage) {
#if ACHIEVEMENTS_ENABLED
	if (!stats->achievements.test(static_cast<uint64>(achievement))) {
		// only write achievement progress if progress is divisible by 10, greater than the previous percentage by 10, or if progress is 100
		// done to reduce the total number of writes
		if ((online::getCrossplayOss()->IsPS4Active() || online::getCrossplayOss()->IsSteamActive()) && FMath::IsNearlyEqual(percentage, 100.0f)) {
			WriteAchievementsPlatform(achievement, percentage);
		}
		else if (FMath::IsNearlyEqual(percentage, 100.0f) || !userAchievements.Contains(achievement) || percentage >= userAchievements[achievement] + 10.0f || (percentage > userAchievements[achievement] && static_cast<int>(percentage) % 10 == 0 && static_cast<int>(percentage) != 0)) {
			WriteAchievementsPlatform(achievement, percentage);
		}
#endif
	}
}

void UStatTrackerComponent::WriteAchievementsPlatform(EAchievement achievement, float percentage) {
	if (auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF()) {
		if (!retryAchievementQuery) {
			FOnlineAchievementsWritePtr writeObject = MakeShareable(new FOnlineAchievementsWrite());
			writeObject->SetFloatStat(FName(*GetEnumValueToStringStripped(achievement)), percentage);
			FOnlineAchievementsWriteRef writeObjectRef = writeObject.ToSharedRef();
			onlineAchievements->WriteAchievements(*netID.Get(), writeObjectRef);

			float& userAchievement = userAchievements.FindOrAdd(achievement);
			userAchievement = percentage;
		}
		else {
			cachedAchievements.Emplace(achievement, percentage);
		}
		if (FMath::IsNearlyEqual(percentage, 100.0f)) {
			// no longer need to track for this achievement
			stats->achievements.set(static_cast<uint64>(achievement), 1);
			stats->cachedAchievements.set(static_cast<uint64>(achievement), 0);
			UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Achievement Criteria Met: %d"), achievement);
			Save();
		}
		
	}
}

// write cached achievements
void UStatTrackerComponent::WriteCachedAchievements() {
#if ACHIEVEMENTS_ENABLED
	if (auto onlineAchievements = online::getCrossplayOss()->GetAchievementsIF()) {
		for (auto achievement : cachedAchievements) {
			FOnlineAchievementsWritePtr writeObject = MakeShareable(new FOnlineAchievementsWrite());
			writeObject->SetFloatStat(FName(*GetEnumValueToStringStripped(achievement.Key)), achievement.Value);
			FOnlineAchievementsWriteRef writeObjectRef = writeObject.ToSharedRef();
			onlineAchievements->WriteAchievements(*netID.Get(), writeObjectRef);
			UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Attempted Cached Achievement Rewrite: %d"), achievement.Key);
		}
		cachedAchievements.Empty();
	}
#endif
}

// set achievement as unlocked
void UStatTrackerComponent::OnAchievementWritten(bool success, TSharedPtr<const FUniqueNetId> playerID, int achievementID, FString achievementName, float percentage, FString achievementURL, FString achievementTitle, FString achievementDesc) {
	EAchievement achievementEnum = static_cast<EAchievement>(achievementID);
	if (playerID == netID) {
		if (success) {
			if (percentage == 100.0f) {
				UE_LOG_ONLINE_ACHIEVEMENTS(Display, TEXT("Achievement Unlocked: %d"), achievementID);
				GetOwner()->GetGameInstance<UDungeonsGameInstance>()->OnAchievementUnlocked(achievementName, achievementURL, achievementTitle, achievementDesc);
			}
		}
		else {
			if (userAchievements.Num() > 0 && FMath::IsNearlyEqual(userAchievements[achievementEnum], 100.0f) && !stats->cachedAchievements.test(static_cast<uint64>(achievementEnum))) {
				stats->cachedAchievements.set(static_cast<uint64>(achievementEnum), 1);
				Save();
			}
			cachedAchievements.Emplace(achievementEnum, userAchievements[achievementEnum]);
		}
	}
}

#if DUNGEONS_OSS_ENABLED
// write feature stats to backend
void UStatTrackerComponent::WriteStats() {
	auto onlineStats = online::getCrossplayOss()->GetStatsIF();
	if (onlineStats.IsValid()) {
		TArray<FOnlineStatsUserUpdatedStats> UpdatedUserStats;
		FOnlineStatsUserUpdatedStats& UpdatedStats = UpdatedUserStats.Emplace_GetRef(netID.ToSharedRef());

		TMap<FString, FVariantData> featureStats = {
			{KILLS_STAT, stats->mobKills},
			{CHESTS_STAT, stats->chestsOpened},
			{EMERALD_STAT, stats->emeralds},
			{REVIVES_STAT, stats->revives},
			{LEVEL_STAT, stats->level},
			{DEATHS_STAT, stats->deaths},
			{DISTANCE_STAT, stats->distanceTravelled},
			{HEAL_STAT, uint64(stats->healed)},
			{ARTEFACTS_STAT, stats->usedArtefacts},
		};

		for (auto stat : featureStats) {
			// if local value is larger than remote value, write
			UpdatedStats.Stats.Emplace(stat.Key, FOnlineStatUpdate(stat.Value, FOnlineStatUpdate::EOnlineStatModificationType::Largest));
		}

		onlineStats->UpdateStats(netID->AsShared(), UpdatedUserStats, FOnlineStatsUpdateStatsComplete::CreateLambda([this](const FOnlineError& ResultState) {
			UE_LOG_ONLINE_STATS(Display, TEXT("\t[STATS] Update stat result state was %s"), *ResultState.ToLogString());
		}));
	}
}
#endif

void UStatTrackerComponent::UnlockTitle(ETitle title, float percentage) {
	if (!stats->titles.test(static_cast<uint8>(title)) && percentage == 100.f) {
		stats->titles.set(static_cast<uint8>(title));
		OnTitleUnlocked.Broadcast(title);
		Save();
	}
}
