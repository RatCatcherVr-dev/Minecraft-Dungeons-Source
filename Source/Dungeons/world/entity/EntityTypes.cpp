/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/EntityTypes.h"
#include "world/entity/EntityDefinitionIdentifier.h"

namespace entitytypesinternal {


	static const std::unordered_map<EntityType, EntityMapping> map({
	// Animals
	{ EntityType::Chicken, EntityMapping("chicken") },
	{ EntityType::Cow, EntityMapping("cow") },
	{ EntityType::Pig, EntityMapping("pig") },
	{ EntityType::Sheep, EntityMapping("sheep") },
	{ EntityType::Wolf, EntityMapping("wolf") },
	{ EntityType::SheepFireRed, EntityMapping("sheepfirered") },
	{ EntityType::SheepPoisonGreen, EntityMapping("sheeppoisongreen") },
	{ EntityType::SheepSpeedBlue, EntityMapping("sheepspeedblue") },
	{ EntityType::Villager, EntityMapping("villager") },
	{ EntityType::Mooshroom, EntityMapping("mooshroom", "mushroomcow") },
	{ EntityType::MooshroomAncient, EntityMapping("mooshroomancient", "mushroom-ancient") },
	{ EntityType::Squid, EntityMapping("squid") },
	{ EntityType::Rabbit, EntityMapping("rabbit") },
	{ EntityType::Hoglin, EntityMapping("hoglin") },
	{ EntityType::HoglinAncient, EntityMapping("hoglinancient", "unstoppabletusk") },
	{ EntityType::Bat, EntityMapping("bat") },
	{ EntityType::IronGolem, EntityMapping("iron_golem", "irongolem") },
	{ EntityType::SnowGolem, EntityMapping("snow_golem", "snowgolem") },
	{ EntityType::Horse, EntityMapping("horse") },
	{ EntityType::BabyChicken, EntityMapping("babychicken") },
	{ EntityType::BabyPig, EntityMapping("babypig") },
	{ EntityType::BabyGhast, EntityMapping("babyghast") },

	{ EntityType::ZombifiedBabyPig, EntityMapping("zombifiedbabypig") },

	{ EntityType::Donkey, EntityMapping("donkey") },
	{ EntityType::Mule, EntityMapping("mule") },
	{ EntityType::SkeletonHorse, EntityMapping("skeleton_horse", "skeletonhorse") },
	{ EntityType::ZombieHorse, EntityMapping("zombie_horse", "zombiehorse") },
	{ EntityType::PiggyBank, EntityMapping("piggybank") },
	{ EntityType::GoldBabyKey, EntityMapping("goldbabykey") },
	{ EntityType::SilverBabyKey, EntityMapping("silverbabykey") },
	{ EntityType::NamelessKing, EntityMapping("namelessking") },
	{ EntityType::PiglinMelee, EntityMapping("piglin", "piglinmelee") },
	{ EntityType::PiglinMeleeVariant0, EntityMapping("piglinvariant0", "piglinmeleevariant0") },
	{ EntityType::PiglinMeleeVariant1, EntityMapping("piglinvariant1", "piglinmeleevariant1") },
	{ EntityType::PiglinMeleeVariant2, EntityMapping("piglinvariant2", "piglinmeleevariant2") },
	{ EntityType::PiglinRanged, EntityMapping("piglinranged") },
	{ EntityType::PiglinRangedVariant0, EntityMapping("piglinrangedvariant0") },
	{ EntityType::PiglinRangedVariant1, EntityMapping("piglinrangedvariant1") },
	{ EntityType::PiglinRangedVariant2, EntityMapping("piglinrangedvariant2") },
	{ EntityType::ZombifiedPiglinMelee, EntityMapping("zombifiedpiglin", "zombifiedpiglinmelee") },
	{ EntityType::ZombifiedPiglinMeleeVariant0, EntityMapping("zombifiedpiglinvariant0", "zombifiedpiglinmeleevariant0") },
	{ EntityType::ZombifiedPiglinMeleeVariant1, EntityMapping("zombifiedpiglinvariant1", "zombifiedpiglinmeleevariant1") },
	{ EntityType::ZombifiedPiglinMeleeVariant2, EntityMapping("zombifiedpiglinvariant2", "zombifiedpiglinmeleevariant2") },
	{ EntityType::ZombifiedPiglinRanged, EntityMapping("zombifiedpiglinranged") },
	{ EntityType::ZombifiedPiglinRangedVariant0, EntityMapping("zombifiedpiglinrangedvariant0") },
	{ EntityType::ZombifiedPiglinRangedVariant1, EntityMapping("zombifiedpiglinrangedvariant1") },
	{ EntityType::ZombifiedPiglinRangedVariant2, EntityMapping("zombifiedpiglinrangedvariant2") },
	{ EntityType::PiglinFungusThrower, EntityMapping("piglinfungusthrower") },
	{ EntityType::ZombifiedPiglinFungusThrower, EntityMapping("zombifiedpiglinfungusthrower") },
	{ EntityType::Pillager, EntityMapping("pillager") },
	{ EntityType::PillagerVariant0, EntityMapping("pillagervariant0") },
	{ EntityType::PillagerVariant1, EntityMapping("pillagervariant1") },
	{ EntityType::PillagerVariant2, EntityMapping("pillagervariant2") },
	{ EntityType::PillagerAncient, EntityMapping("pillagerancient", "solemngiant") },
	{ EntityType::FalseKing, EntityMapping("falseking") },
	{ EntityType::SkeletonVanguard, EntityMapping("skeletonvanguard") },
	{ EntityType::SkeletonVanguardAncient, EntityMapping("skeletonvanguardancient", "cursedpresence") },

	//	Hostile Mobs
	{ EntityType::Zombie, EntityMapping("zombie") },
	{ EntityType::ZombieVariant0, EntityMapping("zombievariant0") },
	{ EntityType::ZombieVariant1, EntityMapping("zombievariant1") },
	{ EntityType::ZombieVariant2, EntityMapping("zombievariant2") },
	{ EntityType::ZombieAncient, EntityMapping("zombieancient", "grimguardian") },
	{ EntityType::Creeper, EntityMapping("creeper") },
	{ EntityType::ChargedCreeper, EntityMapping("chargedcreeper") },
	{ EntityType::WitherSkeletonRanged, EntityMapping("witherskeletonranged") },
	{ EntityType::Skeleton, EntityMapping("skeleton") },
	{ EntityType::SkeletonVariant0, EntityMapping("skeletonvariant0") },
	{ EntityType::SkeletonVariant1, EntityMapping("skeletonvariant1") },
	{ EntityType::SkeletonVariant2, EntityMapping("skeletonvariant2") },
	{ EntityType::SkeletonAncient, EntityMapping("skeletonancient", "barrage") },
	{ EntityType::Spider, EntityMapping("spider") },
	{ EntityType::SpiderAncient, EntityMapping("spiderancient", "abominableweaver") },
	{ EntityType::SlimeLarge, EntityMapping("slimelarge") },
	{ EntityType::SlimeMedium, EntityMapping("slimemedium") },
	{ EntityType::SlimeSmall, EntityMapping("slimesmall") },
	{ EntityType::SlimeSmallAncient, EntityMapping("slimesmallancient", "oozingmenace") },
	{ EntityType::Silverfish, EntityMapping("silverfish") },
	{ EntityType::SilverfishSmart, EntityMapping("silverfishsmart") },
	{ EntityType::SilverfishAncient, EntityMapping("silverfishancient") },
	{ EntityType::CaveSpider, EntityMapping("cave_spider", "cavespider") },
	{ EntityType::Ghast, EntityMapping("ghast") },
	{ EntityType::MagmaCubeLarge, EntityMapping("magmacube-large") },
	{ EntityType::MagmaCubeMedium, EntityMapping("magmacube-medium") },
	{ EntityType::MagmaCubeSmall, EntityMapping("magmacube-small") },
	{ EntityType::Blaze, EntityMapping("blaze") },
	{ EntityType::HoveringInferno, EntityMapping("hoveringinferno", "hovering inferno") },
	{ EntityType::BlazeSpawner, EntityMapping("blazespawner", "blaze-spawner") },
	{ EntityType::BlazeSpawnerAncient, EntityMapping("blazespawnerancient", "theunending") },
	{ EntityType::ZombieVillager, EntityMapping("zombie_villager", "zombievillager") },
	{ EntityType::Witch, EntityMapping("witch") },
	{ EntityType::WitchAncient, EntityMapping("witchancient", "pestilentconjurer") },
	{ EntityType::Enderman, EntityMapping("enderman") },

	{ EntityType::Stray, EntityMapping("stray", "skeleton.stray") },
	{ EntityType::Husk, EntityMapping("husk") },
	{ EntityType::WitherSkeleton, EntityMapping("witherskeleton", "skeletonwither") },
	{ EntityType::Guardian, EntityMapping("guardian") },
	{ EntityType::GuardianAncient, EntityMapping("guardianancient") },
	{ EntityType::ElderGuardian, EntityMapping("elder_guardian", "guardian.elder") },

	{ EntityType::WitherBoss, EntityMapping("wither", "wither.boss") },
	{ EntityType::Dragon, EntityMapping("ender_dragon", "dragon") },
	{ EntityType::Shulker, EntityMapping("shulker") },
	{ EntityType::ShulkerRespawning, EntityMapping("shulkerrespawning") },
	{ EntityType::Endermite, EntityMapping("endermite") },
	{ EntityType::EndermiteSmart, EntityMapping("endermitesmart") },

	{ EntityType::BabyZombie, EntityMapping("babyzombie", "baby-zombie") },
	{ EntityType::BabyDrowned, EntityMapping("babydrowned", "baby-drowned") },
	{ EntityType::BabyZombieAncient, EntityMapping("babyzombieancient", "thetinyscourge") },
	{ EntityType::Necromancer, EntityMapping("necromancer") },
	{ EntityType::NecromancerAncient , EntityMapping("necromancerancient", "hauntedcaller") },
	{ EntityType::RedstoneGolem, EntityMapping("redstonegolem", "redstone-golem") },
	{ EntityType::RedstoneGolemAncient, EntityMapping("redstonegolemancient", "unbreakableone") },
	{ EntityType::Enchanter, EntityMapping("enchanter") },
	{ EntityType::EnchanterAncient, EntityMapping("enchanterancient" , "firstenchanter") },
	{ EntityType::Vindicator, EntityMapping("vindicator") },
	{ EntityType::VindicatorVariant0, EntityMapping("vindicatorvariant0") },
	{ EntityType::VindicatorVariant1, EntityMapping("vindicatorvariant1") },
	{ EntityType::VindicatorVariant2, EntityMapping("vindicatorvariant2") },
	{ EntityType::Vex, EntityMapping("vex") },
	{ EntityType::VexAncient, EntityMapping("vexancient", "theseekingflame") },
	{ EntityType::Wraith, EntityMapping("wraith") },
	{ EntityType::WraithAncient, EntityMapping("wraithancient", "ancientterror") },
	{ EntityType::Evoker, EntityMapping("evoker") },
	{ EntityType::EvokerFang, EntityMapping("fang", "evoker-fang") },
	{ EntityType::Geomancer, EntityMapping("geomancer") },
	{ EntityType::GeomancerWall, EntityMapping("geomancerwall") },
	{ EntityType::GeomancerBomb, EntityMapping("geomancerbomb") },
	{ EntityType::GeomancerAncient, EntityMapping("geomancerancient", "frostwarden") },
	{ EntityType::ChickenJockey, EntityMapping("chickenjockey", "chicken-jockey") },
	{ EntityType::ChickenJockeyTower, EntityMapping("chickenjockeytower", "chicken-jockey-tower") },
	{ EntityType::ChickenJockeyTowerAncient, EntityMapping("chickenjockeytowerancient", "thetower") },
	{ EntityType::RedstoneMonstrosity, EntityMapping("redstonemonstrosity", "redstone-monstrosity") },
	{ EntityType::MooshroomMonstrosity, EntityMapping("mooshroommonstrosity", "mooshroom-monstrosity") },
	{ EntityType::RedstoneCube, EntityMapping("redstonecube", "redstone-cube") },
	{ EntityType::MobSpawner, EntityMapping("mobspawner", "mob-spawner") },
	{ EntityType::SkeletonHorseman, EntityMapping("skeletonhorseman", "skeleton-horseman") },
	{ EntityType::OrdinaryHorse, EntityMapping("ordinaryhorse", "ordinary-horse") },
	{ EntityType::CauldronBoss, EntityMapping("cauldronboss", "cauldron-boss") },
	{ EntityType::SlimeCauldron, EntityMapping("slimecauldron", "slime-cauldron") },
	{ EntityType::Bat, EntityMapping("bat") },
	{ EntityType::Bee, EntityMapping("bee") },
	{ EntityType::ArchIllager, EntityMapping("archillager") },
	{ EntityType::ArchVessel, EntityMapping("archvessel") },
	{ EntityType::ArchVisage, EntityMapping("archvisage") },
	{ EntityType::JackOLantern, EntityMapping("jackolantern") },
	{ EntityType::RoyalGuard, EntityMapping("royalguard") },
	{ EntityType::RoyalGuardAncient, EntityMapping("royalguardancient", "vigilantscoundrel") },
	{ EntityType::VindicatorChef, EntityMapping("vindicator-chef") },
	{ EntityType::Llama, EntityMapping("llama") },
	{ EntityType::SoulWizard, EntityMapping("soulwizard") },
	{ EntityType::LlamaMob, EntityMapping("llamamob", "llama_mob") },

	// D11.DB
    { EntityType::JungleZombie, EntityMapping("junglezombie") },
    { EntityType::MossySkeleton, EntityMapping("mossyskeleton") },
    { EntityType::QuickGrowingVine, EntityMapping("quickgrowingvine") },
    { EntityType::QuickGrowingVineSimple, EntityMapping("quickgrowingvinesimple") },
	{ EntityType::QuickGrowingKelp, EntityMapping("quickgrowingkelp") },
    { EntityType::PoisonQuillVine, EntityMapping("poisonquillvine") },
    { EntityType::PoisonQuillVineSimple, EntityMapping("poisonquillvinesimple") },
	{ EntityType::PoisonAnemone, EntityMapping("poisonanemone") },
    { EntityType::EntangleVine, EntityMapping("entanglevine") },
    { EntityType::AbominationVine, EntityMapping("abominationvine") },
    { EntityType::Ocelot, EntityMapping("ocelot") },
    { EntityType::PlayfulPanda, EntityMapping("playfulpanda") },
    { EntityType::LazyPanda, EntityMapping("lazypanda") },
    { EntityType::BabyPanda, EntityMapping("babypanda") },
    { EntityType::BrownPanda, EntityMapping("brownpanda") },
    { EntityType::Leaper, EntityMapping("leaper") },
	{ EntityType::LeaperAncient, EntityMapping("leaperancient", "thunderinggrowth") },
    { EntityType::Whisperer, EntityMapping("whisperer") },
	{ EntityType::WaveWhisperer, EntityMapping("wavewhisperer") },
    { EntityType::JungleAbomination, EntityMapping("jungleabomination") },
    { EntityType::Parrot, EntityMapping("parrot") },
	{ EntityType::Panda, EntityMapping("panda") },

	{ EntityType::FrozenZombie, EntityMapping("frozenzombie") },
	{ EntityType::IcyCreeper, EntityMapping("icycreeper") },
	{ EntityType::Stray, EntityMapping("stray") },
	{ EntityType::PolarBear, EntityMapping("polarbear") },
	{ EntityType::ArcticFox, EntityMapping("arcticfox") },
	{ EntityType::Illusioner, EntityMapping("illusioner") },
	{ EntityType::IllusionerClone, EntityMapping("illusionerclone") },
	{ EntityType::Chillager, EntityMapping("chillager") },
	{ EntityType::WickedWraith, EntityMapping("wickedwraith") },

	{ EntityType::BabyGoat, EntityMapping("babygoat", "baby_goat") },
	{ EntityType::Goat, EntityMapping("goat") },
	{ EntityType::GoatAncient, EntityMapping("goatancient", "windbeard") },
	{ EntityType::Ravager, EntityMapping("ravager") },
	{ EntityType::Mountaineer, EntityMapping("mountaineer") },
	{ EntityType::MountaineerVariant0, EntityMapping("mountaineervariant0") },
	{ EntityType::MountaineerVariant1, EntityMapping("mountaineervariant1") },
	{ EntityType::MountaineerVariant2, EntityMapping("mountaineervariant2") },
	{ EntityType::WindCaller, EntityMapping("windcaller", "wind_caller") },
	{ EntityType::SquallGolem, EntityMapping("squallgolem", "squall_golem") },
	{ EntityType::TempestGolem, EntityMapping("tempestgolem", "tempest_golem") },
	{ EntityType::WoolyCow, EntityMapping("woolycow") },
	{ EntityType::RampartCaptain, EntityMapping("rampartcaptain", "rampart_captain") },

	{ EntityType::TropicalSlimeLarge, EntityMapping("tropicalslimelarge") },
	{ EntityType::TropicalSlimeMedium, EntityMapping("tropicalslimemedium") },
	{ EntityType::TropicalSlimeSmall, EntityMapping("tropicalslimesmall") },
	{ EntityType::Squid, EntityMapping("squid") },
	{ EntityType::GlowSquid, EntityMapping("glowsquid") },
	{ EntityType::Turtle, EntityMapping("turtle") },
	{ EntityType::BabyTurtle, EntityMapping("babyturtle") },
	{ EntityType::Pufferfish, EntityMapping("pufferfish") },
	{ EntityType::Drowned, EntityMapping("drowned") },
	{ EntityType::DrownedAncient, EntityMapping("drownedancient") },
	{ EntityType::DrownedVariant0, EntityMapping("drownedvariant0") },
	{ EntityType::DrownedVariant1, EntityMapping("drownedvariant1") },
	{ EntityType::DrownedVariant2, EntityMapping("drownedvariant2") },
	{ EntityType::TridentDrowned, EntityMapping("tridentdrowned") },
	{ EntityType::TridentDrownedVariant0, EntityMapping("tridentdrownedvariant0") },
	{ EntityType::TridentDrownedVariant1, EntityMapping("tridentdrownedvariant1") },
	{ EntityType::TridentDrownedVariant2, EntityMapping("tridentdrownedvariant2") },
	{ EntityType::Dolphin, EntityMapping("dolphin") },
	{ EntityType::SunkenSkeleton, EntityMapping("sunkenskeleton") },
	{ EntityType::SunkenSkeletonVariant0, EntityMapping("sunkenskeletonVariant0") },
	{ EntityType::SunkenSkeletonVariant1, EntityMapping("sunkenskeletonVariant1") },
	{ EntityType::SunkenSkeletonVariant2, EntityMapping("sunkenskeletonVariant2") },
	{ EntityType::DrownedNecromancer, EntityMapping("drownednecromancer") },
	{ EntityType::BabyTurtlePet, EntityMapping("babyturtlepet") },
	{ EntityType::AncientGuardian, EntityMapping("ancient_guardian", "ancientguardian") },
	{ EntityType::AncientGuardianMine, EntityMapping("ancient_guardian_mine", "ancientguardianmine") },	
	{ EntityType::Biomine, EntityMapping("biomine") },
	{ EntityType::Seravex, EntityMapping("seravex") },
	{ EntityType::PerfectFormHeart, EntityMapping("perfectform") },
	{ EntityType::EndermiteMinion, EntityMapping("endermiteminion") },
	// D11.DB - END

	// D11.PC

	{ EntityType::VindicatorRaidCaptain, EntityMapping("VindicatorRaidCaptain") },
	{ EntityType::PillagerRaidCaptain, EntityMapping("PillagerRaidCaptain") },

	//D11.AS - END
	//DLC6
	{ EntityType::Blastling, EntityMapping("blastling") },
	{ EntityType::Snareling, EntityMapping("snareling") },
	{ EntityType::SnarelingAncient, EntityMapping("snarelingancient") },
	{ EntityType::Endling, EntityMapping("endling") },
	{ EntityType::EndlingAncient, EntityMapping("endlingancient") },
	{ EntityType::Endersent, EntityMapping("endersent") },
	{ EntityType::EndersentThornblight, EntityMapping("endersent_thornblight", "thornblight") },
	{ EntityType::EndersentVoidstrike, EntityMapping("endersent_voidstrike", "voidstrike") },
	{ EntityType::EndersentWritherot, EntityMapping("endersent_writherot", "writherot") },
	{ EntityType::EndersentEverfire, EntityMapping("endersent_everfire", "everfire") },
	{ EntityType::EndersentSurgefiend, EntityMapping("endersent_surgefiend", "surgefiend") },
	{ EntityType::EndersentDeadeye, EntityMapping("endersent_deadeye", "deadeye") },
	{ EntityType::Friendermite, EntityMapping("friendermite") },
	//DLC6 - END

	{ EntityType::Player, EntityMapping("player") },

	//	Non-Mob
	{ EntityType::MinecartHopper, EntityMapping("hopper_minecart", "minecarthopper") },
	{ EntityType::MinecartTNT, EntityMapping("tnt_minecart", "minecarttnt") },
	{ EntityType::MinecartChest, EntityMapping("chest_minecart", "minecartchest") },

	{ EntityType::ItemEntity, EntityMapping("item") },
	{ EntityType::PrimedTnt, EntityMapping("tnt", "primedtnt") },
	{ EntityType::FallingBlock, EntityMapping("falling_block", "fallingblock") },
	{ EntityType::MovingBlock, EntityMapping("moving_block", "movingblock") },
	{ EntityType::ExperiencePotion, EntityMapping("xp_bottle", "potion.experience") },
	{ EntityType::Experience, EntityMapping("xp_orb", "xporb") },
	{ EntityType::EyeOfEnder, EntityMapping("eye_of_ender_signal", "eyeofender") },
	{ EntityType::EnderCrystal, EntityMapping("ender_crystal", "endercrystal") },
	{ EntityType::ShulkerBullet, EntityMapping("shulker_bullet", "shulkerbullet") },
	{ EntityType::FishingHook, EntityMapping("fishing_hook", "fishinghook") },

	{ EntityType::DragonFireball, EntityMapping("dragon_fireball", "fireball.dragon") },
	{ EntityType::Arrow, EntityMapping("arrow", "arrow.skeleton") },
	{ EntityType::Snowball, EntityMapping("snowball") },
	{ EntityType::ThrownEgg, EntityMapping("egg", "thrownegg") },
	{ EntityType::Painting, EntityMapping("painting") },
	{ EntityType::LargeFireball, EntityMapping("fireball", "fireball.large") },
	{ EntityType::ThrownPotion, EntityMapping("splash_potion", "thrownpotion") },
	{ EntityType::Enderpearl, EntityMapping("ender_pearl", "thrownenderpearl") },
	{ EntityType::LeashKnot, EntityMapping("leash_knot", "leashknot") },
	{ EntityType::WitherSkull, EntityMapping("wither_skull", "wither.skull") },
	{ EntityType::WitherSkullDangerous, EntityMapping("wither_skull_dangerous", "wither.skull.dangerous") },

	{ EntityType::BoatRideable, EntityMapping("boat")},
	{ EntityType::LightningBolt, EntityMapping("lightning_bolt", "lightningbolt") },
	{ EntityType::SmallFireball, EntityMapping("small_fireball", "fireball.small") },
	{ EntityType::AreaEffectCloud, EntityMapping("area_effect_cloud", "areaeffectcloud") },

	{ EntityType::LingeringPotion, EntityMapping("lingering_potion", "lingeringpotion") },

#ifdef MCPE_EDU
	{ EntityType::Chalkboard, EntityMapping("chalkboard") },
	{ EntityType::Npc, EntityMapping("npc") },
	{ EntityType::TripodCamera, EntityMapping("tripod_camera", "tripodcamera") }
#endif
	});

const std::unordered_map<EntityType, EntityMapping>& ENTITYTYPE_TO_STRINGS() {
	return map;
}

}

std::string EntityTypeToString(EntityType entityType, EntityTypeNamespaceRules namespaceRule) {
	if (entityType != EntityType::Undefined) {
		auto i = entitytypesinternal::ENTITYTYPE_TO_STRINGS().find(entityType);
		if (i != entitytypesinternal::ENTITYTYPE_TO_STRINGS().end()) {
			return i->second.getMappingName(namespaceRule);
		}
	}

	//	If we've made it here than we can assume we didn't find an entity with this type so let's check through our custom entities.
	//	TODO: Add code for finding custom entities created from EntityDefinitions here.

	return "unknown";
}


std::vector<std::string> EntityTypeStrings(EntityType entityType) {
	auto it = entitytypesinternal::ENTITYTYPE_TO_STRINGS().find(entityType);
	if (it == entitytypesinternal::ENTITYTYPE_TO_STRINGS().end()) {
		return {};
	}
	return !it->second.mAlternateName.empty() ?
		std::vector<std::string> { it->second.mPrimaryName, it->second.mAlternateName } :
		std::vector<std::string> { it->second.mPrimaryName };
}

EntityType EntityTypeFromString(const std::string& str) {
	if (!str.empty() && str != "unknown") {
		//	First thing we want to do is try to strip any namespace from this entity's name to make it easier to find what entity we're looking for.
		const std::size_t namespacePos = str.find(EntityDefinitionIdentifier::NAMESPACE_SEPARATOR);
		const std::string entityNamespace = str.substr(0, namespacePos);
		const std::string lowerName = Util::toLower(str.substr(namespacePos + 1));

		//	If there is no namespace or it's minecraft we want to test against vanilla entities.
		if (namespacePos == std::string::npos || entityNamespace == "minecraft") {
			//	Loop through our ENTITY_TYPE_MAP and see if this entity name is a thing.
			for (auto&& mapping : entitytypesinternal::ENTITYTYPE_TO_STRINGS()) {
				if (mapping.second.mPrimaryName == lowerName
					|| mapping.second.mAlternateName == lowerName) {
					return mapping.first;
				}
			}
		}

		//	If we've made it here than we can assume we didn't find an entity with this name so let's check through our custom entities.
		//	TODO: Add code for finding custom entities created from EntityDefinitions here.
	}

	// Unknown type
	return EntityType::Undefined;
}

TOptional<EntityType> MaybeEntityTypeFromString(const std::string& s) {
	const auto type = EntityTypeFromString(s);
	if (type != EntityType::Undefined) {
		return type;
	}
	return {};
}

std::string EntityTypeResolveAlias(const std::string& fromString, EntityTypeNamespaceRules namespaceRule) {
	return EntityTypeToString(EntityTypeFromString(fromString), namespaceRule);
}
