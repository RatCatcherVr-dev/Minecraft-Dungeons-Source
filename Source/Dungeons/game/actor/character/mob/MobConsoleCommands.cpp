#include "Dungeons.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/Game.h"
#include "game/GameTypes.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/BackpackComponent.h"
#include "game/component/EnchantmentComponent.h"
#include "game/component/drop/LootTableItemDropComponent.h"
#include "game/mob/MobTypeDefs.h"

#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "game/util/ComponentUtils.h"
#include "game/util/ActorQuery.h"

#include "world/entity/MobTags.h"
#include "util/EnumUtil.h"
#include "util/StringUtil.h"

namespace {

void killHostileMobs(const APlayerCharacter& hostileTowardsPlayer, const Pred<AMobCharacter&>& pred) {
	for (auto* mob : actorquery::getActors<AMobCharacter>(hostileTowardsPlayer.GetWorld())) {
		if (mob && hostileTowardsPlayer.IsHostileTowards(mob) && pred(*mob)) {
			mob->Kill();
		}
	}
}

void DoKillAllMobs(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Mob.KillAll", world, commands)) {
		return;
	}
	killHostileMobs(*GetPlayerCharacter(world, commands), RETLAMBDA(true));
}

void DoKillAllMobsOfType(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Mob.KillAllOf", world, commands)) {
		return;
	}
	const auto entityType = ArgAsEntityType(commands, 0);
	if (!entityType) {
		out.Log(TEXT("Unknown mob type: " + ArgAsFString(commands, 0).Get("<no-argument>")));
		return;
	}
	killHostileMobs(*GetPlayerCharacter(world, commands), RETLAMBDA(it.EntityType == entityType.GetValue()));
}

void DoKillAllMobsExceptType(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Mob.KillAllExcept", world, commands)) {
		return;
	}
	const auto entityType = ArgAsEntityType(commands, 0);
	if (!entityType) {
		out.Log(TEXT("Unknown mob type: " + ArgAsFString(commands, 0).Get("<no-argument>")));
		return;
	}
	killHostileMobs(*GetPlayerCharacter(world, commands), RETLAMBDA(it.EntityType != entityType.GetValue()));
}

void DoCheckMobExpression(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() == 0) {
		out.Log(TEXT("Missing mob expression"));
		return;
	}
	auto expr = TCHAR_TO_UTF8(*FString::Join(commands, TEXT(" ")));

	TArray<FString> lines;
	for (auto&& mobType : game::mobspawn::evaluateMobExpr(expr)) {
		lines.Add(EntityTypeToString(mobType).c_str());
	}
	if (lines.Num() == 0) {
		out.Log(TEXT("No matching mobs"));
	} else {
		lines.Insert(TEXT("Matching mobs:"), 0);
		out.Log(FString::Join(lines, TEXT("\n    ")));
	}
}

void DoListMobTags(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (commands.Num() != 1) {
		out.Log(TEXT("Missing mob name"));
		return;
	}
	TArray<FString> tagStrings;
	for (auto&& tag : MobTags::singleton().tags(TCHAR_TO_UTF8(*commands[0])).tags()) {
		tagStrings.Add(FString(tag.c_str()));
	}
	out.Log(FString::Join(tagStrings, TEXT(" ")));
}

void DoLocateMobs(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (!ArgAsFString(commands, 0)) {
		out.Log(TEXT("Missing mob type pattern"));
		return;
	}
	const auto pattern = ArgAsFString(commands, 0).GetValue();

	const auto matchingTypes = algo::map_if_tarray(game::TypeMap::singleton().mobTypes(),
		RETLAMBDA(stringutil::toFString(EntityTypeToString(it))),
		RETLAMBDA(it.MatchesWildcard(pattern))
	);
	if (matchingTypes.Num() == 0) {
		out.Log(TEXT("Pattern doesn't match any mob types."));
		return;
	}
	out.Logf(TEXT("Matching mob types:\n  %s\n "), *FString::Join(matchingTypes, TEXT("\n  ")));

	const auto* game = actorquery::getGame(world);
	for (auto* mob : InstanceTracker<AMobCharacter>::GetList(world)) {
		const auto type = stringutil::toFString(EntityTypeToString(mob->EntityType));

		const auto backpackTypeMaybe = [&]() -> TOptional<FString> {
			const auto backpackComponent = mob->FindComponentByClass<UMochilaComponent>();

			if (backpackComponent && !backpackComponent->IsEmpty()) {
				const auto backpackType = stringutil::toFString(EntityTypeToString(backpackComponent->Get()));
				return backpackType.MatchesWildcard(pattern) ? backpackType : TOptional<FString>();
			}

			return {};
		}();

		if (!type.MatchesWildcard(pattern) && !backpackTypeMaybe) {
			continue;
		}

		FString line = backpackTypeMaybe
			? type + FString(TEXT(" holding ")) + backpackTypeMaybe.GetValue()
			: type;

		if (game) {
			if (auto* tile = game->tiles().getTile(*mob)) {
				const auto location = mob->GetActorLocation();

				line += FString::Format(TEXT(" @ {0} in dungeon: {1}#{2} (Z={3}, ground={4})"), {
					stringutil::toFString(tile->tile().id()),
					stringutil::toFString(tile->dungeon().def().id.id),
					FString::FromInt(tile->dungeon().instanceId()),
					conversion::ueToBlock(location).y,
					tile->tileArea().heights.getOrDefault(conversion::ueToBlock(location), -1)
				});
			}
		}
		out.Log(line);
	}
}

void DoSpawnMob(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (HandleOnServer("Dungeons.Mob.Spawn", world, commands))
		return; 
	
	if (commands.Num() <= 0) {
		out.Log(TEXT("First argument must be mob/tag name"));
		return;
	}

	const auto count = [&] {
		if (commands.Num() > 1) {
			if (commands[1].IsNumeric()) {
				return FCString::Atoi(*commands[1]);
			}

			out.Log(TEXT("First arg must be a mob name/tag and second argument must be a number (if present)"));
		}

		return 1;
	}();

	bool isUnderling = false;
	TArray<FEnchantmentData> enchantments;

	for (int i = 2; i < commands.Num(); ++i) {
		if (auto enchantment = EnumValueFromString(EEnchantmentTypeID, commands[i])) {
			enchantments.Emplace(enchantment.Get(EEnchantmentTypeID::Unset), 3);
		}
		if (commands[i] == "underling")
			isUnderling = true;
	}

	if (auto playerController = world->GetFirstPlayerController()) {
		const std::string typeName { TCHAR_TO_UTF8(*commands[0]) };
		io::MobGroup mobGroup { { { typeName } } };
		game::mobspawn::prepareMobGroup(mobGroup);

		const auto entityTypes = game::mobspawn::calculateMobsWithoutDifficultyLimitation({ mobGroup }, count);
		if (entityTypes.Num() <= 0) {
			out.Log(TEXT("No mobs match the mob/tag name"));
			return;
		}

		const auto entityType = entityTypes.Last();

		const auto* player = GetPlayerCharacter(world, commands);
		const auto forward = player->GetActorForwardVector() * FMath::Max(300.f, count * 45.f);
		const float yaw = player->GetActorRotation().Yaw;

		const float angleIncrement = 360.f / count;

		float angle = 0.f;
		const auto transformer = [&]() -> FTransform {
			FTransform transform {
				FRotator{ 0.f, angle + 180.f, 0.f },
				player->GetActorLocation() + forward.RotateAngleAxis(angle, FVector::UpVector),
				FVector::OneVector
			};

			angle += angleIncrement;
			return transform;
		};

		AGameBP* game = actorquery::getFirstActor<AGameBP>(world);

		// @todo: add a UE_LOG message? There's no guarantee that the <out> log object is valid in the callback
		game->RequestMobGroupSpawn(game::mobspawn::SpawnGroup(entityType, count, std::move(enchantments), game->GetGame()->settings().difficultyStats.GetEnchantedMobGroupDropChance())
			, transformer
			, isUnderling? game::mobspawn::configs::DefaultNoVariants(false).SpawnAsUnderling() : 
				game::mobspawn::configs::DefaultNoVariants(false)
		);
	}
}

void DoPrintMobDifficultySettings( const TArray<FString>& commands, UWorld* world, FOutputDevice& out ) {

	if( const auto* game = actorquery::getGame( world ) ) {

		int msgId = 129348; // D11.DB - Arbitrary starting number for Ids.
		const float msgDuration = 10.0f;

		UEndlessStruggleLibrary* lib;
		if( GWorld ) {
			if( auto instance = GWorld->GetGameInstance<UDungeonsGameInstance>() ) {
				lib = instance->GetEndlessStruggleLibrary();
			}
		}

		auto diff = game->settings().difficulty.chosen();
		auto threat = game->settings().difficulty.threatLevel();
		auto struggle = game->settings().difficulty.endlessStruggle();
		if( commands.Num() == 3 ) {
			auto optDiff = EnumValueFromString(EGameDifficulty, "Difficulty_" + commands[0]);
			auto optThreat = EnumValueFromString(EThreatLevel, "Threat_" + commands[1]);
			FEndlessStruggle optStruggle;
			optStruggle.Value = commands[2].IsNumeric() ? FCString::Atoi( *commands[2] ) : 0;

			if( optDiff.IsSet() && optThreat.IsSet() ) {
				diff = optDiff.GetValue();
				threat = optThreat.GetValue();
				struggle.Value = optStruggle.Value;
			}
		}
		auto difficulty = game::FDifficulty( diff, threat, EExtraChallenge::Invalid, struggle );
		auto difficultyStats = game::DifficultyStats{ difficulty };

		struggle.Value = 0;
		auto difficultyStruggless = game::FDifficulty( diff, threat, EExtraChallenge::Invalid, struggle );
		auto difficultyStatsStruggleless = game::DifficultyStats{ difficultyStruggless };

		auto hasBeenIntroduced = [=]( FName rowName ) {
			if( lib ) {
				if( auto row = lib->GetRow(rowName) ) {
					return difficulty.endlessStruggle().Value >= row->IntroducedAt;
				}
			}
			return false;
		};
		auto getIntroColor = [=]( FName rowName ) -> FColor {
			return hasBeenIntroduced(rowName) ? FColor::Green : FColor::Red;
		};

		auto fromFloat2 = [&]( const FString& pre, float value, float value2, const FColor& col = FColor::Magenta ) {
			FString msg = "  " + pre + ": " + FString::SanitizeFloat( value );
			GEngine->AddOnScreenDebugMessage(-1, msgDuration, col, msg, false);
			out.Log(msg);
		};
		auto fromFloat = [&]( const FString& pre, float value, const FColor& col = FColor::Magenta ) {
			fromFloat2(pre, value, value, col);
		};
		auto fromInt2 = [&]( const FString& pre, int value, int value2, const FColor& col = FColor::Magenta ) {
			FString msg = "  " + pre + ": " + FString::FromInt(value);
			GEngine->AddOnScreenDebugMessage(-1, msgDuration, col, msg, false);
			out.Log(msg);
		};
		auto fromInt = [&]( const FString& pre, int value, const FColor& col = FColor::Magenta ) {
			fromInt2(pre, value, value, col);
		};
		auto fromStr = [&]( const FString& msg, const FColor& col = FColor::Yellow ) {
			GEngine->AddOnScreenDebugMessage(-1, msgDuration, col, msg, false);
			out.Log(msg);
		};

		fromStr( "Difficulty" );
		fromInt( "Difficulty", static_cast<int32>(difficulty.chosen()) );
		fromInt( "Threat Index", static_cast<int32>(difficulty.globalThreatIndex()) );
		fromFloat( "Threat Fraction", difficulty.globalThreatFraction() );
		fromInt( "Extra Challenge Index: ", static_cast<int32>(difficulty.extraChallenge()) );
		fromFloat( "Extra Challenge Fraction: ", difficulty.extraChallengeFraction() );
		fromInt( "Endless Struggle Index: ", difficulty.endlessStruggle().Value );
		fromFloat( "Endless Struggle Fraction", difficulty.endlessStruggleFraction() );
		fromStr("");

		fromStr("Loot and XP");
		fromFloat2( "XP Multiplier", difficultyStats.GetXpMultiplier(), difficultyStatsStruggleless.GetXpMultiplier(), getIntroColor("XPMultiplier") );
		fromFloat2( "Loot Power Multiplier", difficultyStats.GetCombinedItemPowerRange().max(), difficultyStatsStruggleless.GetCombinedItemPowerRange().max(), getIntroColor("LootPowerMultiplier") );
		fromInt2( "Loot Quality Boost", difficultyStats.GetLootQualityBoost(), difficultyStatsStruggleless.GetLootQualityBoost(), getIntroColor("LootQualityBoost") );
		fromFloat2( "Shop Price Multiplier", difficultyStats.GetShopPriceMultiplier(), difficultyStatsStruggleless.GetShopPriceMultiplier(), getIntroColor("ShopPriceMultiplier") );
		fromStr("");

		fromStr("Mobs");
		fromFloat2( "Count", difficultyStats.GetMobCountMultiplier(), difficultyStatsStruggleless.GetMobCountMultiplier(), getIntroColor("MobCountMultiplier") );
		fromFloat2( "Max Health", difficultyStats.GetMobMaxHealthMultiplier(), difficultyStatsStruggleless.GetMobMaxHealthMultiplier(), getIntroColor("MobMaxHealthMultiplier") );
		fromFloat2( "Perform Healing", difficultyStats.GetMobPerformHealingMultiplier(), difficultyStatsStruggleless.GetMobPerformHealingMultiplier(), getIntroColor("MobPerformHealingMultiplier") );
		fromFloat2( "Max Endurance", difficultyStats.GetMobMaxEnduranceMultiplier(), difficultyStatsStruggleless.GetMobMaxEnduranceMultiplier(), getIntroColor("MobMaxEnduranceMultiplier") );
		fromFloat2( "Recover Endurance", difficultyStats.GetMobRecoverEnduranceMultiplier(), difficultyStatsStruggleless.GetMobRecoverEnduranceMultiplier(), getIntroColor("MobRecoverEnduranceMultiplier") );
		fromFloat2( "Damage", difficultyStats.GetMobDamageMultiplier(), difficultyStatsStruggleless.GetMobDamageMultiplier(), getIntroColor("MobDamageMultiplier") );
		fromFloat2( "Stun Duration", difficultyStats.GetMobStunDurationMultiplier(), difficultyStatsStruggleless.GetMobStunDurationMultiplier(), getIntroColor("MobStunDurationMultiplier") );
		fromFloat2( "Speed", difficultyStats.GetMobSpeedMultiplier(), difficultyStatsStruggleless.GetMobSpeedMultiplier(), getIntroColor("MobSpeedMultiplier") );
		fromFloat2( "Resurrection Chance", difficultyStats.GetMobResurrectionChance(), difficultyStatsStruggleless.GetMobResurrectionChance(), getIntroColor("MobResurrectionChance") );
		fromFloat2( "Pushback Multiplier", difficultyStats.GetMobPushbackMultiplier(), difficultyStatsStruggleless.GetMobPushbackMultiplier(), getIntroColor("MobPushbackMultiplier"));
		fromStr("");

		fromStr("Enchanted Mobs");
		fromFloat2( "Probability", difficultyStats.GetEnchantedMobProbability(), difficultyStatsStruggleless.GetEnchantedMobProbability(), getIntroColor("EnchantedMobChanceMultiplier") );
		fromFloat2( "Power Fraction", difficultyStats.GetEnchantedMobPowerFraction(), difficultyStatsStruggleless.GetEnchantedMobPowerFraction(), getIntroColor("EnchantedMobPowerMultiplier") ); 
		fromStr("");

		fromStr("Players");
		fromFloat2( "Perform Healing", difficultyStats.GetPlayerReceiveHealingMultiplier(), difficultyStatsStruggleless.GetPlayerReceiveHealingMultiplier(), getIntroColor("Perform Healing")); fromStr("\n");
	}
}

void DoPrintAncientMobDrops(const TArray<FString>& commands, UWorld* world, FOutputDevice& out) {
	if (const auto* game = actorquery::getGame(world)) {
		out.Log("#### PrintAncientMobLoot ####");

		const auto ancients = algo::copy_if(mob::type::getAllEnabled(), RETLAMBDA(hasMobTag(it, MobTags::HashTag_Ancient)));
		for (EntityType t : ancients) {
			UClass* mobclass = game::TypeMap::singleton().mobClass(t);
			if (UBlueprintGeneratedClass* mobBp = Cast<UBlueprintGeneratedClass>(mobclass))
			{
				out.Logf(TEXT(" -- %s Loot Table -- "), *mobBp->GetName());
				if (auto* comp = componentutils::GetDefaultComponentByClass<ULootTableItemDropComponent>(mobBp)) {
					out.Logf(TEXT("%s"), *FString::JoinBy(comp->LootTable, TEXT(","), [](const FSerializableItemId& id) { return id.ToString(); }));
					out.Logf(TEXT("%s %s\n"), *GetEnumValueToString<EDropSpawnType>(comp->SpawnType), comp->IncludeUniques ? TEXT(" +UNIQUE Child Types") : TEXT(""));
					out.Log("\\");
				}
				else
				{
					out.Log("WARN: No LootTableComp for ancient mob!");
					out.Log(TEXT("\\\\"));
				}
			}			
			
		}
	}
}

const FAutoConsoleCommand KillAllMobsCommand(TEXT("Dungeons.Mob.KillAll")
	, TEXT("Kills all Mobs around in radius")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoKillAllMobs)
	, ECVF_Cheat);

const FAutoConsoleCommand KillAllMobsOfCommand(TEXT("Dungeons.Mob.KillAllOf")
	, TEXT("Kills all mobs of the given type")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoKillAllMobsOfType)
	, ECVF_Cheat);

const FAutoConsoleCommand KillAllMobsExceptCommand(TEXT("Dungeons.Mob.KillAllExcept")
	, TEXT("Kills all mobs except the given type")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoKillAllMobsExceptType)
	, ECVF_Cheat);

const FAutoConsoleCommand CheckMobExpression(TEXT("Dungeons.Mob.Check")
	, TEXT("Check what mobs fit the given string expression (e.g. melee)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoCheckMobExpression)
	, ECVF_Cheat);

const FAutoConsoleCommand ListMobTags(TEXT("Dungeons.Mob.Tags")
	, TEXT("Check what tags a given mob type has (e.g. skeleton)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoListMobTags)
	, ECVF_Cheat);

const FAutoConsoleCommand LocateMobs(TEXT("Dungeons.Mob.Locate")
	, TEXT("Locate mobs given matching a given type pattern (e.g. *babykey matches both goldbabykey, silverbabykey)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoLocateMobs)
	, ECVF_Default);

const FAutoConsoleCommand SpawnMob(TEXT("Dungeons.Mob.Spawn")
	, TEXT("Spawns a mob in front of the player")
	TEXT("\nUsage: Dungeons.Mob.Spawn \"typename\"")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoSpawnMob)
	, ECVF_Cheat);

const FAutoConsoleCommand PrintMobDifficultySettings(TEXT("Dungeons.Mob.PrintDifficultySettings")
	, TEXT("Prints information about the current difficulty settings")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPrintMobDifficultySettings)
	, ECVF_Cheat);

const FAutoConsoleCommand PrintAncientMobDrops(TEXT("Dungeons.Mob.PrintAncientMobDrops")
	, TEXT("Prints list of ancient mobs and their associated loot tables")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoPrintAncientMobDrops)
	, ECVF_Cheat);
} // anonymous namespace
