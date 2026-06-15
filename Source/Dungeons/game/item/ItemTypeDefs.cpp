#include "Dungeons.h" 
#include "ItemTypeDefs.h"
#include "ItemTypeBuilder.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "util/CollectionUtils.h"
#include <chrono>
#include "util/StringUtil.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionDef.h"

namespace game { namespace item { 

namespace tag {
	TOptional<ItemTag> fromString(const FString& tag) {
		const auto tagLower = tag.ToLower();
		if (tagLower == "item" || tagLower == "artifact") {
			return ItemTag::Item;
		}
		if (tagLower == "ranged" || tagLower == "rangedweapon") {
			return ItemTag::RangedWeapon;
		}
		if (tagLower == "melee" || tagLower == "meleeweapon") {
			return ItemTag::MeleeWeapon;
		}
		if (tagLower == "armor") {
			return ItemTag::Armor;
		}
		return {};
	}
}

namespace type {

namespace levels {

	TMap<ELevelNames, game::FDifficulty> matching(const Pred<const MissionDef&>& pred, game::FDifficulty difficultyThreshold) {
		TMap<ELevelNames, game::FDifficulty> restrictions;
		for (const auto* matchingDef : missions::getAllMatching(pred)) {
			restrictions.Add(matchingDef->level(), difficultyThreshold);
		}
		return restrictions;
	}

	const TMap<ELevelNames, game::FDifficulty>& dungeons() {
		static const TMap<ELevelNames, game::FDifficulty> defaultDungeonsRestrictions = [] {
			TMap<ELevelNames, game::FDifficulty> restrictions;
			restrictions.Add(ELevelNames::Invalid, FDifficulty(EGameDifficulty::Difficulty_1));
			restrictions.Append(matching(RETLAMBDA(!it.getRequiredDLC().IsSet()), FDifficulty(EGameDifficulty::Difficulty_1)));
			return restrictions;
		}();
		return defaultDungeonsRestrictions;
	}

	TMap<ELevelNames, game::FDifficulty> dlc(EDLCName dlcName) {
		const TMap<ELevelNames, game::FDifficulty> restrictions = matching(RETLAMBDA(it.getRequiredDLC() ? it.getRequiredDLC().GetValue() == dlcName : false), FDifficulty(EGameDifficulty::Difficulty_1));
		return restrictions;
	};

	const TMap<ELevelNames, game::FDifficulty>& jungle() {
		static const TMap<ELevelNames, game::FDifficulty> defaultJungleRestrictions = dlc(EDLCName::TheJungleAwakens);
		return defaultJungleRestrictions;
	};

	const TMap<ELevelNames, game::FDifficulty>& frozen() {
		static const TMap<ELevelNames, game::FDifficulty> defaultFrozenRestrictions = dlc(EDLCName::TheCreepingWinter);
		return defaultFrozenRestrictions;
	};

	const TMap<ELevelNames, game::FDifficulty>& mountains() {
		static const TMap<ELevelNames, game::FDifficulty> defaultMountainsRestrictions = dlc(EDLCName::Mountains);
		return defaultMountainsRestrictions;
	};

	const TMap<ELevelNames, game::FDifficulty>& nether() {
		static const TMap<ELevelNames, game::FDifficulty> defaultNetherRestrictions = dlc(EDLCName::Nether);
		return defaultNetherRestrictions;
	};

	const TMap<ELevelNames, game::FDifficulty>& oceans() {
		static const TMap<ELevelNames, game::FDifficulty> defaultOceansRestrictions = dlc(EDLCName::Oceans);
		return defaultOceansRestrictions;
	};	

	const TMap<ELevelNames, game::FDifficulty>& end() {
		static const TMap<ELevelNames, game::FDifficulty> defaultEndRestrictions = dlc(EDLCName::TheEnd);
		return defaultEndRestrictions;
	};

	TMap<ELevelNames, game::FDifficulty> trialsFor(ELevelNames levelname, game::FDifficulty difficultyThreshold) {
		return matching(RETLAMBDA(it.variationBasedOnLevel() ? it.variationBasedOnLevel().GetValue() == levelname : false), difficultyThreshold);		
	}

	TMap<ELevelNames, game::FDifficulty> allLevelsFor(ELevelNames levelname, game::FDifficulty difficultyThreshold) {
		const auto isLevelOrVariantOf = RETLAMBDA(it.variationBasedOnLevel().Get(it.level()) == levelname);
		return matching(isLevelOrVariantOf, difficultyThreshold);
	}
}

TMap<ELevelNames, game::FDifficulty> mergeRestrictions(const TArray<TMap<ELevelNames, game::FDifficulty>>& restrictionMaps) {
	TMap<ELevelNames, game::FDifficulty> out;
	for (auto& restrictionMap : restrictionMaps) {
		out.Append(restrictionMap);
	}
	return out;
}

using namespace std::literals::chrono_literals;
using Seconds = std::chrono::duration<float>;

ItemTypeBuilder& setupPropertiesBasedOn(ItemTypeBuilder& builder, const ItemType& baseItemDef) {
	if (baseItemDef.isVendorBlocked()) {
		builder.vendorBlocked();
	}
	return builder
		.parent(&baseItemDef)
		.difficultyThreshold(baseItemDef.difficultyThreshold()) //Same difficulty threshold
		.restrictToSameAs(baseItemDef) //Same mission restrictions
		;
}

const ItemType& melee(ItemTypeBuilder builder) {
	return (builder.tag(ItemTag::MeleeWeapon).gear().slotType(ESlotType::MeleeWeapon)).create();
}

const ItemType& meleeUniqueLimited(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return melee(builder
		.unique() //Always Unique
		.difficultyThreshold(baseItemDef.difficultyThreshold()) //Same difficulty threshold
		.restrictToSameAs(baseItemDef) //Same mission restrictions
	);
}

const ItemType& meleeUnique(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return melee(
		setupPropertiesBasedOn(builder.unique(), baseItemDef)
	);
}

const ItemType& ranged(RangedWeaponType type, ItemTypeBuilder builder) {
	return builder.tag(ItemTag::RangedWeapon).gear().slotType(ESlotType::RangedWeapon).rangedWeaponType(type).create();
}

const ItemType& rangedUniqueLimited(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return ranged(baseItemDef.getRangedWeaponType(), builder
		.unique() //Always Unique
		.difficultyThreshold(baseItemDef.difficultyThreshold()) //Same difficulty threshold
		.restrictToSameAs(baseItemDef) //Same mission restrictions
	);
}

const ItemType& rangedUnique(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return ranged(baseItemDef.getRangedWeaponType(),
		setupPropertiesBasedOn(builder.unique(), baseItemDef)
	);
}

const ItemType& armor(TArray<EArmorPropertyID> properties, ItemTypeBuilder builder) {
	return builder.tag(ItemTag::Armor).gear().slotType(ESlotType::Armor).addArmorProperties(Util::map(RETLAMBDA(FArmorPropertyData(it, EItemRarity::Common)), properties)).create();
}

const ItemType& armorUniqueLimited(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return armor(baseItemDef.getDefaultArmorPropertyIds(), builder
		.unique() //Always Unique
		.difficultyThreshold(baseItemDef.difficultyThreshold()) //Same difficulty threshold
		.restrictToSameAs(baseItemDef) //Same mission restrictions
	);
}

const ItemType& armorUnique(const ItemType& baseItemDef, ItemTypeBuilder builder) {
	return armor(baseItemDef.getDefaultArmorPropertyIds(),
		setupPropertiesBasedOn(builder.unique(), baseItemDef)
	);
}

const ItemType& projectile(ItemTag itemTag, ESlotType slotType, ItemTypeBuilder builder) {
	return builder.tag(itemTag).consumable().slotType(slotType).create();
}

const ItemType& permanent(Seconds cooldown, Seconds duration, ItemTypeBuilder builder) {
	return builder.active().cooldown(cooldown.count()).duration(duration.count()).slotType(ESlotType::ActivePermanent).tag(ItemTag::Item).create(); // so far all permanents are active
}

const ItemType& permanent(Seconds cooldown, ItemTypeBuilder builder) {
	return permanent(cooldown, 0s, builder);
}

//Permanent without cooldown becomes passive
const ItemType& permanent(ItemTypeBuilder builder) {
	return builder.passive().slotType(ESlotType::ActivePermanent).tag(ItemTag::Item).create();
}

const ItemType& instant(ItemTypeBuilder builder) {
	return builder.instant().slotType(ESlotType::None).create();
}

const ItemType& food(ItemTypeBuilder builder) {
	return builder.instant().tag(ItemTag::Food).slotType(ESlotType::None).create();
}

const ItemType& potion(ItemTypeBuilder builder) {
	return builder.instant().tag(ItemTag::Potion).slotType(ESlotType::None).create();
}

/////////////////////////////////////////////////
// Melee
/////////////////////////////////////////////////
#define LOCTEXT_NAMESPACE "ItemType"



const ItemType& Sword = melee(ItemTypeBuilder(TEXT("Sword"),
	LOCTEXT("Sword", "Sword"),
	"MeleeWeapons/Sword_Steel").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Desc_Sword", "A sturdy and reliable blade.")));

const ItemType& Sword_Unique1 = meleeUnique(Sword, ItemTypeBuilder(TEXT("Sword_Unique1"),
	LOCTEXT("Sword_Unique1", "Diamond Sword"),
	"MeleeWeapons/Sword_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Sharpness, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Desc_Sword_Unique1", "The Diamond Sword is the true mark of a hero and an accomplished adventurer.")));

const ItemType& Sword_Unique2 = meleeUnique(Sword, ItemTypeBuilder(TEXT("Sword_Unique2"),
	LOCTEXT("Sword_Unique2", "Hawkbrand"),
	"MeleeWeapons/Sword_Unique2").
	permanentEnchantments({
		{{ EEnchantmentTypeID::CriticalHit, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Desc_Sword_Unique2", "The Hawkbrand is the legendary sword of proven warriors. ")));

const ItemType& Sword_Spooky1 = meleeUniqueLimited(Sword, ItemTypeBuilder(TEXT("Sword_Spooky1"),
	LOCTEXT("Sword_Spooky1", "Sinister Sword"),
	"MeleeWeapons/Sword_Spooky1").
	permanentEnchantments({
		{{ EEnchantmentTypeID::CriticalHit, 1}, EItemRarity::Unique }
		}).
	eventType(EItemEventType::Spooky).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Desc_Sword_Spooky1", "The Sinister Sword, drawn to those who face the spookiest of nights, cuts through the night with a howl.")));

const ItemType& Katana = melee(ItemTypeBuilder(TEXT("Katana"),
	LOCTEXT("Katana", "Katana"),
	"MeleeWeapons/Katana").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Katana", "A blade fit for expert warriors and fighters, its blade is crafted to inflict precision damage.")));

const ItemType& Katana_Unique1 = meleeUnique(Katana, ItemTypeBuilder(TEXT("Katana_Unique1"),
	LOCTEXT("Katana_Unique1", "Master's Katana"),
	"MeleeWeapons/Katana_Unique1").
	//workInProgress().
	permanentEnchantments({
		{ { EEnchantmentTypeID::CriticalHit, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Katana_Unique1", "The Master's Katana has existed throughout the ages, appearing to heroes at the right moment.")));

const ItemType& Katana_Unique2 = meleeUnique(Katana, ItemTypeBuilder(TEXT("Katana_Unique2"),
	LOCTEXT("Katana_Unique2", "Dark Katana"),
	"MeleeWeapons/Katana_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Smiting, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Katana_Unique2", "A blade that will not rest until the battle has been won.")));



const ItemType& Spear = melee(ItemTypeBuilder(TEXT("Spear"),
	LOCTEXT("Spear", "Spear"),	
	"MeleeWeapons/Spear").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_Spear", "The spear, with its long reach and powerful range, is a solid choice of weapon.")).
	characteristic(LOCTEXT("long_melee_reach", "Long Melee Reach")));

const ItemType& Spear_Unique1 = meleeUnique(Spear, ItemTypeBuilder(TEXT("Spear_Unique1"),
	LOCTEXT("Spear_Unique1", "Whispering Spear"),
	"MeleeWeapons/Spear_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Echo, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Spear_Unique1", "Legend says that this cursed spear is plagued by a soul that controls the mind of any who wield it.")).
	characteristic(LOCTEXT("long_melee_reach", "Long Melee Reach")));

const ItemType& Spear_Unique2 = meleeUnique(Spear, ItemTypeBuilder(TEXT("Spear_Unique2"),
	LOCTEXT("Spear_Unique2", "Fortune spear"),
	"MeleeWeapons/Spear_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Looting, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_Spear_Unique2", "A spear that is watched over by lucky souls, bringing luck to any who wield it.")).
	characteristic(LOCTEXT("long_melee_reach", "Long Melee Reach")));


const ItemType& Rapier = melee(ItemTypeBuilder(TEXT("Rapier"),
	LOCTEXT("Rapier", "Rapier"),
	"MeleeWeapons/Rapier").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::trialsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	vendorBlocked().
	archetypes ({ EItemArchetype::Fighter, EItemArchetype::Special }).
	flavour(LOCTEXT("Flavour_Rapier", "The rapier, a nimble and narrow blade, strikes with quick ferocity.")).
	characteristic(LOCTEXT("fast_thrusts", "Fast Thrusts")));

const ItemType& Rapier_Unique1 = meleeUnique(Rapier, ItemTypeBuilder(TEXT("Rapier_Unique1"),
	LOCTEXT("Rapier_Unique1", "Bee Stinger"),
	"MeleeWeapons/Rapier_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::BusyBee, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Special, EItemArchetype::Summoner }).
	flavour(LOCTEXT("Flavour_Rapier_Unique1", "The Bee Stinger, a swift blade inspired by a bee's barb, can draw friendly bees into the fray to fight alongside you.")).
	characteristic(LOCTEXT("fast_thrusts", "Fast Thrusts")));

const ItemType& Rapier_Unique2 = meleeUnique(Rapier, ItemTypeBuilder(TEXT("Rapier_Unique2"),
	LOCTEXT("Rapier_Unique2", "Freezing Foil"),
	"MeleeWeapons/Rapier_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Freezing, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Special, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Rapier_Unique2", "This needle-like blade is ice cold to the touch and makes quick work of any cut.")).
	characteristic(LOCTEXT("fast_thrusts", "Fast Thrusts")));



const ItemType& Claymore = melee(ItemTypeBuilder(TEXT("Claymore"),
	LOCTEXT("Claymore", "Claymore"),
	"MeleeWeapons/Claymore").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_Claymore", "A massive sword that seems impossibly heavy to lift yet rests easily in a just warrior's hands. ")).
	characteristic(LOCTEXT("powerful_pushback", "Powerful Pushback")));

const ItemType& Claymore_Unique1 = meleeUnique(Claymore, ItemTypeBuilder(TEXT("Claymore_Unique1"),
	LOCTEXT("Claymore_Unique1", "Heartstealer"),
	"MeleeWeapons/Claymore_Unique1").
	//workInProgress().
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Claymore_Unique1", "Gifted to one of the Arch-Illager's most distinguished generals upon their conquest of the Squid Coast - this runeblade is infused with dark witchcraft. ")).
	characteristic(LOCTEXT("powerful_pushback", "Powerful Pushback")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Leeching, 1}, EItemRarity::Unique }
	}));

const ItemType& Claymore_Unique2 = meleeUnique(Claymore, ItemTypeBuilder(TEXT("Claymore_Unique2"),
	LOCTEXT("Claymore_Unique2", "Broadsword"),
	"MeleeWeapons/Claymore_Unique2").
	//workInProgress().
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Claymore_Unique2", "Only those with the strength of a champion and the heart of a hero can carry this massive blade.")).
	characteristic(LOCTEXT("powerful_pushback", "Powerful Pushback")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Sharpness, 1}, EItemRarity::Unique }
		}));

const ItemType& Claymore_Unique3 = meleeUnique(Claymore, ItemTypeBuilder(TEXT("Claymore_Unique3"),
	LOCTEXT("Claymore_Unique3", "Great Axeblade"),
	"MeleeWeapons/Claymore_Unique3").
	archetypes({ EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Claymore_Unique3", "A lucky blacksmith turned a workshop blunder into a battlefield wonder, fusing two weapons into something new.")).
	characteristic({ { {LOCTEXT("powerful_pushback", "Powerful Pushback")} } }).
	permanentEnchantments({
		{ { EEnchantmentTypeID::DynamoMelee, 1}, EItemRarity::Unique }
		}));

const ItemType& Claymore_Winter1 = meleeUniqueLimited(Claymore, ItemTypeBuilder(TEXT("Claymore_Winter1"),
	LOCTEXT("Claymore_Winter1", "Frost Slayer"),
	"MeleeWeapons/Claymore_Winter1").
	eventType(EItemEventType::Winter).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Claymore_Winter1", "The legendary Frost Slayer returns to the battlefield only in the darkest and coldest days of winter.")).
	characteristic(LOCTEXT("powerful_pushback", "Powerful Pushback")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Sharpness, 1}, EItemRarity::Unique }
		}));

const ItemType& ObsidianClaymore = melee(ItemTypeBuilder(TEXT("ObsidianClaymore"),
	LOCTEXT("ObsidianClaymore", "Obsidian Claymore"),
	"MeleeWeapons/ObsidianClaymore").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
		})))).
	archetype(EItemArchetype::Tank). //< --DUMMY from merge; need eyes on / jryden
	flavour(LOCTEXT("Flavour_ObsidianClaymore", "This massive blade cleaves even the thickest shulker shells with style and ease.")).
	characteristic(LOCTEXT("powerful_combo", "Very powerful combo")));

const ItemType& ObsidianClaymore_Unique1 = meleeUnique(ObsidianClaymore, ItemTypeBuilder(TEXT("ObsidianClaymore_Unique1"),
	LOCTEXT("ObsidianClaymore_Unique1", "The Starless Night"),
	"MeleeWeapons/ObsidianClaymore_Unique1").
	permanentEnchantments({
		{{ EEnchantmentTypeID::SharedPain, 1}, EItemRarity::Unique }// jryden <- should be reviewed 
		}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter }). //< --DUMMY from merge; need eyes on / jryden
	flavour(LOCTEXT("Flavour_ObsidianClaymore_Unique1", "The Starless Night is haunted by echoes of pain that linger within the pitch-black blade.")).
	characteristic(LOCTEXT("powerful_combo", "Very powerful combo"))
	);

const ItemType& Sickles = melee(ItemTypeBuilder(TEXT("Sickles"),
	LOCTEXT("Sickles", "Sickles"),
	"MeleeWeapons/Sickles").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_3))
		})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Sickles", "A ceremonial weapon that hails from the same region as the Desert Temple.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Sickles_Unique1 = meleeUnique(Sickles, ItemTypeBuilder(TEXT("Sickles_Unique1"),
	LOCTEXT("Sickles_Unique1", "Nightmare's Bite"),
	"MeleeWeapons/Sickles_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::PoisonedMelee, 1}, EItemRarity::Unique }
	}).
	//workInProgress().
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Sickles_Unique1", "The twin blades of Nightmare's Bite drip with deadly venom, still potent after all these years.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Sickles_Unique2 = meleeUnique(Sickles, ItemTypeBuilder(TEXT("Sickles_Unique2"),
	LOCTEXT("Sickles_Unique2", "The Last Laugh"),
	"MeleeWeapons/Sickles_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Prospector, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Sickles_Unique2", "Strange, distorted laughter seems to whisper from these menacing looking sickles.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));



const ItemType& Gauntlets = melee(ItemTypeBuilder(TEXT("Gauntlets"),
	LOCTEXT("Gauntlets", "Gauntlets"),	
	"MeleeWeapons/Gauntlets").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_3)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Gauntlets", "Gauntlets call back to an ancient style of hand to hand combat.")).
	characteristic(LOCTEXT("relentless_combo", "Relentless Combo")));

const ItemType& Gauntlets_Unique1 = meleeUnique(Gauntlets, ItemTypeBuilder(TEXT("Gauntlets_Unique1"),
	LOCTEXT("Gauntlets_Unique1", "Soul Fists"),
	"MeleeWeapons/Gauntlets_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EnigmaResonatorMelee, 1}, EItemRarity::Unique }
	}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_Gauntlets_Unique1", "Soul Fists are gauntlets clad with great gemstones, each containing a powerful soul.")).	
	characteristic(LOCTEXT("relentless_combo", "Relentless Combo")));

const ItemType& Gauntlets_Unique2 = meleeUnique(Gauntlets, ItemTypeBuilder(TEXT("Gauntlets_Unique2"),
	LOCTEXT("Gauntlets_Unique2", "Maulers"),
	"MeleeWeapons/Gauntlets_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Rampaging, 1}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Gauntlets_Unique2", "These claw-like weapons, wielded by ancient Illager soldiers, are savage in battle.")).
	characteristic(LOCTEXT("relentless_combo", "Relentless Combo")));

const ItemType& Gauntlets_Unique3 = meleeUnique(Gauntlets, ItemTypeBuilder(TEXT("Gauntlets_Unique3"),
	LOCTEXT("Gauntlets_Unique3", "Fighter's Bindings"),
	"MeleeWeapons/Gauntlets_Unique3").
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Gauntlets_Unique3", "Made in the wilds beyond the mountains, these gauntlets have been worn by warriors for centuries.")).
	characteristic(LOCTEXT("turbo_punches", "Turbo Punches")));

const ItemType& Battlestaff = melee(ItemTypeBuilder(TEXT("Battlestaff"),
	LOCTEXT("Battlestaff", "Battlestaff"),
	"MeleeWeapons/Battlestaff").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_3))
		})))).
	archetype(EItemArchetype::Fighter).
		flavour(LOCTEXT("Flavour_Battlestaff", "The Battlestaff is a perfectly balanced staff that is ready for any battle.")).
	characteristic(LOCTEXT("stylish_combo", "Stylish Combo")));

const ItemType& Battlestaff_Unique1 = meleeUnique(Battlestaff, ItemTypeBuilder(TEXT("Battlestaff_Unique1"),
	LOCTEXT("Battlestaff_Unique1", "Growing Staff"),
	"MeleeWeapons/Battlestaff_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Committed, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Battlestaff_Unique1", "A staff that grows and shifts as it attacks, the Growing Staff is unpredictable and powerful.")).
	characteristic(LOCTEXT("stylish_combo", "Stylish Combo")));

const ItemType& Battlestaff_Unique2 = meleeUnique(Battlestaff, ItemTypeBuilder(TEXT("Battlestaff_Unique2"),
	LOCTEXT("Battlestaff_Unique2", "Battlestaff of Terror"),
	"MeleeWeapons/Battlestaff_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Exploding, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Battlestaff_Unique2", "This staff overwhelms its target in battle to explosive effect.")).
	characteristic(LOCTEXT("stylish_combo", "Stylish Combo")));


const ItemType& Glaive = melee(ItemTypeBuilder(TEXT("Glaive"),
	LOCTEXT("Glaive", "Glaive"),	
	"MeleeWeapons/Glaive").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_2)),
		
	})))).
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_Glaive", "The glaive, wielded by the servants of the Nameless One, is a weapon with style and power.")).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));

const ItemType& Glaive_Unique1 = meleeUnique(Glaive, ItemTypeBuilder(TEXT("Glaive_Unique1"),
	LOCTEXT("Glaive_Unique1", "Grave Bane"),
	"MeleeWeapons/Glaive_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Smiting, 1}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Fighter}).
	flavour(LOCTEXT("Flavour_Glaive_Unique1", "A relic from ages of darkness; this glaive radiates potent magical energy to ward off the undead.")).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));

const ItemType& Glaive_Unique2 = meleeUnique(Glaive, ItemTypeBuilder(TEXT("Glaive_Unique2"),
	LOCTEXT("Glaive_Unique2", "Venom glaive"),
	"MeleeWeapons/Glaive_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::PoisonedMelee, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Glaive_Unique2", "A toxic cloud seems to follow the Venom Glaive wherever it goes...")).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));

const ItemType& Glaive_Spooky2 = meleeUniqueLimited(Glaive, ItemTypeBuilder(TEXT("Glaive_Spooky2"),
	LOCTEXT("Glaive_Unique1", "Cackling Broom"),
	"MeleeWeapons/Glaive_Spooky2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Smiting, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Fighter }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_Glaive_Spooky2", "Sweep your enemies off their feet with the Cackling Broom.")).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));


const ItemType& DoubleAxe = melee(ItemTypeBuilder(TEXT("DoubleAxe"),
	LOCTEXT("DoubleAxe", "Double Axe"),	
	"MeleeWeapons/DoubleAxe").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_DoubleAxe", "A devastating weapon fit for barbaric fighters.")).
	characteristic(LOCTEXT("spin_attack", "Spin attack")));

const ItemType& DoubleAxe_Unique1 = meleeUnique(DoubleAxe, ItemTypeBuilder(TEXT("DoubleAxe_Unique1"),
	LOCTEXT("DoubleAxe_Unique1", "Whirlwind"),
	"MeleeWeapons/DoubleAxe_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Shockwave, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_DoubleAxe_Unique1", "Whirlwind, forged during an epic windstorm, is a double-bladed axe that levitates slightly.")).
	characteristic(LOCTEXT("spin_attack", "Spin attack")));

const ItemType& DoubleAxe_Unique2 = meleeUnique(DoubleAxe, ItemTypeBuilder(TEXT("DoubleAxe_Unique2"),
	LOCTEXT("DoubleAxe_Unique2", "Cursed Axe"),
	"MeleeWeapons/DoubleAxe_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Exploding, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_DoubleAxe_Unique2", "This cursed, poisonous axe leaves their victims sick for years with just a single scratch.")).
	characteristic(LOCTEXT("spin_attack", "Spin attack")));

const ItemType& Whip = melee(ItemTypeBuilder(TEXT("Whip"),
	LOCTEXT("Whip", "Whip"),	
	"MeleeWeapons/Whip").
	flavour(LOCTEXT("Flavour_Whip", "A whip made of sturdy rope, very dangerous in the right hands.")).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).	
	archetype(EItemArchetype::Support).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));

const ItemType& Whip_Unique1 = meleeUnique(Whip, ItemTypeBuilder(TEXT("Whip_Unique1"),
	LOCTEXT("Whip_Unique1", "Vine Whip"),
	"MeleeWeapons/Whip_Unique1").	
	permanentEnchantments({{{EEnchantmentTypeID::JunglePoisonMelee, 1}, EItemRarity::Unique } }).
	archetypes({ EItemArchetype::Support, EItemArchetype::Fighter}).
	flavour(LOCTEXT("Flavour_Whip_Unique1", "A sturdy whip made from thick, thorn-laden vines capable of poisoning anything it touches. Be careful not to scratch yourself!")).
	characteristic(LOCTEXT("longer_melee_reach", "Longer Melee Reach")));

const ItemType& Daggers = melee(ItemTypeBuilder(TEXT("Daggers"),
	LOCTEXT("Daggers", "Daggers"),	
	"MeleeWeapons/Daggers").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Daggers", "Daggers are the weapon of cravens - or so folk say.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Daggers_Unique1 = meleeUnique(Daggers, ItemTypeBuilder(TEXT("Daggers_Unique1"),
	LOCTEXT("Daggers_Unique1", "Fangs of Frost"),
	"MeleeWeapons/Daggers_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Freezing, 1}, EItemRarity::Unique }
	}).
	//workInProgress().
	archetypes({ EItemArchetype::Support, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Daggers_Unique1", "These lauded twin daggers of the northern mountains are known to freeze their foes to solid ice.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Daggers_Unique2 = meleeUnique(Daggers, ItemTypeBuilder(TEXT("Daggers_Unique2"),
	LOCTEXT("Daggers_Unique2", "Moon Daggers"),
	"MeleeWeapons/Daggers_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EnigmaResonatorMelee, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_Daggers_Unique2", "These curved blades shine like the crescent moon on a dark night.")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Daggers_Unique3 = meleeUnique(Daggers, ItemTypeBuilder(TEXT("Daggers_Unique3"),
	LOCTEXT("Daggers_Unique3", "Sheer Daggers"),
	"MeleeWeapons/Daggers_Unique3").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Swirling, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Daggers_Unique3", "Even the simplest of farmers can wield these Shear Daggers with savage results. ")).
	characteristic(LOCTEXT("dual_wield", "Dual Wield")));

const ItemType& Chainsword = melee(ItemTypeBuilder(TEXT("Chainsword"),
	LOCTEXT("Chainsword", "Broken Sawblade"),
	"MeleeWeapons/Chainsword").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_2))

		})))).
		archetype(EItemArchetype::Tank).
		flavour(LOCTEXT("Flavour_Chainsword", "The Broken Sawblade has been ravaged by time, but it still does considerable damage.")).
	characteristic(LOCTEXT("continuous_attacks", "Continuous attacks")));

const ItemType& Chainsword_Unique1 = meleeUnique(Chainsword, ItemTypeBuilder(TEXT("Chainsword_Unique1"),
	LOCTEXT("Chainsword_Unique1", "Mechanized Sawblade"),
	"MeleeWeapons/Chainsword_Unique1").
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_Chainsword_Unique1", "The Mechanized Sawblade still glows from the fires of the Nether where it was forged.")).
	characteristic({LOCTEXT("long_continuous_attacks", "Long Continuous Attacks"), EItemRarity::Unique }));

const ItemType& VoidTouchedBlades = melee(ItemTypeBuilder(TEXT("VoidTouchedBlades"),
	LOCTEXT("VoidTouchedBlades", "Void Touched Blades"),
	"MeleeWeapons/VoidTouchedBlades").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1))
		})))).
	permanentEnchantments({ {{EEnchantmentTypeID::VoidTouchedMelee, 1}, EItemRarity::Common} }).
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_VoidTouchedBlades", "These blades are infused with a disturbing purpose after countless ages trapped in the End.")));

const ItemType& VoidTouchedBlades_Unique1 = meleeUnique(VoidTouchedBlades, ItemTypeBuilder(TEXT("VoidTouchedBlades_Unique1"),
	LOCTEXT("VoidTouchedBlades_Unique1", "The Beginning and The End"),
	"MeleeWeapons/VoidTouchedBlades_Unique1").
	permanentEnchantments({
		{{ EEnchantmentTypeID::Leeching, 1}, EItemRarity::Common},
		{{ EEnchantmentTypeID::VoidTouchedMelee, 1}, EItemRarity::Common }//jryden <- should be reviewed
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_VoidTouchedBlades_Unique1", "Forged by the survivors of a doomed expedition to the End, these twin blades carry dark secrets.")));

const ItemType& Backstabber = melee(ItemTypeBuilder(TEXT("Backstabber"),
	LOCTEXT("Backstabber", "Backstabber"),
	"MeleeWeapons/Backstabber").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1))
			})))).
	permanentEnchantments({
		{{ EEnchantmentTypeID::Backstabber, 1}, EItemRarity::Common},
		}).
	archetype(EItemArchetype::Acrobat).
	flavour(LOCTEXT("Desc_Backstabber", "The preferred blade of thieves and assassins, the Backstabber is a must in any rogue's pack.")));

const ItemType& Backstabber_Unique1 = meleeUnique(Backstabber, ItemTypeBuilder(TEXT("Backstabber_Unique1"),
	LOCTEXT("Backstabber_Unique1", "Swift Striker"),
	"MeleeWeapons/Backstabber_Unique1").
	permanentEnchantments({
		{{ EEnchantmentTypeID::Backstabber, 1}, EItemRarity::Common},
		{{ EEnchantmentTypeID::Echo, 1}, EItemRarity::Common},
		}).
		archetypes({ EItemArchetype::Acrobat, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Desc_Backstabber_Unique1", "A blade for those who know that the surest way to victory is to strike without being seen.")));

const ItemType& Cutlass = melee(ItemTypeBuilder(TEXT("Cutlass"),
	LOCTEXT("Cutlass", "Cutlass"),	
	"MeleeWeapons/Cutlass").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Cutlass", "This curved blade, wielded by the warriors of the Squid Coast, requires a steady hand in battle.")).
	characteristic(LOCTEXT("reliable_combo", "Reliable Combo")));

const ItemType& Cutlass_Unique1 = meleeUnique(Cutlass, ItemTypeBuilder(TEXT("Cutlass_Unique1"),
	LOCTEXT("Cutlass_Unique1", "Nameless Blade"),
	"MeleeWeapons/Cutlass_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Weakening, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Cutlass_Unique1", "This deadly blade's story was lost to the endless sands of time.")).
	characteristic(LOCTEXT("reliable_combo", "Reliable Combo")));

const ItemType& Cutlass_Unique2 = meleeUnique(Cutlass, ItemTypeBuilder(TEXT("Cutlass_Unique2"),
	LOCTEXT("Cutlass_Unique2", "Dancer's Sword"),
	"MeleeWeapons/Cutlass_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Rampaging, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Cutlass_Unique2", "Warriors who view battle as a dance with death prefer double-bladed swords.")).
	characteristic(LOCTEXT("reliable_combo", "Reliable Combo")));

const ItemType& Cutlass_Year1 = meleeUnique(Cutlass, ItemTypeBuilder(TEXT("Cutlass_Year1"),
	LOCTEXT("Cutlass_Year1", "Sparkler "),
	"MeleeWeapons/Cutlass_Year1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Rampaging, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Cutlass_Year1", "The Sparkler lights up the party as it cuts through the darkness.")).
	eventType(EItemEventType::Year1).
	characteristic(LOCTEXT("reliable_combo", "Reliable Combo")));

const ItemType& Boneclub = melee(ItemTypeBuilder(TEXT("Boneclub"),
	LOCTEXT("Boneclub", "Boneclub"),
	"MeleeWeapons/Boneclub").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_2))
		})))).
	archetype(EItemArchetype::Fighter).
	characteristic(LOCTEXT("great_pushback", "Great Pushback")).
	flavour(LOCTEXT("Flavour_Boneclub", "Those who wield a Bone Club prefer a less-subtle approach to problem-solving.")));

const ItemType& Boneclub_Unique1 = meleeUnique(Boneclub, ItemTypeBuilder(TEXT("Boneclub_Unique1"),
	LOCTEXT("Boneclub_Unique1", "Bone Cudgel"),
	"MeleeWeapons/Boneclub_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::BaneOfIllagers, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Boneclub_Unique1", "The Bone Cudgel is old enough to be considered an ancient treasure, but still menacing even by modern standards.")).
	characteristic(LOCTEXT("great_pushback", "Great Pushback")));



const ItemType& Mace = melee(ItemTypeBuilder(TEXT("Mace"), 
	LOCTEXT("Mace", "Mace"),	
	"MeleeWeapons/Mace").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Mace", "The Mace is a brutal tool of war and what it lacks in finesse; it makes up for in power.")));

const ItemType& Mace_Unique1 = meleeUnique(Mace, ItemTypeBuilder(TEXT("Mace_Unique1"),
	LOCTEXT("Mace_Unique1", "Sun's Grace"), 
	"MeleeWeapons/Mace_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::RadianceMelee, 1}, EItemRarity::Unique }
	}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Mace_Unique1", "This mace, engraved with secret healing runes, grants powerful restorative powers.")));

const ItemType& Mace_Unique2 = meleeUnique(Mace, ItemTypeBuilder(TEXT("Mace_Unique2"),
	LOCTEXT("Mace_Unique2", "Flail"),
	"MeleeWeapons/Mace_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Chains, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Mace_Unique2", "This ancient weapon inflicts grave blunt damage to those who cannot evade the deadly metal ball.")));



const ItemType& Pickaxe = melee(ItemTypeBuilder(TEXT("Pickaxe"),
	LOCTEXT("Pickaxe", "Pickaxe"),	
	"MeleeWeapons/Pickaxe_Steel").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Pickaxe", "The pickaxe has been the iconic tool of adventurers and heroes for as long as anyone can remember.")));

const ItemType& Pickaxe_Unique1 = meleeUnique(Pickaxe, ItemTypeBuilder(TEXT("Pickaxe_Unique1"),
	LOCTEXT("Pickaxe_Unique1", "Diamond Pickaxe"),
	"MeleeWeapons/Pickaxe_Unique1_Steel").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Prospector, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Pickaxe_Unique1", "Diamond is one of the most durable materials, making it an excellent choice for a pickaxe.")));

const ItemType& Pickaxe_Unique2 = meleeUnique(Pickaxe, ItemTypeBuilder(TEXT("Pickaxe_Unique2"),
	LOCTEXT("Pickaxe_Unique2", "The Monkey Motivator"),
	"MeleeWeapons/Pickaxe_Unique2_Steel").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Knockback, 1}, EItemRarity::Unique }
		}).
	workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_Pickaxe_Unique2", "A strange weapon that makes a sound that no creature from the Overworld has ever made.")));




const ItemType& Axe = melee(ItemTypeBuilder(TEXT("Axe"),
	LOCTEXT("Axe", "Axe"),	
	"MeleeWeapons/Axe").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_Axe", "The axe is an effective weapon, favored by the relentless Vindicators of the Arch-Illager's army.")).
	characteristic(LOCTEXT("spin_attack_move", "Spin attack move")));

const ItemType& Axe_Unique1 = meleeUnique(Axe, ItemTypeBuilder(TEXT("Axe_Unique1"),
	LOCTEXT("Axe_Unique1", "Firebrand"),
	"MeleeWeapons/Axe_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::FireAspect, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage}).
	flavour(LOCTEXT("Flavour_Axe_Unique1", "Crafted in the blackest depths of the Fiery Forge and enchanted with fiery powers.")).
	characteristic(LOCTEXT("spin_attack_move", "Spin attack move")));

const ItemType& Axe_Unique2 = meleeUnique(Axe, ItemTypeBuilder(TEXT("Axe_Unique2"),
	LOCTEXT("Axe_Unique2", "Highland Axe"),
	"MeleeWeapons/Axe_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Stunning, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Support}).
	flavour(LOCTEXT("Flavour_Axe_Unique2", "Expertly crafted and a polished weapon of war, the Highland Axe also makes a daring backscratcher.")).
	characteristic(LOCTEXT("spin_attack_move", "Spin attack move")));



const ItemType& Hammer = melee(ItemTypeBuilder(TEXT("Hammer"),
	LOCTEXT("Hammer", "Great Hammer"),	
	"MeleeWeapons/Hammer").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_Hammer", "Blacksmiths and soldiers alike use the Great Hammer for its strength in forging and in battle.")).
	characteristic(LOCTEXT("great_splash", "Great Splash")));

const ItemType& Hammer_Unique1 = meleeUnique(Hammer, ItemTypeBuilder(TEXT("Hammer_Unique1"),
	LOCTEXT("Hammer_Unique1", "Stormlander"),
	"MeleeWeapons/Hammer_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Thundering, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Tank, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Hammer_Unique1", "The Stormlander, enchanted with the power of the raging storm, is a treasure of the Illagers.")).
	characteristic(LOCTEXT("great_splash", "Great Splash")));

const ItemType& Hammer_Unique2 = meleeUnique(Hammer, ItemTypeBuilder(TEXT("Hammer_Unique2"),
	LOCTEXT("Hammer_Unique2", "Hammer of Gravity"),
	"MeleeWeapons/Hammer_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::GravityMelee, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Tank, EItemArchetype::Mage}).
	flavour(LOCTEXT("Flavour_Hammer_Unique2", "A hammer, embedded with a crystal that harnesses the power of gravity, that is incredibly powerful.")).
	characteristic(LOCTEXT("great_splash", "Great Splash")));



const ItemType& SoulScythe = melee(ItemTypeBuilder(TEXT("SoulScythe"),
	LOCTEXT("SoulScythe", "Soul Scythe"),	
	"MeleeWeapons/SoulScythe").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulScythe", "A cruel reaper of souls, the Soul Scythe is unsentimental in its work.")).
	soulGatherItem(1));

const ItemType& SoulScythe_Unique1 = meleeUnique(SoulScythe, ItemTypeBuilder(TEXT("SoulScythe_Unique1"),
	LOCTEXT("SoulScythe_Unique1", "Jailor's Scythe"),
	"MeleeWeapons/SoulScythe_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Chains, 1}, EItemRarity::Unique }
		}).
	//workInProgress().
	archetypes({ EItemArchetype::Mage, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_SoulScythe_Unique1", "This scythe belonged to the terror of Highblock Keep, the Jailor.")).
	soulGatherItem(1));

const ItemType& SoulScythe_Unique2 = meleeUnique(SoulScythe, ItemTypeBuilder(TEXT("SoulScythe_Unique2"),
	LOCTEXT("SoulScythe_Unique2", "Frost Scythe"),
	"MeleeWeapons/SoulScythe_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Freezing, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_SoulScythe_Unique2", "The Frost Scythe is an indestructible blade that is freezing to the touch and never seems to melt.")).
	soulGatherItem(1));

const ItemType& SoulScythe_Spooky2 = meleeUniqueLimited(SoulScythe, ItemTypeBuilder(TEXT("SoulScythe_Spooky2"),
	LOCTEXT("SoulScythe_Spooky2", "Skull Scythe"),
	"MeleeWeapons/SoulScythe_Spooky2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Freezing, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Soul }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_SoulScythe_Spooky2", "Don't ask what unnatural creature's bones were used to build this weapon. You don't want to know.")).
	soulGatherItem(1));

const ItemType& SoulKnife = melee(ItemTypeBuilder(TEXT("SoulKnife"),
	LOCTEXT("SoulKnife", "Soul Knife"),
	"MeleeWeapons/SoulKnife").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::trialsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1))
		
	})))).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulKnife", "A ceremonial knife that uses magical energy to hold the wrath of souls inside its blade.")).
	soulGatherItem(1).
	characteristic(LOCTEXT("thrust_attack", "Thrust attack")));

const ItemType& SoulKnife_Unique1 = meleeUnique(SoulKnife, ItemTypeBuilder(TEXT("SoulKnife_Unique1"),
	LOCTEXT("SoulKnife_Unique1", "Truthseeker"),
	"MeleeWeapons/SoulKnife_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Committed, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_SoulKnife_Unique1", "The warden of Highblock Keep kept this unpleasant blade by their side during interrogations. ")).
	soulGatherItem(1).
	characteristic(LOCTEXT("thrust_attack", "Thrust attack")));

const ItemType& SoulKnife_Unique2 = meleeUnique(SoulKnife, ItemTypeBuilder(TEXT("SoulKnife_Unique2"),
	LOCTEXT("SoulKnife_Unique2", "Eternal knife"),
	"MeleeWeapons/SoulKnife_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::SoulSiphon, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulKnife_Unique2", "A disturbing aura surrounds this knife, as if it has existed for all time and will outlive us all.")).
	soulGatherItem(1).
	characteristic(LOCTEXT("thrust_attack", "Thrust attack")));

const ItemType& TempestKnife = melee(ItemTypeBuilder(TEXT("TempestKnife"),
	LOCTEXT("TempestKnife", "Tempest knife"),
	"MeleeWeapons/TempestKnife").
	permanentEnchantments({
		{ {	EEnchantmentTypeID::Rushdown,	1}, EItemRarity::Common }
		}).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		})))).
	archetype(EItemArchetype::Acrobat).
	flavour(LOCTEXT("Flavour_TempestKnife", "This knife slices through enemies like the wind that cuts between the mountaintops.")));

const ItemType& TempestKnife_Unique1 = meleeUnique(TempestKnife, ItemTypeBuilder(TEXT("TempestKnife_Unique1"),
	LOCTEXT("TempestKnife_Unique1", "Resolute Tempest knife"),
	"MeleeWeapons/TempestKnife_Unique1").
	permanentEnchantments({
		{ {	EEnchantmentTypeID::Rushdown,	1}, EItemRarity::Common },
		{ {	EEnchantmentTypeID::Committed,	1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Acrobat}).
	flavour(LOCTEXT("Flavour_TempestKnife_Unique1", "Passed down by nomads who roam the mountain peaks, this knife has been used in countless battles.")));

const ItemType& TempestKnife_Unique2 = meleeUnique(TempestKnife, ItemTypeBuilder(TEXT("TempestKnife_Unique2"),
	LOCTEXT("TempestKnife_Unique2", "Chill Gale knife"),
	"MeleeWeapons/TempestKnife_Unique2").
	permanentEnchantments({
		{ {	EEnchantmentTypeID::Rushdown,	1}, EItemRarity::Common },
		{ {	EEnchantmentTypeID::Freezing,	1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_TempestKnife_Unique2", "Crafted from the never-melting ice atop the mountain peaks, this knife is forever icy to the touch.")));

const ItemType& CoralBlade = melee(ItemTypeBuilder(TEXT("CoralBlade"),
	LOCTEXT("CoralBlade", "Coral Blade"),
	"MeleeWeapons/CoralBlade").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Desc_CoralBlade", "The Coral Blade cuts through enemies with stinging accuracy.")).
	characteristic(LOCTEXT("rapid_slashes", "Rapid Slashes")));

const ItemType& CoralBlade_Unique1 = meleeUnique(CoralBlade, ItemTypeBuilder(TEXT("CoralBlade_Unique1"),
	LOCTEXT("CoralBlade_Unique1", "Sponge Striker"),
	"MeleeWeapons/CoralBlade_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::SpongeStrike, 1}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_CoralBlade_Unique1", "This blade may look colorless and dead, but it soaks up energy in combat and expels it in a powerful burst.")).
	characteristic(LOCTEXT("rapid_slashes", "Rapid Slashes")));

const ItemType& Anchor = melee(ItemTypeBuilder(TEXT("Anchor"),
	LOCTEXT("Anchor", "Anchor"),
	"MeleeWeapons/Anchor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Desc_Anchor", "Those strong enough to wield the Anchor in battle follow the tradition of legendary seafaring warriors.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Heavyweight, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::GravityMelee, 1}, EItemRarity::Common }
	}).
	characteristic(LOCTEXT("slow_but_powerful", "Slow but Powerful"))
);

const ItemType& Anchor_Unique1 = meleeUnique(Anchor, ItemTypeBuilder(TEXT("Anchor_Unique1"),
	LOCTEXT("Anchor_Unique1", "Encrusted Anchor"),
	"MeleeWeapons/Anchor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::JunglePoisonMelee, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::Heavyweight, 1 },      EItemRarity::Common },
		{ { EEnchantmentTypeID::GravityMelee, 1}, EItemRarity::Common }
	}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Anchor_Unique1", "This Encrusted Anchor was lost at sea long ago and has become harsh and corrosive during its ages of neglect.")).
	characteristic(LOCTEXT("slow_but_powerful", "Slow but Powerful")));

/////////////////////////////////////////////////
// Ranged
/////////////////////////////////////////////////


const ItemType& Bow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("Bow"),
	LOCTEXT("Bow", "Bow"),	
	"RangedWeapons/Bow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods,      FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt,       FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle,       FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Bow", "A simple but well-rounded piece of weaponry. The hunters of the Pumpkin Pastures say that a bow doesn't let you down, unlike other trinkets.")));

const ItemType& Bow_Spooky1 = rangedUniqueLimited(Bow, ItemTypeBuilder(TEXT("Bow_Spooky1"),
	LOCTEXT("Bow_Spooky1", "Haunted Bow"),
	"RangedWeapons/Bow_Spooky1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::BonusShot, 1}, EItemRarity::Unique }
		}).
	eventType(EItemEventType::Spooky).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Bow_Spooky1", "What indescribable horror! The creeping tentacles of this bow reach for the unknowable Void.")));

const ItemType& Bow_Unique1 = rangedUnique(Bow, ItemTypeBuilder(TEXT("Bow_Unique1"),
	LOCTEXT("Bow_Unique1", "Bonebow"),
	"RangedWeapons/Bow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Growing, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Bow_Unique1", "The Bonebow is the pride of Pumpkin Pastures, crafted within the walls of their humble village.")));

const ItemType& Bow_Unique2 = rangedUnique(Bow, ItemTypeBuilder(TEXT("Bow_Unique2"),
	LOCTEXT("Bow_Unique2", "Twin Bow"),
	"RangedWeapons/Bow_Unique2").
	permanentEnchantments({ 
		{ { EEnchantmentTypeID::BonusShot, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Bow_Unique2", "The Twin Bow is the champion of the hero who finds themselves outnumbered and alone.")));

const ItemType& Trickbow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("Trickbow"),
	LOCTEXT("Trickbow", "Trickbow"),
	"RangedWeapons/Trickbow").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Ricochet, 99}, EItemRarity::Common }
		}).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_3))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Trickbow", "A sleek bow that seems to never miss its target. ")));

const ItemType& Trickbow_Unique1 = rangedUnique(Trickbow, ItemTypeBuilder(TEXT("Trickbow_Unique1"),
	LOCTEXT("Trickbow_Unique1", "The Green Menace"),
	"RangedWeapons/Trickbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::PoisonedRanged, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::Ricochet, 99}, EItemRarity::Common }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Trickbow_Unique1", "The arrows fired from the Green Menace always hit their intended target, even in the thickest fog."))
);

const ItemType& Trickbow_Unique2 = rangedUnique(Trickbow, ItemTypeBuilder(TEXT("Trickbow_Unique2"),
	LOCTEXT("Trickbow_Unique2", "The Pink Scoundrel"),
	"RangedWeapons/Trickbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::WildRage, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::Ricochet, 99} , EItemRarity::Common }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Trickbow_Unique2", "It would be a deadly mistake to underestimate the power of the Pink Scoundrel."))
);

const ItemType& Trickbow_Year1 = rangedUnique(Trickbow, ItemTypeBuilder(TEXT("Trickbow_Year1"),
	LOCTEXT("Trickbow_Year1", "Sugar Rush"),
	"RangedWeapons/Trickbow_Year1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::WildRage, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::Ricochet, 99} , EItemRarity::Common }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	eventType(EItemEventType::Year1).
	flavour(LOCTEXT("Flavour_Trickbow_Year1", "This candy-colored bow has a surprisingly bitter bite."))
);

const ItemType& Shortbow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("Shortbow"),
	LOCTEXT("Shortbow", "Shortbow"),
	"RangedWeapons/Shortbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Shortbow", "The preferred weapon of thieves and rogues, the Shortbow is nimble and lethal at short range."))
	);

const ItemType& Shortbow_Unique1 = rangedUnique(Shortbow, ItemTypeBuilder(TEXT("Shortbow_Unique1"),
	LOCTEXT("Shortbow_Unique1", "Purple Storm"),
	"RangedWeapons/Shortbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::RapidFire, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Shortbow_Unique1", "Named the Purple Storm after its lovely hue, this legendary bow packs a powerful punch."))
);

const ItemType& Shortbow_Unique2 = rangedUnique(Shortbow, ItemTypeBuilder(TEXT("Shortbow_Unique2"),
	LOCTEXT("Shortbow_Unique2", "Mechanical Shortbow"),
	"RangedWeapons/Shortbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Accelerating, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Shortbow_Unique2", "A brand new development in quick firing technology, this bow packs a swift punch."))
);

const ItemType& Shortbow_Unique3 = rangedUnique(Shortbow, ItemTypeBuilder(TEXT("Shortbow_Unique3"),
	LOCTEXT("Shortbow_Unique3", "Love Spell Bow"),
	"RangedWeapons/Shortbow_Unique3").
	permanentEnchantments({
		{ { EEnchantmentTypeID::WildRage, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Shortbow_Unique3", "Love can be magical, but when it becomes too powerful, it can turn into a frenzy."))
);

const ItemType& Longbow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("Longbow"),
	LOCTEXT("Longbow", "Longbow"),
	"RangedWeapons/Longbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven,       FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon,     FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::trialsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Longbow", "The Longbow, crafted for hunting rather than battle, is still useful in a fight.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
	);

const ItemType& Longbow_Unique1 = rangedUnique(Longbow, ItemTypeBuilder(TEXT("Longbow_Unique1"),
	LOCTEXT("Longbow_Unique1", "Guardian Bow"),
	"RangedWeapons/Longbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Supercharge, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Longbow_Unique1", "Forged from fossilized coral, the Guardian Bow is a remnant from sunken civilizations of lost ages. ")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks")));

const ItemType& Longbow_Unique2 = rangedUnique(Longbow, ItemTypeBuilder(TEXT("Longbow_Unique2"),
	LOCTEXT("Longbow_Unique2", "Red Snake"),
	"RangedWeapons/Longbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::FuseShot, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_Longbow_Unique2", "The Red Snake radiates an explosive heat, making it a deadly fire risk in the dry, desert lands.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
);

const ItemType& HuntingBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("HuntingBow"),
	LOCTEXT("HuntingBow", "Hunting Bow"),
	"RangedWeapons/HuntingBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Summoner).
	flavour(LOCTEXT("Flavour_HuntingBow", "This enchanted bow strengthens the bond between the hunter and their animal companion.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::HuntingBowEnchantment, 1}, EItemRarity::Common }
	}));

const ItemType& HuntingBow_Unique1 = rangedUnique(HuntingBow, ItemTypeBuilder(TEXT("HuntingBow_Unique1"),
	LOCTEXT("HuntingBow_Unique1", "Hunter's Promise"),
	"RangedWeapons/HuntingBow_Unique1").
	archetypes({ EItemArchetype::Archer, EItemArchetype::Summoner }).
	flavour(LOCTEXT("Flavour_HuntingBow_Unique1", "This enchanted bow deepens the bond between the hunter and their trusted animal companion.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::HuntingBowEnchantment, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::Infinity, 1}, EItemRarity::Unique }
	}));

const ItemType& HuntingBow_Unique2 = rangedUnique(HuntingBow, ItemTypeBuilder(TEXT("HuntingBow_Unique2"),
	LOCTEXT("HuntingBow_Unique2", "Master's Bow"),
	"RangedWeapons/HuntingBow_Unique2").
	//workInProgress().
	archetypes({ EItemArchetype::Archer, EItemArchetype::Summoner }).
	flavour(LOCTEXT("Flavour_HuntingBow_Unique2", "The bow prefered by the masters of archery across the Overworld, no matter how simple it appears.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::HuntingBowEnchantment, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::Power, 1}, EItemRarity::Unique }
		}));

const ItemType& HuntingBow_Unique3 = rangedUnique(HuntingBow, ItemTypeBuilder(TEXT("HuntingBow_Unique3"),
	LOCTEXT("HuntingBow_Unique3", "Ancient Bow"),
	"RangedWeapons/HuntingBow_Unique3").
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_HuntingBow_Unique3", "The Ancient Bow is still as sprightly as the day it was first strung.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::HuntingBowEnchantment, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::DynamoRanged, 1}, EItemRarity::Unique }
		}));

const ItemType& SoulBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("SoulBow"),
	LOCTEXT("SoulBow", "Soul Bow"),
	"RangedWeapons/SoulBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_3))
	})))).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulBow", "The Soul Bow shimmers with all the beauty and fury of an attacking Vex. ")).
	soulGatherItem(1)
	);

const ItemType& SoulBow_Unique1 = rangedUnique(SoulBow, ItemTypeBuilder(TEXT("SoulBow_Unique1"),
	LOCTEXT("SoulBow_Unique1", "Nocturnal Bow"),
	"RangedWeapons/SoulBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::TempoTheft, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_SoulBow_Unique1", "The souls bound to the bow guide the arrows to their targets and cause it to glow slightly. ")).
	soulGatherItem(1)
	);

const ItemType& SoulBow_Unique2 = rangedUnique(SoulBow, ItemTypeBuilder(TEXT("SoulBow_Unique2"),
	LOCTEXT("SoulBow_Unique2", "Bow of Lost Souls"),
	"RangedWeapons/SoulBow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::MultiShot, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Archer }).
	flavour(LOCTEXT("Flavour_SoulBow_Unique2", "This bow, made of cursed bones, strips the living of their very souls.")).
	soulGatherItem(1)
);

const ItemType& TwistingVineBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("TwistingVineBow"),
	LOCTEXT("TwistingVineBow", "Twisting Vine Bow"),
	"RangedWeapons/TwistingVineBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_2))
		})))).
	archetype(EItemArchetype::Archer).
	characteristic({ LOCTEXT("Characteristic_TwistingVineBow", "Poison Trail") }).
	flavour(LOCTEXT("Flavour_TwistingVineBow", "This bow writhes within your grasp as if trying to wrap its tendrils around everything in its path."))
);

const ItemType& TwistingVineBow_Unique1 = rangedUnique(TwistingVineBow, ItemTypeBuilder(TEXT("TwistingVineBow_Unique1"),
	LOCTEXT("TwistingVineBow_Unique1", "Weeping Vine Bow"),
	"RangedWeapons/TwistingVineBow_Unique1").
	permanentEnchantments({
	{ { EEnchantmentTypeID::RollCharge, 1}, EItemRarity::Unique } }).
		restrictTo(RETLAMBDA2((mergeRestrictions({
			levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1))
			})))).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_TwistingVineBow_Unique1", "The Weeping Vine Bow's toxic vines create a poisonous haze on the battlefield."))
);

const ItemType& SoulBow_Winter1 = rangedUniqueLimited(SoulBow, ItemTypeBuilder(TEXT("SoulBow_Winter1"),
	LOCTEXT("SoulBow_Winter1", "Shivering Bow"),
	"RangedWeapons/SoulBow_Winter1").
	permanentEnchantments({ {
{ EEnchantmentTypeID::TempoTheft, 1}, EItemRarity::Unique }
		}).
	eventType(EItemEventType::Winter).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_SoulBow_Winter1", "The Shivering Bow pierces even the most ancient ice and strikes a chill into the heart of any enemy.")).
	soulGatherItem(1)
);

const ItemType& PowerBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("Powerbow"),
	LOCTEXT("PowerBow", "Power Bow"),
	"RangedWeapons/PowerBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Powerbow", "The question of what makes the Power Bow so powerful has puzzled the minds of learned folk for ages.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
	);

const ItemType& PowerBow_Unique1 = rangedUnique(PowerBow, ItemTypeBuilder(TEXT("Powerbow_Unique1"),
	LOCTEXT("PowerBow_Unique1", "Elite Power Bow"),
	"RangedWeapons/PowerBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Power, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_Powerbow_Unique1", "The secrets of the Elite Power Bow's construction have been lost without a trace.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
	);

const ItemType& PowerBow_Unique2 = rangedUnique(PowerBow, ItemTypeBuilder(TEXT("Powerbow_Unique2"),
	LOCTEXT("PowerBow_Unique2", "Sabrewing"),
	"RangedWeapons/PowerBow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::RadianceRanged, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_Powerbow_Unique2", "This bow, built for a long-lost champion, feels right in the hands of those who seek justice.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
);

const ItemType& PowerBow_Spooky2 = rangedUniqueLimited(PowerBow, ItemTypeBuilder(TEXT("PowerBow_Spooky2"),
	LOCTEXT("PowerBow_Spooky2", "Phantom Bow"),
	"RangedWeapons/PowerBow_Spooky2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Power, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter, EItemArchetype::Support }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_PowerBow_Spooky2", "The arrows that take flight from the Phantom Bow swoop down on your enemies like the bow's nightmarish namesake.")).
	characteristic(LOCTEXT("strong_charged_attacks", "Strong Charged Attacks"))
);

const ItemType& CogCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("CogCrossbow"),
	LOCTEXT("CogCrossbow", "Cog Crossbow"),
	"RangedWeapons/CogCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_2))
		})))).
	blockedEnchantments({ EEnchantmentTypeID::RapidFire, EEnchantmentTypeID::Accelerating }).
	permanentEnchantments({
		{ { EEnchantmentTypeID::CogCrossbowEnchantment, 1 }, EItemRarity::Common }
	}).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_CogCrossbow", "The gears of this ancient Cog Crossbow still turn smoothly, making it a reliable weapon choice to this day.")));

const ItemType& CogCrossbow_Unique1 = rangedUnique(CogCrossbow, ItemTypeBuilder(TEXT("CogCrossbow_Unique1"),
	LOCTEXT("CogCrossbow_Unique1", "Pride of the Piglins"),
	"RangedWeapons/CogCrossbow_Unique1").
	blockedEnchantments({ EEnchantmentTypeID::RapidFire, EEnchantmentTypeID::Accelerating }).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Piercing, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::CogCrossbowEnchantment, 1 }, EItemRarity::Common }
	}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter}).
	flavour(LOCTEXT("Flavour_CogCrossbow_Unique1", "Found in the farthest reaches of the Nether, the Pride of the Piglins is both vintage and vicious.")));



const ItemType& Crossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("Crossbow"),
	LOCTEXT("Crossbow", "Crossbow"),
	"RangedWeapons/Crossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Crossbow", "The crossbow is the ranged weapon of the Illagers and is a common sight among Pillager warriors.")).
	characteristic(LOCTEXT("faster_projectiles", "Faster Projectiles")));

const ItemType& Crossbow_Unique1 = rangedUnique(Crossbow, ItemTypeBuilder(TEXT("Crossbow_Unique1"),
	LOCTEXT("Crossbow_Unique1", "The Slicer"),
	"RangedWeapons/Crossbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Piercing, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_Crossbow_Unique1", "The peak of mischievous Illager-engineering, the famous Slicer was designed to fire bolts that pierce through even the thickest of armor.")).
	characteristic(LOCTEXT("faster_projectiles", "Faster Projectiles")));

const ItemType& Crossbow_Unique2 = rangedUnique(Crossbow, ItemTypeBuilder(TEXT("Crossbow_Unique2"),
	LOCTEXT("Crossbow_Unique2", "Azure Seeker"),
	"RangedWeapons/Crossbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::RapidFire, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_Crossbow_Unique2", "If the color blue had a sound, it would be the shots fired by the Azure Seeker.")).
	characteristic(LOCTEXT("faster_projectiles", "Faster Projectiles")));

const ItemType& SpiderCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("SpiderCrossbow"),
	LOCTEXT("SpiderCrossbow", "Spider Crossbow"),
	"RangedWeapons/SpiderCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	workInProgress().
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_SpiderCrossbow", "No one knows how these bows were made, if it was originally a spider first or a magical crossbow.")).
	characteristic(LOCTEXT("webs_enemies", "Webs Enemies")));


const ItemType& HeavyCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("HeavyCrossbow"),
	LOCTEXT("HeavyCrossbow", "Heavy Crossbow"),
	"RangedWeapons/HeavyCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_HeavyCrossbow", "The weighted crossbow is a damage-dealing menace and a real threat from a ranged distance.")).
	characteristic(LOCTEXT("powerful_shots", "Powerful Shots")));

const ItemType& HeavyCrossbow_Unique1 = rangedUnique(HeavyCrossbow, ItemTypeBuilder(TEXT("HeavyCrossbow_Unique1"),
	LOCTEXT("HeavyCrossbow_Unique1", "Doom Crossbow"),
	"RangedWeapons/HeavyCrossbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Punch, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_HeavyCrossbow_Unique1", "Many thought that the Doom Crossbow was just a myth, but this time the rumors turned out to be true.")).
	characteristic(LOCTEXT("powerful_shots", "Powerful Shots")));

const ItemType& HeavyCrossbow_Unique2 = rangedUnique(HeavyCrossbow, ItemTypeBuilder(TEXT("HeavyCrossbow_Unique2"),
	LOCTEXT("HeavyCrossbow_Unique2", "Slayer Crossbow"),
	"RangedWeapons/HeavyCrossbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Ricochet, 1}, EItemRarity::Unique } //
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_HeavyCrossbow_Unique2", "The Slayer Crossbow is the treasured heirloom of many legendary hunters.")).
	characteristic(LOCTEXT("powerful_shots", "Powerful Shots")));

const ItemType& RapidCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("RapidCrossbow"),
	LOCTEXT("RapidCrossbow", "Rapid Crossbow"),
	"RangedWeapons/RapidCrossbow").
	characteristic(LOCTEXT("high_firerate", "High Firerate")).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::trialsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_RapidCrossbow", "New reloading technology helps this crossbow fire more rapidly than those that came before it.")));

const ItemType& RapidCrossbow_Unique1 = rangedUnique(RapidCrossbow, ItemTypeBuilder(TEXT("RapidCrossbow_Unique1"),
	LOCTEXT("RapidCrossbow_Unique1", "Butterfly Crossbow"),
	"RangedWeapons/RapidCrossbow_Unique1").
	characteristic(LOCTEXT("high_firerate", "High Firerate")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::BonusShot, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support}).
	flavour(LOCTEXT("Flavour_RapidCrossbow_Unique1", "This crossbow moves faster than wings in flight and strikes down enemies before they see the bolt.")));

const ItemType& RapidCrossbow_Unique2 = rangedUnique(RapidCrossbow, ItemTypeBuilder(TEXT("RapidCrossbow_Unique2"),
	LOCTEXT("RapidCrossbow_Unique2", "Auto Crossbow"),
	"RangedWeapons/RapidCrossbow_Unique2").
	characteristic(LOCTEXT("high_firerate", "High Firerate")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Accelerating, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_RapidCrossbow_Unique2", "The Arch-Illager said the design for this powerful crossbow came to him in a vision.")));

const ItemType& ScatterCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("ScatterCrossbow"),
	LOCTEXT("ScatterCrossbow", "Scatter Crossbow"),
	"RangedWeapons/ScatterCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_ScatterCrossbow", "This crossbow, modified to hold and fire multiple bolts, is also a half-decent musical instrument. ")).
	characteristic(LOCTEXT("multiple_projectiles", "Multiple projectiles")));

const ItemType& ScatterCrossbow_Unique1 = rangedUnique(ScatterCrossbow, ItemTypeBuilder(TEXT("ScatterCrossbow_Unique1"),
	LOCTEXT("ScatterCrossbow_Unique1", "Harp Crossbow"),
	"RangedWeapons/ScatterCrossbow_Unique1").
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_ScatterCrossbow_Unique1", "Half musical instrument and fully deadly weapon, the Harp Crossbow is the life of the party. ")).
	characteristic({ LOCTEXT("even_more_projectiles", "Even More Projectiles") ,EItemRarity::Unique }));

const ItemType& ScatterCrossbow_Unique2 = rangedUnique(ScatterCrossbow, ItemTypeBuilder(TEXT("ScatterCrossbow_Unique2"),
	LOCTEXT("ScatterCrossbow_Unique2", "Lightning Harp Crossbow"),
	"RangedWeapons/ScatterCrossbow_Unique2").
	//characteristic(LOCTEXT("fires_lightning_bolts", "Fires Lightning Bolts")).
	flavour(LOCTEXT("Flavour_ScatterCrossbow_Unique2", "Adding electrical energy to this crossbow changed the firing sound dramatically.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Ricochet, 1}, EItemRarity::Unique } //Thundering
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	characteristic(LOCTEXT("multiple_projectiles", "Multiple projectiles")));

const ItemType& BurstCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("BurstCrossbow"),
	LOCTEXT("BurstCrossbow", "Burst Crossbow"),
	"RangedWeapons/BurstCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::trialsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	vendorBlocked().
	archetypes({ EItemArchetype::Archer, EItemArchetype::Special }).
	flavour(LOCTEXT("Flavour_BurstCrossbow", "A tactical crossbow favored by warriors and hunters alike, the Burst Crossbow is a powerful tool for any hero.")).
	characteristic(LOCTEXT("fast_multiple_projectiles", "Fast Multiple Projectiles")));

const ItemType& BurstCrossbow_Unique1 = rangedUnique(BurstCrossbow, ItemTypeBuilder(TEXT("BurstCrossbow_Unique1"),
	LOCTEXT("BurstCrossbow_Unique1", "Soul Hunter Crossbow"),
	"RangedWeapons/BurstCrossbow_Unique1").
	flavour(LOCTEXT("Flavour_BurstCrossbow_Unique1", "This crossbow, crafted in a forge rich with souls, thrives when an abundance of souls are nearby. ")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::EnigmaResonatorRanged, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Special, EItemArchetype::Soul }).
	characteristic(LOCTEXT("fast_multiple_projectiles", "Fast Multiple Projectiles")));

const ItemType& BurstCrossbow_Unique2 = rangedUnique(BurstCrossbow, ItemTypeBuilder(TEXT("BurstCrossbow_Unique2"),
	LOCTEXT("BurstCrossbow_Unique2", "Corrupted Crossbow"),
	"RangedWeapons/BurstCrossbow_Unique2").
	flavour(LOCTEXT("Flavour_BurstCrossbow_Unique2", "This crossbow has a subtle but corrupting power that is suitable for thieves and nimble warriors alike.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::DynamoRanged, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Special, EItemArchetype::Acrobat }).
	characteristic(LOCTEXT("fast_multiple_projectiles", "Fast Multiple Projectiles")));	
	
const ItemType& DualCrossbows = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("DualCrossbows"),
	LOCTEXT("DualCrossbows", "Dual Crossbows"),
	"RangedWeapons/DualCrossbows").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_3))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_DualCrossbows", "Dual Crossbows are the perfect choice for a warrior with quick reflexes in a fast-paced battle.")).
	characteristic(LOCTEXT("double_projectiles", "Double Projectiles")));

const ItemType& DualCrossbows_Unique1 = rangedUnique(DualCrossbows, ItemTypeBuilder(TEXT("DualCrossbows_Unique1"),
	LOCTEXT("DualCrossbows_Unique1", "Spellbound Crossbows"),
	"RangedWeapons/DualCrossbows_Unique1").
	flavour(LOCTEXT("Flavour_DualCrossbows_Unique1", "Enchanted enemies are especially vulnerable to the magic of these crossbows.")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Unchanting, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Mage}).
	characteristic(LOCTEXT("double_projectiles", "Double Projectiles")));

const ItemType& DualCrossbows_Unique2 = rangedUnique(DualCrossbows, ItemTypeBuilder(TEXT("DualCrossbows_Unique2"),
	LOCTEXT("DualCrossbows_Unique2", "Baby Crossbows"),
	"RangedWeapons/DualCrossbows_Unique2").
	flavour(LOCTEXT("Flavour_DualCrossbows_Unique2", "While some dismiss the cute Baby Crossbows, these deadly weapons grow into heavy hitters. ")).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Growing, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Archer).
	characteristic(LOCTEXT("double_projectiles", "Double Projectiles")));

const ItemType& SoulCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("SoulCrossbow"),
	LOCTEXT("SoulCrossbow", "Soul Crossbow"),
	"RangedWeapons/SoulCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulCrossbow", "The Soul Crossbow was crafted by the mysterious Evokers and Enchanters of the Woodland Mansions. ")).
	soulGatherItem(1));

const ItemType& SoulCrossbow_Unique1 = rangedUnique(SoulCrossbow, ItemTypeBuilder(TEXT("SoulCrossbow_Unique1"),
	LOCTEXT("SoulCrossbow_Unique1", "Feral Soul Crossbow"),
	"RangedWeapons/SoulCrossbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EnigmaResonatorRanged, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulCrossbow_Unique1", "If you listen closely you can hear the souls inside the crossbow, usually ridiculing you.")).
	soulGatherItem(1));

const ItemType& SoulCrossbow_Unique2 = rangedUnique(SoulCrossbow, ItemTypeBuilder(TEXT("SoulCrossbow_Unique2"),
	LOCTEXT("SoulCrossbow_Unique2", "Voidcaller"),
	"RangedWeapons/SoulCrossbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Gravity, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_SoulCrossbow_Unique2", "This weapon calls out to souls that are trapped between this world and the next.")).
	soulGatherItem(1));

const ItemType& ExplodingCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("ExplodingCrossbow"),
	LOCTEXT("ExplodingCrossbow", "Exploding Crossbow"),
	"RangedWeapons/ExplodingCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_ExplodingCrossbow", "The power of TNT fused with the latest in archery design resulted in this devastating crossbow.")).
	characteristic(LOCTEXT("explodes_on_impact", "Explodes on Impact")));

const ItemType& ExplodingCrossbow_Unique1 = rangedUnique(ExplodingCrossbow, ItemTypeBuilder(TEXT("ExplodingCrossbow_Unique1"),
	LOCTEXT("ExplodingCrossbow_Unique1", "Imploding Crossbow"),
	"RangedWeapons/ExplodingCrossbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Gravity, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_ExplodingCrossbow_Unique1", "The Imploding Crossbow has been magically fine-tuned to maximize the impact of the explosion.")).
	characteristic(LOCTEXT("explodes_on_impact", "Explodes on Impact")));

const ItemType& ExplodingCrossbow_Unique2 = rangedUnique(ExplodingCrossbow, ItemTypeBuilder(TEXT("ExplodingCrossbow_Unique2"),
	LOCTEXT("ExplodingCrossbow_Unique2", "Firebolt Thrower"),
	"RangedWeapons/ExplodingCrossbow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::ChainReaction, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_ExplodingCrossbow_Unique2", "The Firebolt Thrower launches chaos in a chain reaction of exploding arrows.")).
	characteristic(LOCTEXT("explodes_on_impact", "Explodes on Impact")));

const ItemType& ShadowCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("ShadowCrossbow"),
	LOCTEXT("ShadowCrossbow", "Shadow Crossbow"),
	"RangedWeapons/ShadowCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
			})))).
	permanentEnchantments({
	{{ EEnchantmentTypeID::ShadowShot, 1}, EItemRarity::Common }//jryden <- should be reviewed
		}).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_ShadowCrossbow", "Those who wish to use the Shadow Crossbow must train in total darkness before wielding it."))
	);

const ItemType& ShadowCrossbow_Unique1 = rangedUnique(ShadowCrossbow, ItemTypeBuilder(TEXT("ShadowCrossbow_Unique1"),
	LOCTEXT("ShadowCrossbow_Unique1", "Veiled Crossbow"),
	"RangedWeapons/ShadowCrossbow_Unique1").
	permanentEnchantments({
	{{EEnchantmentTypeID::ShadowShot, 1}, EItemRarity::Common },
	{{EEnchantmentTypeID::ShadowBarbRanged, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_ShadowCrossbow_Unique1", "The Veiled Crossbow cloaks the wielder in shadow, perfect for those who prefer to go unnoticed (and undefeated)."))
	);

const ItemType& SlowBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("SlowBow"),
	LOCTEXT("SlowBow", "Snow Bow"),
	"RangedWeapons/SlowBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	permanentEnchantments({
		{ { EEnchantmentTypeID::SlowBowEnchantment, 1}, EItemRarity::Common }
	}).	
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_SlowBow", "Those who face the Snow Bow in battle must also prepare to face the chill of freezing wintry winds.")));

const ItemType& SlowBow_Unique1 = rangedUnique(SlowBow, ItemTypeBuilder(TEXT("SlowBow_Unique1"),
	LOCTEXT("SlowBow_Unique1", "Winter's Touch"),
	"RangedWeapons/SlowBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::SlowBowEnchantment, 1}, EItemRarity::Unique }
	}).	
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_SlowBow_Unique1", "Arrows fired from this legendary bow are said to be carried by the winter winds themselves.")).
	characteristic({ LOCTEXT("chared_fires_three_arrows", "Fires 3 arrows when charged"), EItemRarity::Unique }));


const ItemType& SlowBow_Spooky2 = rangedUniqueLimited(SlowBow, ItemTypeBuilder(TEXT("SlowBow_Spooky2"),
	LOCTEXT("SlowBow_Spooky2", "Web Bow"),
	"RangedWeapons/SlowBow_Spooky2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::SlowBowEnchantment, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_SlowBow_Spooky2", "Crafted from the webs of mighty spiders, the Webbed Bow will get you out of any sticky situation.")).
	characteristic({ LOCTEXT("chared_fires_three_arrows", "Fires 3 arrows when charged"), EItemRarity::Unique }));


const ItemType& WindBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("WindBow"),
	LOCTEXT("WindBow", "Wind Bow"),
	"RangedWeapons/WindBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	permanentEnchantments({
		{ { EEnchantmentTypeID::WindBowEnchantment, 1}, EItemRarity::Common }
	}).
	blockedEnchantments({
		{ EEnchantmentTypeID::Gravity, EEnchantmentTypeID::Punch }
	}).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_WindBow", "A mesmerizing bow that captures the power of the wind to fire mighty Gale Arrows.")));

const ItemType& WindBow_Unique1 = rangedUnique(WindBow, ItemTypeBuilder(TEXT("WindBow_Unique1"),
	LOCTEXT("WindBow_Unique1", "Echo of the Valley"),
	"RangedWeapons/WindBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::WindBowEnchantment, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::Ricochet, 1}, EItemRarity::Unique }
	}).
	blockedEnchantments({
		{ EEnchantmentTypeID::Gravity, EEnchantmentTypeID::Punch }
	}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_WindBow_Unique1", "This bow calls upon the twisting winds of the hidden valley where it was first strung.")));

const ItemType& WindBow_Unique2 = rangedUnique(WindBow, ItemTypeBuilder(TEXT("WindBow_Unique2"),
	LOCTEXT("WindBow_Unique2", "Burst Gale Bow"),
	"RangedWeapons/WindBow_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::WindBowEnchantment, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::RollCharge, 1}, EItemRarity::Unique }
		}).
	blockedEnchantments({
		{ EEnchantmentTypeID::Gravity, EEnchantmentTypeID::Punch }
		}).
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_WindBow_Unique2", "A bow infused with the force of a rolling wind which can flare up in an instant. "))
		);

const ItemType& BubbleBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("BubbleBow"),
	LOCTEXT("BubbleBow", "Bubble Bow"),
	"RangedWeapons/BubbleBow").
	characteristic(LOCTEXT("bubbled_when_charged", "Bubble damage")).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	blockedEnchantments({ {EEnchantmentTypeID::FuseShot} }).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_BubbleBow", "Let your troubles float away with the Bubble Bow, which seals your target in a bubble on a charged shot.")));

const ItemType& BubbleBow_Unique1 = rangedUnique(BubbleBow, ItemTypeBuilder(TEXT("BubbleBow_Unique1"),
	LOCTEXT("BubbleBow_Unique1", "Bubble Burster"),
	"RangedWeapons/BubbleBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::ReliableRicochet, 1}, EItemRarity::Unique }
		}).
	blockedEnchantments({ {EEnchantmentTypeID::FuseShot} }).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_BubbleBow_Unique1", "The Bubble Burster creates a chain effect of bubble trouble for your enemies.")).
	characteristic(LOCTEXT("bubbled_when_charged", "Bubble damage")));

const ItemType& HarpoonCrossbow = ranged(RangedWeaponType::Crossbow, ItemTypeBuilder(TEXT("HarpoonCrossbow"),
	LOCTEXT("HarpoonCrossbow", "Harpoon Crossbow"),
	"RangedWeapons/HarpoonCrossbow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Desc_HarpoonCrossbow", "The Harpoon Crossbow shoots harpoons that cut through wind and water with devastating power.")).
	characteristic(LOCTEXT("fires_harpoons", "Fires Harpoons")));

const ItemType& HarpoonCrossbow_Unique1 = rangedUnique(HarpoonCrossbow, ItemTypeBuilder(TEXT("HarpoonCrossbow_Unique1"),
	LOCTEXT("HarpoonCrossbow_Unique1", "Nautical Crossbow"),
	"RangedWeapons/HarpoonCrossbow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Piercing, 3}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_HarpoonCrossbow_Unique1", "Why harpoon one enemy when you can harpoon many!")).
	characteristic(LOCTEXT("fires_harpoons", "Fires Harpoons")));

const ItemType& VoidBow = ranged(RangedWeaponType::Bow, ItemTypeBuilder(TEXT("VoidBow"),
	LOCTEXT("VoidBow", "Void Bow"),
	"RangedWeapons/VoidBow").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
			})))).
	permanentEnchantments({
		{{ EEnchantmentTypeID::VoidTouchedRanged, 1}, EItemRarity::Common }//jryden <- should be reviewed
		}).
	archetype(EItemArchetype::Archer). 
	flavour(LOCTEXT("Flavour_VoidBow", "When you pluck the string of the Void Bow, an unsettling silence reverberates across the battlefield.")));

const ItemType& VoidBow_Unique1 = rangedUnique(VoidBow, ItemTypeBuilder(TEXT("VoidBow_Unique1"),
	LOCTEXT("VoidBow_Unique1", "Call of the Void"),
	"RangedWeapons/VoidBow_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::VoidTouchedRanged, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::FuseShot, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_VoidBow_Unique1", "You can feel the Void whispering from deep within this bow, but behind that sound is another voice in the darkness.")));

/////////////////////////////////////////////////
// Armor
/////////////////////////////////////////////////
const ItemType& CowardsArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::ItemCooldownDecrease,
		EArmorPropertyID::IncreasedArrowBundleSize
	}, 
	ItemTypeBuilder(TEXT("CowardsArmor"),
	LOCTEXT("CowardsArmor", "Guard's Armor"),	
	"Armor/CowardsArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_CowardsArmor", "Cheap armor made in bulk, the Guard's Armor is a common sight in the villages of the Overworld."))
		);

const ItemType& CowardsArmor_Unique1 = armorUnique(CowardsArmor, ItemTypeBuilder(TEXT("CowardsArmor_Unique1"),
	LOCTEXT("CowardsArmor_Unique1", "Curious Armor"),
	"Armor/CowardsArmor_Unique1").
	workInProgress().
	addArmorProperties({
	{EArmorPropertyID::TeleportChance, EItemRarity::Unique},
	}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_CowardsArmor_Unique1", "No one knows where this strange armor came from but it seems familiar to you."))
	);

const ItemType& ReinforcedMail = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SuperbDamageAbsorption,
		EArmorPropertyID::MissChance,
		EArmorPropertyID::DodgeCooldownIncrease
	}, 
	ItemTypeBuilder(TEXT("ReinforcedMail"),
	LOCTEXT("ReinforcedMail", "Reinforced Mail"),	
	"Armor/ReinforcedMail").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_ReinforcedMail", "Mostly worn by casual adventurers, Reinforced Mail is a common sight throughout the land."))
		);

const ItemType& ReinforcedMail_Unique1 = armorUnique(ReinforcedMail,	ItemTypeBuilder(TEXT("ReinforcedMail_Unique1"),
	LOCTEXT("ReinforcedMail_Unique1", "Stalwart Armor"),
	"Armor/ReinforcedMail_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::PotionFortification, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_ReinforcedMail_Unique1", "This reliable armor is sturdy enough to be passed down for generations."))
	);

const ItemType& BeenestArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::Beekeeper,
		EArmorPropertyID::AreaHeal
},
ItemTypeBuilder(TEXT("BeenestArmor"),
	LOCTEXT("BeenestArmor", "Beenest Armor"),
	"Armor/BeeNestArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::trialsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	vendorBlocked().
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Special }).
	flavour(LOCTEXT("Flavour_BeenestArmor", "There always seems to be a slight buzz around the Beenest Armor. Or maybe the buzz is coming from within?"))
	);

const ItemType& SproutArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::AreaHeal,
	EArmorPropertyID::DodgeRoot
},
ItemTypeBuilder(TEXT("SproutArmor"),
	LOCTEXT("SproutArmor", "Sprout Armor"),
	"Armor/SproutArmor").
		restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_2))
			})))).
		archetype(EItemArchetype::Acrobat).
		flavour(LOCTEXT("Flavour_SproutArmor", "The dark vines of the Sprout Armor continue to grow even in complete darkness."))
	);

const ItemType& SproutArmor_Unique1 = armorUnique(SproutArmor, ItemTypeBuilder(TEXT("SproutArmor_Unique1"),
	LOCTEXT("SproutArmor_Unique1", "Living Vines Armor"),
	"Armor/SproutArmor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Acrobat, 1}, EItemRarity::Unique }
		}).
	archetype(EItemArchetype::Acrobat).
	flavour(LOCTEXT("Flavour_SproutArmor_Unique1", "This armor is made from the living vines of a plant which grows only on battlefields."))
);

const ItemType& PiglinArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::ItemCooldownReset,
		EArmorPropertyID::ItemDamageBoost
},
ItemTypeBuilder(TEXT("PiglinArmor"),
	LOCTEXT("PiglinArmor", "Piglin Armor"),
	"Armor/PiglinArmor").
		restrictTo(RETLAMBDA2((mergeRestrictions({
			levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_2))
			})))).
		archetype(EItemArchetype::Mage).
		flavour(LOCTEXT("Flavour_PiglinArmor", "Wearing this Piglin Armor is almost enough to make one feel at home in the Nether. Almost."))
		);

const ItemType& PiglinArmor_Unique1 = armorUnique(PiglinArmor, ItemTypeBuilder(TEXT("PiglinArmor_Unique1"),
	LOCTEXT("PiglinArmor_Unique1", "Golden Piglin Armor"),
	"Armor/PiglinArmor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::SurpriseGift, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_PiglinArmor_Unique1", "The Golden Piglin Armor combines a piglin's two favorite things: not dying and gold."))
);


const ItemType& BeenestArmor_Unique1 = armorUnique(BeenestArmor, ItemTypeBuilder(TEXT("BeenestArmor_Unique1"),
	LOCTEXT("BeenestArmor_Unique1", "Beehive Armor"),
	"Armor/BeenestArmor_Unique1").
	addArmorProperties({
		{EArmorPropertyID::SuperbDamageAbsorption, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Special, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_BeenestArmor_Unique1", "The Beehive Armor hums with the focused power of countless bees."))
);
const ItemType& ScaleMail = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SuperbDamageAbsorption,
		EArmorPropertyID::MeleeDamageBoost
	}, 
	ItemTypeBuilder(TEXT("ScaleMail"),
	LOCTEXT("ScaleMail", "Scale Mail"),	
	"Armor/ScaleMail").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))

	})))).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_ScaleMail", "This armor, crafted near the shores of a great sea, was inspired by the scales of fish."))
		);

const ItemType& ScaleMail_Unique1 = armorUnique(ScaleMail, ItemTypeBuilder(TEXT("ScaleMail_Unique1"),
	LOCTEXT("ScaleMail_Unique1", "Highland Armor"),
	"Armor/ScaleMail_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Swiftfooted, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Acrobat, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_ScaleMail_Unique1", "A wise armorer made this armor with care."))
	);

const ItemType& GhostArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::DodgeGhostForm,
		EArmorPropertyID::MissChance
	},
	ItemTypeBuilder(TEXT("GhostArmor"),
	LOCTEXT("GhostArmor", "Ghostly Armor"),
	"Armor/GhostArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::trialsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	vendorBlocked().
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Special }).
	flavour(LOCTEXT("Flavour_GhostArmor", "Those who wear Ghostly Armor may feel their bodies briefly disconnect from the physical world, but are quickly snapped back to reality."))
	);

const ItemType& GhostArmor_Unique1 = armorUnique(GhostArmor, ItemTypeBuilder(TEXT("GhostArmor_Unique1"),
	LOCTEXT("GhostArmor_Unique1", "Ghost Kindler"),
	"Armor/GhostArmor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::FireTrail, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Special, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_GhostArmor_Unique1", "Strange flames follow the shifting form of those who wear the Ghost Kindler armor."))
);


const ItemType& EvocationRobe = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::ItemCooldownDecrease,
		EArmorPropertyID::MoveSpeedAura
	}, 
	ItemTypeBuilder(TEXT("EvocationRobe"),
	LOCTEXT("EvocationRobe", "Evocation Robe"),	
	"Armor/EvocationRobe").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_3)),
	})))).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_EvocationRobe", "Potent magical runes are weaved into the fabric of these robes, their origins and true powers are shrouded in mystery."))
		);

const ItemType& EvocationRobe_Unique1 = armorUnique(EvocationRobe, ItemTypeBuilder(TEXT("EvocationRobe_Unique1"),
	LOCTEXT("EvocationRobe_Unique1", "Ember Robe"),
	"Armor/EvocationRobe_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Burning, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_EvocationRobe_Unique1", "The Ember Robe was created by Illager Evokers to distinguish themselves from the common guard."))
	);

const ItemType& EvocationRobe_Unique2 = armorUnique(EvocationRobe, ItemTypeBuilder(TEXT("EvocationRobe_Unique2"),
	LOCTEXT("EvocationRobe_Unique2", "Verdant Robe"),
	"Armor/EvocationRobe_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::BagOfSouls, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Soul, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_EvocationRobe_Unique2", "Those who don the Verdant Robe soon find that they can commune with more souls than ever before."))
);

const ItemType& ArchersStrappings = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::IncreasedArrowBundleSize,
		EArmorPropertyID::RangedDamageBoost
	}, 
	ItemTypeBuilder(TEXT("ArchersStrappings"),
	LOCTEXT("ArchersStrappings", "Hunter's Armor"),	
	"Armor/ArchersStrappings").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Archer).
	flavour(LOCTEXT("Flavour_ArchersStrappings", "Hunter's Armor may not be the fanciest gear around, but it will stop a blade."))
		);

const ItemType& ArchersStrappings_Unique1 = armorUnique(ArchersStrappings, ItemTypeBuilder(TEXT("ArchersStrappings_Unique1"),
	LOCTEXT("ArchersStrappings_Unique1", "Archer's Armor"),
	"Armor/ArchersStrappings_Unique1").
	addArmorProperties({
	{EArmorPropertyID::MoveSpeedAura, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Archer, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_ArchersStrappings_Unique1", "Archer's Armor, favored by humble warriors, is cheap to make and still stops a blade."))
	);

const ItemType& AssassinArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::MeleeAttackSpeedBoost
	}, 
	ItemTypeBuilder(TEXT("AssassinArmor"),
	LOCTEXT("AssassinArmor", "Thief Armor"),	
	"Armor/AssassinArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Fighter).
	flavour(LOCTEXT("Flavour_AssassinArmor", "This armor is light, nimble, and smells faintly of sulfur."))
		);

const ItemType& AssassinArmor_Unique1 = armorUnique(AssassinArmor, ItemTypeBuilder(TEXT("AssassinArmor_Unique1"),
	LOCTEXT("AssassinArmor_Unique1", "Spider Armor"),	
	"Armor/AssassinArmor_Unique1").
	addArmorProperties({
	{EArmorPropertyID::LifeStealAura, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_AssassinArmor_Unique1", "Spider Armor, created by master thieves, is inspired by the agile talents of the Spider."))
	);

const ItemType& BattleRobe = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::ItemCooldownDecrease,
		EArmorPropertyID::MeleeDamageBoost
	}, 
	ItemTypeBuilder(TEXT("BattleRobe"),
	LOCTEXT("BattleRobe", "Battle Robe"),	
	"Armor/BattleRobe").
	restrictTo(RETLAMBDA2((mergeRestrictions({		
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage }).
	flavour(LOCTEXT("Flavour_BattleRobe", "The Battle Robe is worn by the distinguished Illager Evokers of the Arch-Illager's court."))
		);

const ItemType& BattleRobe_Unique1 = armorUnique(BattleRobe, ItemTypeBuilder(TEXT("BattleRobe_Unique1"),
	LOCTEXT("BattleRobe_Unique1", "Splendid Robe"),	
	"Armor/BattleRobe_Unique1").
	addArmorProperties({
	{EArmorPropertyID::ItemDamageBoost, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Mage, EItemArchetype::Archer }).
	flavour(LOCTEXT("Flavour_BattleRobe_Unique1", "The distinguished Splendid Robe is worn by the hardened Illager warriors who protect the Arch-Illager."))
	);

const ItemType& DarkArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SoulGatheringBoost,
		EArmorPropertyID::SuperbDamageAbsorption
	}, 
	ItemTypeBuilder(TEXT("DarkArmor"),
	LOCTEXT("DarkArmor", "Dark Armor"),	
	"Armor/DarkArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_DarkArmor", "Dark Armor, made in the blackest depths of the Fiery Forge, is worn by the Illager royal guard."))
		);

const ItemType& DarkArmor_Unique1 = armorUnique(DarkArmor, ItemTypeBuilder(TEXT("DarkArmor_Unique1"),
	LOCTEXT("DarkArmor_Unique1", "Titan's Shroud"),	
	"Armor/DarkArmor_Unique1").
	addArmorProperties({		
		{EArmorPropertyID::AllyDamageBoost, EItemRarity::Unique},
	}).	
	archetypes({ EItemArchetype::Soul, EItemArchetype::Tank, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_DarkArmor_Unique1", "The inspiring tales of the Titan's Shroud armor have passed through Pumpkin Pastures for generations."))
	);

const ItemType& WolfArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::AllyDamageBoost,
		EArmorPropertyID::AreaHeal
	}, 
	ItemTypeBuilder(TEXT("WolfArmor"),
	LOCTEXT("WolfArmor", "Wolf Armor"),	
	"Armor/WolfArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_WolfArmor", "Many warriors wear the heads of wolves into battle to strike fear into the hearts of their enemies."))
		);

const ItemType& WolfArmor_Unique1 = armorUnique(WolfArmor, ItemTypeBuilder(TEXT("WolfArmor_Unique1"),
	LOCTEXT("WolfArmor_Unique1", "Fox Armor"),	
	"Armor/WolfArmor_Unique1").
	addArmorProperties({
	{EArmorPropertyID::MissChance, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_WolfArmor_Unique1", "Ancient Villager tribes created this armor to honor the fox, who is a great and agile warrior."))
	);

const ItemType& WolfArmor_Unique2 = armorUnique(WolfArmor, ItemTypeBuilder(TEXT("WolfArmor_Unique2"),
	LOCTEXT("WolfArmor_Unique2", "Black Wolf Armor"),
	"Armor/WolfArmor_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Acrobat, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Support, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_WolfArmor_Unique2", "The wolf who strikes from the shadows lives to tell the tale."))
);

const ItemType& WolfArmor_Winter1 = armorUniqueLimited(WolfArmor, ItemTypeBuilder(TEXT("WolfArmor_Winter1"),
	LOCTEXT("WolfArmor_Winter1", "Arctic Fox Armor"),
	"Armor/WolfArmor_Winter1").
	addArmorProperties({
		{EArmorPropertyID::MissChance, EItemRarity::Unique},
	}).
	eventType(EItemEventType::Winter).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_WolfArmor_Winter1", "If you want to survive the icy tundra in style, the Arctic Fox Armor is the best choice this season."))
);

const ItemType& ChampionsArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SuperbDamageAbsorption,
		EArmorPropertyID::IncreasedMobTargeting,
		EArmorPropertyID::PotionCooldownDecrease
	}, 
	ItemTypeBuilder(TEXT("ChampionsArmor"),
	LOCTEXT("ChampionsArmor ", "Champion's Armor"),	
	"Armor/ChampionsArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_ChampionsArmor", "Granted to the champions of the Overworld by the free villagers as a token of their thanks for many years of protection and assistance. The mark of a true hero.")));

const ItemType& ChampionsArmor_Unique1 = armorUnique(ChampionsArmor, ItemTypeBuilder(TEXT("ChampionsArmor_Unique1"),
	LOCTEXT("ChampionsArmor_Unique1 ", "Hero's Armor"),	
	"Armor/ChampionsArmor_Unique1").
	addArmorProperties({
		{EArmorPropertyID::AreaHeal, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_ChampionsArmor_Unique1", "Only given to the highest caliber of heroes, the Hero's Armor is a precious gift from the Villagers."))
	);

const ItemType& MercenaryArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SuperbDamageAbsorption,
		EArmorPropertyID::AllyDamageBoost
	}, 
	ItemTypeBuilder(TEXT("MercenaryArmor"),
	LOCTEXT("MercenaryArmor", "Mercenary Armor"),	
	"Armor/MercenaryArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::trialsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_MercenaryArmor", "Mercenary Armor, favored by those looking to save some coin, lacks flair but gets the job done."))
		);

const ItemType& MercenaryArmor_Unique1 = armorUnique(MercenaryArmor, ItemTypeBuilder(TEXT("MercenaryArmor_Unique1"),
	LOCTEXT("MercenaryArmor_Unique1", "Renegade Armor"),	
	"Armor/MercenaryArmor_Unique1").
	addArmorProperties({
	{EArmorPropertyID::MeleeAttackSpeedBoost, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_MercenaryArmor_Unique1", "Renegade Armor made a fine payment for mercenaries hired to protect the villages against Illagers."))
	);

const ItemType& MercenaryArmor_Spooky1 = armorUniqueLimited(MercenaryArmor, ItemTypeBuilder(TEXT("MercenaryArmor_Spooky1"),
	LOCTEXT("MercenaryArmor_Spooky1", "Hungry Horror"),
	"Armor/MercenaryArmor_Spooky1").
	addArmorProperties({
	{EArmorPropertyID::MeleeAttackSpeedBoost, EItemRarity::Unique},
		}).
	eventType(EItemEventType::Spooky).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_MercenaryArmor_Spooky1", "When you wear this cloak, the sounds of screams fill the air. But are the screams yours, or your enemies'?"))
);

const ItemType& GrimArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SoulGatheringBoost,
		EArmorPropertyID::LifeStealAura
	}, 
	ItemTypeBuilder(TEXT("GrimArmor"),
	LOCTEXT("GrimArmor", "Grim Armor"),	
	"Armor/GrimArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::deserttemple,     FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple,      FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1))
		
	})))).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_GrimArmor", "Grim Armor invokes a sense of dread for the one who wears it and to those who face it in battle."))
		);

const ItemType& GrimArmor_Unique1 = armorUnique(GrimArmor, ItemTypeBuilder(TEXT("GrimArmor_Unique1"),
	LOCTEXT("GrimArmor_Unique1", "Wither Armor"),	
	"Armor/GrimArmor_Unique1").
	addArmorProperties({
		{EArmorPropertyID::SuperbDamageAbsorption, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_GrimArmor_Unique1", "Wither Armor, crafted with the parts of slain enemies, was made to terrify the wearer's enemies."))
	);

const ItemType& GrimArmor_Spooky2 = armorUniqueLimited(GrimArmor, ItemTypeBuilder(TEXT("GrimArmor_Spooky2"),
	LOCTEXT("GrimArmor_Spooky2", "The Gourdian Armor"),
	"Armor/GrimArmor_Spooky2").
	addArmorProperties({
		{EArmorPropertyID::SuperbDamageAbsorption, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Soul, EItemArchetype::Support, EItemArchetype::Tank }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_GrimArmor_Spooky2", "Those who wear the mantle of The Spooky Gourdian become the legendary terror of Pumpkin Pastures!"))
);

const ItemType& PhantomArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SoulGatheringBoost,
		EArmorPropertyID::RangedDamageBoost
	}, 
	ItemTypeBuilder(TEXT("PhantomArmor"),
	LOCTEXT("PhantomArmor", "Phantom Armor"),	
	"Armor/PhantomArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_3))
	})))).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_PhantomArmor", "This armor, crafted from the bones of Phantoms, is a terrifying sight on the battlefield.")));

const ItemType& PhantomArmor_Unique1 = armorUnique(PhantomArmor, ItemTypeBuilder(TEXT("PhantomArmor_Unique1"),
	LOCTEXT("PhantomArmor_Unique1", "Frost Bite"),
	"Armor/PhantomArmor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::Snowing, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Soul, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_PhantomArmor_Unique1", "Frost Bite remembers the icy winds which once flowed beneath the wings of mighty Phantoms."))
	);


const ItemType& FullPlateArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::MissChance,
		EArmorPropertyID::SuperbDamageAbsorption,
		EArmorPropertyID::DodgeCooldownIncrease
	},
	ItemTypeBuilder(TEXT("FullPlateArmor"),
	LOCTEXT("FullPlateArmor", "Plate Armor"),
	"Armor/FullPlateArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_FullPlateArmor", "Plate armor turns the average soldier into a fortress but comes with reduced mobility.")));

const ItemType& FullPlateArmor_Unique1 = armorUnique(FullPlateArmor, ItemTypeBuilder(TEXT("FullPlateArmor_Unique1"),
	LOCTEXT("FullPlateArmor_Unique1", "Full Metal Armor"),
	"Armor/FullPlateArmor_Unique1").
	addArmorProperties({
	{EArmorPropertyID::MeleeDamageBoost, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_FullPlateArmor_Unique1", "Full Metal Armor is destined for the great defenders of the Overworld."))
);

const ItemType& FullPlateArmor_Spooky2 = armorUniqueLimited(FullPlateArmor, ItemTypeBuilder(TEXT("FullPlateArmor_Spooky2"),
	LOCTEXT("FullPlateArmor_Spooky2", "Cauldron Armor"),
	"Armor/FullPlateArmor_Spooky2").
	addArmorProperties({
	{EArmorPropertyID::MeleeDamageBoost, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Fighter, EItemArchetype::Tank }).
	eventType(EItemEventType::Spooky2).
	flavour(LOCTEXT("Flavour_FullPlateArmor_Spooky2", "The Cauldron Armor was created with camouflage in mind, but what it lacks in stealth it makes up for in bubbly charm."))
);

const ItemType& SpelunkersArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::AllyDamageBoost,
		EArmorPropertyID::PetBat
	}, 
	ItemTypeBuilder(TEXT("SpelunkersArmor"),
	LOCTEXT("SpelunkersArmor", "Spelunker Armor"),	
	"Armor/SpelunkersArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Summoner).
	flavour(LOCTEXT("Flavour_SpelunkersArmor", "The Spelunker Armor is worn by those who brave the darkest depths of the Overworld."))
		);

const ItemType& SpelunkersArmor_Unique1 = armorUnique(SpelunkersArmor, ItemTypeBuilder(TEXT("SpelunkersArmor_Unique1"),
	LOCTEXT("SpelunkersArmor_Unique1", "Cave Crawler"),	
	"Armor/SpelunkersArmor_Unique1").
	addArmorProperties({
	{EArmorPropertyID::ItemDamageBoost, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Fighter, EItemArchetype::Summoner }).
	flavour(LOCTEXT("Flavour_SpelunkersArmor_Unique1", "The Cave Crawler armor has been passed down through generations of brave spelunkers and miners. "))
	);

const ItemType& SpelunkersArmor_Year1 = armorUnique(SpelunkersArmor, ItemTypeBuilder(TEXT("SpelunkersArmor_Year1"),
	LOCTEXT("SpelunkersArmor_Year1", "Sweet Tooth"),
	"Armor/SpelunkersArmor_Year1").
	addArmorProperties({
	{EArmorPropertyID::ItemDamageBoost, EItemRarity::Unique},
		}).
	archetypes({ EItemArchetype::Fighter, EItemArchetype::Summoner }).
	eventType(EItemEventType::Year1).
	flavour(LOCTEXT("Flavour_SpelunkersArmor_Year1", "This armor may look like a piece of cake to your enemies, but it is surprisingly durable in battle. Must be the fondant."))
);

const ItemType& SoulRobe = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::SoulGatheringBoost,
		EArmorPropertyID::ItemDamageBoost
	}, 
	ItemTypeBuilder(TEXT("SoulRobe"),
	LOCTEXT("SoulRobe", "Soul Robe"),	
	"Armor/SoulRobe").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))).
	archetype(EItemArchetype::Soul).
	flavour(LOCTEXT("Flavour_SoulRobe", "In a particular light, the souls woven into the cloth of the Soul Robe shimmer with power."))
		);

const ItemType& SoulRobe_Unique1 = armorUnique(SoulRobe, ItemTypeBuilder(TEXT("SoulRobe_Unique1"),
	LOCTEXT("SoulRobe_Unique1", "Souldancer Robe"),	
	"Armor/SoulRobe_Unique1").
	addArmorProperties({
	{EArmorPropertyID::MissChance, EItemRarity::Unique},
		}).
		archetypes({ EItemArchetype::Soul, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_SoulRobe_Unique1", "The Souldancer Robe comes alive in the light, as if the souls within are dancing for all eternity.")));

const ItemType& MysteryArmor = armor(TArray<EArmorPropertyID> {}, 
	ItemTypeBuilder(TEXT("MysteryArmor"),
	LOCTEXT("MysteryArmor", "Mystery Armor"),	
	"Armor/MysteryArmor").
	flavour(LOCTEXT("Flavour_MysteryArmor", "Some blacksmiths see the design influence of Iron Golems in this property-shifting armor.")).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::trialsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_3)),
	})))).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Special }).
	applyArmorPropertyColor().
	generateArmorProperties(2).
	matchEnchantmentColorWithArmorPropertyColor());

const ItemType& MysteryArmor_Unique1 = armorUnique(MysteryArmor, ItemTypeBuilder(TEXT("MysteryArmor_Unique1"),
	LOCTEXT("MysteryArmor_Unique1", "Mystery Armor"),	
	"Armor/MysteryArmor_Unique1").
	archetypes({ EItemArchetype::Mage, EItemArchetype::Special }).
	flavour(LOCTEXT("Flavour_MysteryArmor_Unique1", "A seriously mysterious piece of gear, the true origins of this property-shifting armor are unknown. Some blacksmiths see a resemblance in its design to the Iron Golems.")).
	applyArmorPropertyColor().
	generateArmorProperties(3).
	workInProgress().
	matchEnchantmentColorWithArmorPropertyColor()
	);

const ItemType& OcelotArmor = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::DodgeSpeedIncrease,
		EArmorPropertyID::SuperbDamageAbsorption
	}, 
	ItemTypeBuilder(TEXT("OcelotArmor"),
	LOCTEXT("OcelotArmor", "Ocelot Armor"),	
	"Armor/OcelotArmor").
	archetype(EItemArchetype::Acrobat).
	flavour(LOCTEXT("Flavour_OcelotArmor", "You feel a rush of pure adrenaline surge through your body when you wear this Ocelot's pelt.")).	
	restrictTo(RETLAMBDA2((mergeRestrictions({
			levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1))
	}))))
);

const ItemType& OcelotArmor_Unique1 = armorUnique(OcelotArmor, ItemTypeBuilder(TEXT("OcelotArmor_Unique1"),
	LOCTEXT("OcelotArmor_Unique1", "Shadow Walker"),	
	"Armor/OcelotArmor_Unique1").
	addArmorProperties({
		{EArmorPropertyID::DodgeInvulnerability, EItemRarity::Unique},
	}).	
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_OcelotArmor_Unique1", "The legendary black Ocelot was as graceful as it was deadly. When you wear its pelt, you feel like your enemies are left chasing your shadow."))
);

const ItemType& SnowArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::SuperbDamageAbsorption,
	EArmorPropertyID::SlowResistance
	},
	ItemTypeBuilder(TEXT("SnowArmor"),
	LOCTEXT("SnowArmor", "Snow Armor"),"Armor/SnowArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).	
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_SnowArmor", "A suit of armor that was tempered in snowmelt, protecting the wearer from the harsh cold of the tundra."))
);

const ItemType& SnowArmor_Unique1 = armorUnique(SnowArmor, ItemTypeBuilder(TEXT("SnowArmor_Unique1"),
	LOCTEXT("SnowArmor_Unique1", "Frost Armor"), "Armor/SnowArmor_Unique1").
	permanentEnchantments({
		{ {EEnchantmentTypeID::Chilling, 2}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_SnowArmor_Unique1", "This legendary armor, forged from ice that never melts, makes the wearer feel as if they are one with winter."))
);

const ItemType& ClimbingGear = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::ItemCooldownDecrease,
	EArmorPropertyID::Heavyweight
},
ItemTypeBuilder(TEXT("ClimbingGear"),
	LOCTEXT("ClimbingGear", "Climbing Gear"),
	"Armor/ClimbingGear").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_ClimbingGear", "This sturdy Climbing Gear is perfect for braving the blistering cold of the icy mountain face."))
);

const ItemType& ClimbingGear_Unique1 = armorUnique(ClimbingGear, ItemTypeBuilder(TEXT("ClimbingGear_Unique1"),
	LOCTEXT("ClimbingGear_Unique1", "Rugged Climbing Gear"),	"Armor/ClimbingGear_Unique1").
	addArmorProperties({
		{EArmorPropertyID::EnvironmentalProtection, EItemRarity::Unique},
		{EArmorPropertyID::SlowResistance, EItemRarity::Unique}
	}).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_ClimbingGearSolid", "Take on the worst that nature can throw at you in this rugged armor."))
	);

const ItemType& ClimbingGear_Unique2 = armorUnique(ClimbingGear, ItemTypeBuilder(TEXT("ClimbingGear_Unique2"),
	LOCTEXT("ClimbingGear_Unique2", "Goat Gear"), "Armor/ClimbingGear_Unique2").
	permanentEnchantments({
		{ {EEnchantmentTypeID::MultiDodge, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_ClimbingGear_Unique2", "Made from the pelts of Mountain Goats, you'll feel like skipping from peak to peak in this Goat Gear."))
);

const ItemType& EmeraldArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::MeleeAttackSpeedBoost,
},
ItemTypeBuilder(TEXT("EmeraldArmor"),
	LOCTEXT("EmeraldArmor", "Emerald Gear"), "Armor/EmeraldArmor").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EmeraldDivination, 1}, EItemRarity::Common }
		}).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_2)),
		})))).
	archetype(EItemArchetype::Mage).
	flavour(LOCTEXT("Flavour_EmeraldArmor", "As you wear the Emerald Gear during your adventures, it calls other emeralds to you as if by chance."))
);

const ItemType& EmeraldArmor_Unique1 = armorUnique(EmeraldArmor, ItemTypeBuilder(TEXT("EmeraldArmor_Unique1"),
	LOCTEXT("Emerald_Armor_Unique1", "Opulent Armor"), "Armor/EmeraldArmor_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EmeraldDivination, 1}, EItemRarity::Unique }
		}).
	addArmorProperties({
	{EArmorPropertyID::EmeraldShield, EItemRarity::Unique},
	}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_EmeraldArmor_Unique1", "Opulent Armor, originally designed more for show than for combat, thrives in the presence of Emeralds."))
);

const ItemType& EmeraldArmor_Unique2 = armorUnique(EmeraldArmor, ItemTypeBuilder(TEXT("EmeraldArmor_Unique2"),
	LOCTEXT("Emerald_Armor_Unique2", "Gilded Glory"), "Armor/EmeraldArmor_Unique2").
	permanentEnchantments({
		{ { EEnchantmentTypeID::EmeraldDivination, 1}, EItemRarity::Unique },
		{ { EEnchantmentTypeID::DeathBarter, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_EmeraldArmor_Unique2", "Even death itself has to pause and admire the charms of the legendary Gilded Glory armor."))
);

const ItemType& SquidArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::MoveSpeedAura,
	EArmorPropertyID::SquidRollLimited,
},
ItemTypeBuilder(TEXT("SquidArmor"),
	LOCTEXT("SquidArmor", "Squid Armor"), "Armor/SquidArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_2))
	})))).
	archetype(EItemArchetype::Tank).
	flavour(LOCTEXT("Flavour_SquidArmor", "It was easy to make this jet-black armor look cool. The hard part was securing the ink sacs."))
);

const ItemType& SquidArmor_Unique1 = armorUnique(SquidArmor, ItemTypeBuilder(TEXT("SquidArmor_Unique1"),
	LOCTEXT("SquidArmor_Unique1", "Glow Squid Armor"), "Armor/SquidArmor_Unique1").
	removeArmorProperties({
		{EArmorPropertyID::SquidRollLimited, EItemRarity::Common},
	}).
	addArmorProperties({
	{EArmorPropertyID::DodgeInvulnerability, EItemRarity::Unique},
	{EArmorPropertyID::SquidRollQuick, EItemRarity::Common},
	}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_SquidArmor_Unique1", "Those who hunt elusive glow squids wear this armor to blend in with their beautiful prey."))
);

const ItemType& TurtleArmor = armor(TArray<EArmorPropertyID> {
	EArmorPropertyID::SuperbDamageAbsorption,
	EArmorPropertyID::HealingAura
	},
	ItemTypeBuilder(TEXT("TurtleArmor"),
	LOCTEXT("TurtleArmor", "Turtle Armor"), "Armor/TurtleArmor").
		restrictTo(RETLAMBDA2((mergeRestrictions({
			levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1)),
			levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
				})))).
		archetype(EItemArchetype::Tank).
		flavour(LOCTEXT("Flavour_TurtleArmor", "The Turtle Armor is inspired by the hardy and protective shell of the humble turtle."))
);

const ItemType& TurtleArmor_Unique1 = armorUnique(TurtleArmor, ItemTypeBuilder(TEXT("TurtleArmor_Unique1"),
	LOCTEXT("TurtleArmor_Unique1", "Nimble Turtle Armor"), "Armor/TurtleArmor_Unique1").
	permanentEnchantments({
		{ {EEnchantmentTypeID::Flee, 1}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Acrobat }).
	flavour(LOCTEXT("Flavour_TurtleArmor_Unique1", "This armor is made from the dense (but surprisingly light) shells of turtles who lived at crushing depths."))
);

const ItemType& ShulkerArmor = armor(TArray<EArmorPropertyID> { EArmorPropertyID::IncreasedMobTargeting },
	ItemTypeBuilder(TEXT("ShulkerArmor"),
		LOCTEXT("ShulkerArmor", "Shulker Armor"), "Armor/ShulkerArmor").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
			})))).
	permanentEnchantments({
		{ { EEnchantmentTypeID::Deflecting, 1}, EItemRarity::Common },
		{ { EEnchantmentTypeID::ThriveUnderPressure, 1}, EItemRarity::Common }
		}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter }).
	flavour(LOCTEXT("Flavour_ShulkerArmor", "Shulker Armor rivals even the toughest steel plate."))
	);

const ItemType& ShulkerArmor_Unique1 = armorUnique(ShulkerArmor, ItemTypeBuilder(TEXT("ShulkerArmor_Unique1"),
	LOCTEXT("ShulkerArmor_Unique1", "Sturdy Shulker Armor"), "Armor/ShulkerArmor_Unique1").
	permanentEnchantments({
			{ { EEnchantmentTypeID::Deflecting, 1}, EItemRarity::Common },
			{ { EEnchantmentTypeID::ThriveUnderPressure, 1}, EItemRarity::Common },
			{ { EEnchantmentTypeID::ShulkerSentry, 1}, EItemRarity::Unique }
		}).
	archetypes({ EItemArchetype::Tank, EItemArchetype::Fighter, EItemArchetype::Support }).
	flavour(LOCTEXT("Flavour_ShulkerArmor_Unique1", "They say the best defense is a good offense, but good luck breaking through this bulwark."))
	);

const ItemType& BardsGarb = armor(TArray<EArmorPropertyID> {
		EArmorPropertyID::AreaHeal,
		EArmorPropertyID::Resonant
	},
	ItemTypeBuilder(TEXT("BardsGarb"),
	LOCTEXT("BardsGarb", "Entertainer's Garb"), "Armor/BardsGarb").
	restrictTo(RETLAMBDA2((mergeRestrictions(
		{ levels::allLevelsFor(ELevelNames::thestronghold, FDifficulty(EGameDifficulty::Difficulty_1)) }
	)))).
	permanentEnchantments({
			{ { EEnchantmentTypeID::BardIdle, 1}, EItemRarity::Common }
	}).
	archetype(EItemArchetype::Support).
	flavour(LOCTEXT("Flavour_BardsGarb", "Defensive? Sure. Fabulous? ABSOLUTELY."))
);

const ItemType& BardsGarb_Unique1 = armorUnique(BardsGarb, ItemTypeBuilder(TEXT("BardsGarb_Unique1"),
	LOCTEXT("BardsGarb_Unique1", "The Troubadour"), "Armor/BardsGarb_Unique1").
	addArmorProperties({
		{EArmorPropertyID::ImmunityBoost, EItemRarity::Unique},
		}).
	permanentEnchantments({
			{ { EEnchantmentTypeID::BardUnique1Idle, 1} }
		}).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	flavour(LOCTEXT("Flavour_BardsGarb_Unique1", "Why leave your adoring fans wanting more when you can wow them with this inspirational outfit?"))
);

const ItemType& Enderobes = armor(TArray<EArmorPropertyID> { 
		EArmorPropertyID::InstantTransmission,
		EArmorPropertyID::SoulGatheringBoost
	}, 
	ItemTypeBuilder(TEXT("EndRobes"),
	LOCTEXT("EndRobes", "Teleportation Robes"), "Armor/EndRobes").
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_EndRobes", "The Teleportation Robes were made by those who devoted their lives to studying the enigmas of the End."))
);

const ItemType& Enderobes_Unique1 = armorUnique(Enderobes, ItemTypeBuilder(TEXT("EndRobes_Unique1"),
	LOCTEXT("EndRobes_Unique1", "Unstable Robes"), "Armor/EndRobes_Unique1").
	permanentEnchantments({
		{ { EEnchantmentTypeID::VoidBlast, 1}, EItemRarity::Unique }
	}).
	archetypes({ EItemArchetype::Mage, EItemArchetype::Acrobat, EItemArchetype::Soul }).
	flavour(LOCTEXT("Flavour_EndRobes_Unique1", "This Enderobe sparks with static as you sashay across the battlefield."))
);

/////////////////////////////////////////////////
// Projectiles
/////////////////////////////////////////////////
const ItemType& Arrow = projectile(ItemTag::Arrow, ESlotType::Arrow, ItemTypeBuilder(TEXT("Arrow"), LOCTEXT("Arrow", "Arrow Bundle"), LOCTEXT("Desc_Arrow", "A small bundle of arrows"), "Arrow").
	storeCount(20));
const ItemType& BurningArrow = projectile(ItemTag::BurningArrow, ESlotType::BurningArrow, ItemTypeBuilder(TEXT("BurningArrow"), LOCTEXT("BurningArrow", "Burning Arrows"), LOCTEXT("Desc_BurningArrow", "Burning arrows have increased damage and set enemies on fire"), "BurningArrow").
	storeCount(5));
const ItemType& FireworksArrow = projectile(ItemTag::FireworksArrow, ESlotType::FireworksArrow, ItemTypeBuilder(TEXT("FireworksArrow"), LOCTEXT("FireworksArrow", "Fireworks Arrow"), LOCTEXT("Desc_FireworksArrow", "Firework arrows explode on impact, causing massive damage."), "FireworksArrow").
	cooldown(0.5f).
	storeCount(1));
const ItemType& TormentProjectile = projectile(ItemTag::TormentProjectile, ESlotType::TormentProjectile, ItemTypeBuilder(TEXT("TormentProjectile"), LOCTEXT("TormentProjectile ", "Torment Arrow"), LOCTEXT("Desc_TormentProjectile", "Travels slowly and pushes mobs back."), "TormentProjectile").
	storeCount(3));
const ItemType& HeavyHarpoon = projectile(ItemTag::HeavyHarpoon, ESlotType::HeavyHarpoon, ItemTypeBuilder(TEXT("HeavyHarpoon"), LOCTEXT("HeavyHarpoon", "Heavy Duty Harpoons"), LOCTEXT("Desc_HeavyHarpoon", "Heavy Duty Harpoons have increased damage and travel faster"), "HeavyHarpoon").
	storeCount(5));
const ItemType& ThunderingArrow = projectile(ItemTag::ThunderingArrow, ESlotType::ThunderingArrow, ItemTypeBuilder(TEXT("ThunderingArrow"), LOCTEXT("ThunderingArrow", "Thundering Arrow"), LOCTEXT("Desc_ThunderingArrow", "Thundering arrows trigger chain lightning on impact, jumping from mob to mob dealing damage."), "ThunderingArrow").
	storeCount(5));
const ItemType& VoidArrow = projectile(ItemTag::VoidArrow, ESlotType::VoidArrow, ItemTypeBuilder(TEXT("VoidArrow"), LOCTEXT("VoidArrow", "Void Arrows"), LOCTEXT("Desc_VoidArrow", "Mobs hit with these arrows take direct damage and are also afflicted with Void Touched."), "VoidArrow").
	storeCount(5));
const ItemType& PoisonArrow = projectile(ItemTag::PoisonArrow, ESlotType::PoisonArrow, ItemTypeBuilder(TEXT("PoisonArrow"), LOCTEXT("PoisonArrow", "Poison Arrow"), LOCTEXT("Desc_PoisonArrow", "Mobs hit with these arrows take direct damage and are also afflicted with Poison."), "PoisonArrow").
		storeCount(5));
/////////////////////////////////////////////////
// Permanent
/////////////////////////////////////////////////
const ItemType& FireworksArrowItem = permanent(30s, 10s, ItemTypeBuilder(TEXT("FireworksArrowItem"),
	LOCTEXT("FireworksArrowItem", "Fireworks Arrow"),
	LOCTEXT("Desc_FireworksArrowItem", "Puts a fireworks arrow into your quiver. The arrow explodes on hit."), "FireworksArrowItem").
	flavour(LOCTEXT("Flavour_FireworksArrowItem", "The explosive power of the TNT combined with the high velocity movement of an arrow - what could possibly go wrong?")).
	archetype(EItemArchetype::Archer).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1))
	}))))
	);

const ItemType& Harvester = permanent(4s, ItemTypeBuilder(TEXT("Harvester"),
	LOCTEXT("Harvester", "Harvester"),
	LOCTEXT("Desc_Harvester", "When used, the Harvester releases souls in an explosion."), "Harvester").
	flavour(LOCTEXT("Flavour_Harvester", "The Harvester siphons the souls of the dead, before releasing them into a cluster hex of power.")).
	soulGatherItem().
	soulUseItem(40).
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_3))
	})))));

const ItemType& Harvester_Unique1 = permanent(1s, ItemTypeBuilder(TEXT("Harvester_Unique1"),
	LOCTEXT("Harvester_Unique1", "Blightbearer"),
	LOCTEXT("Desc_Harvester_Unique1", "When used, the Blightbearer releases souls in an explosion."), "Harvester_Unique1").
	flavour(LOCTEXT("Flavour_Harvester_Unique1", "The Blightbearer has been twisted by Illager witches to spread plague and disease.")).
	soulGatherItem().
	soulUseItem(40).
	characteristic({ LOCTEXT("mobs spawn poison clouds", "Poison Cloud on Mob"), EItemRarity::Unique }).
	unique().
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	workInProgress());

const ItemType& CorruptedSeeds = permanent(10s, 3s, ItemTypeBuilder(TEXT("CorruptedSeeds"),
	LOCTEXT("CorruptedSeeds_Unique1", "Corrupted Seeds"),
	LOCTEXT("Desc_CorruptedSeeds_Unique1", "Grow grapple vines, which inflict poison."), "CorruptedSeeds").
	flavour(LOCTEXT("Flavour_CorruptedSeeds_Unique1", "A pouch of poisonous corrupted seeds which grow into spiky grapple vines, entangling and slowly draining the life from its victims.")).
	characteristic({ LOCTEXT("grapple vines", "Briefly entangles mobs"), EItemRarity::Common }).
	characteristic({ LOCTEXT("poison grapple vines", "Poisons entangled mobs"), EItemRarity::Common }).	
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1))
	}))))
);

const ItemType& IceWand = permanent(20s, 5s, ItemTypeBuilder(TEXT("IceWand"),
	LOCTEXT("IceWand", "Ice Wand"),
	LOCTEXT("Desc_IceWand", "Creates large ice blocks that can crush your foes."), "IceWand").
	flavour(LOCTEXT("Flavour_IceWand", "The Ice Wand was trapped in a tomb of ice for ages, sealed away by those who feared its power.")).
	characteristic({ LOCTEXT("stuns_mobs_ice", "Stuns Mobs for 2 seconds"), EItemRarity::Common }).	
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1))
	}))))
);

const ItemType& GongOfWeakening = permanent(20s, 4s, ItemTypeBuilder(TEXT("GongOfWeakening"),
	LOCTEXT("GongOfWeakening", "Gong of Weakening"),
	LOCTEXT("Desc_GongOfWeakening", "Weakens enemies around you, decreasing their damage and defensive capabilities."), "GongOfWeakening").
	flavour(LOCTEXT("Flavour_GongOfWeakening", "This ancient gong, marked with the symbols of a nameless kingdom, feels safe in your hands but emits a menacing hum to those nearby.")).
	archetype(EItemArchetype::Support).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_2))
	})))));

const ItemType& LightFeather = permanent(3s, ItemTypeBuilder(TEXT("LightFeather"),
	LOCTEXT("LightFeather", "Light Feather"),
	LOCTEXT("Desc_LightFeather", "Launches you into an extra powerful roll that stuns and pushes enemies back."), "LightFeather").
	flavour(LOCTEXT("Flavour_LightFeather", "No one knows what mysterious creature this feather came from, but it is as beautiful and powerful.")).
	characteristic({ LOCTEXT("roll_trigger", "Activates Rolling Triggers"), EItemRarity::Common }).
	archetype(EItemArchetype::Acrobat).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)), 
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& WindHorn = permanent(10s, ItemTypeBuilder(TEXT("WindHorn"),
	LOCTEXT("WindHorn", "Wind Horn"),
	LOCTEXT("Desc_WindHorn", "Pushes enemies away from you and slows them briefly."), "WindHorn").
	flavour(LOCTEXT("Flavour_WindHorn", "When the Wind Horn echoes throughout the forests of the Overworld the creatures of the night tremble with fear.")).
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Support }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))));

const ItemType& BootsOfSwiftness = permanent(5s, 1.5s, ItemTypeBuilder(TEXT("BootsOfSwiftness"),
	LOCTEXT("BootsOfSwiftness", "Boots of Swiftness"),
	LOCTEXT("Desc_BootsOfSwiftness", "Gives a short boost to movement speed."), "BootsOfSwiftness").
	flavour(LOCTEXT("Flavour_BootsOfSwiftness", "Boots blessed with enchantments to allow for swift movements. Useful in uncertain times such as these.")).
	archetype(EItemArchetype::Acrobat).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& GhostCloak = permanent(6s, 1.5s, ItemTypeBuilder(TEXT("GhostCloak"),
	LOCTEXT("GhostCloak", "Ghost Cloak"),
	LOCTEXT("Desc_GhostCloak", "Briefly gain Ghost Form, allowing you to move through mobs and absorb some damage."), "GhostCloak").
	flavour(LOCTEXT("Flavour_GhostCloak", "The souls trapped within the Ghost Cloak are protective, but they radiate a sense of melancholy.")).
	archetypes({ EItemArchetype::Acrobat, EItemArchetype::Tank }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_3))
	})))));

const ItemType& FishingRod = permanent(5s, ItemTypeBuilder(TEXT("FishingRod"),
	LOCTEXT("FishingRod", "Fishing Rod"),
	LOCTEXT("Desc_FishingRod", "Pulls the closest mob in range to your position, briefly stunning that enemy."), "FishingRod").
	flavour(LOCTEXT("Flavour_FishingRod", "The trusted companion of any true adventurer, the Fishing Rod is useful for more than just fishing.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& DeathCapMushroom = permanent(30s, 8s, ItemTypeBuilder(TEXT("DeathCapMushroom"),
	LOCTEXT("DeathCapMushroom", "Death Cap Mushroom"),
	LOCTEXT("Desc_DeathCapMushroom", "Greatly increases attack and movement speed."), "DeathCapMushroom").
	flavour(LOCTEXT("Flavour_DeathCapMushroom", "Eaten by daring warriors before battle, the Death Cap Mushroom drives fighters into a frenzy.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))));

const ItemType& ShockPowder = permanent(15s, 3s, ItemTypeBuilder(TEXT("ShockPowder"),
	LOCTEXT("ShockPowder", "Shock Powder"),
	LOCTEXT("Desc_ShockPowder", "Stuns nearby enemies."), "ShockPowder").
	flavour(LOCTEXT("Flavour_ShockPowder", "Shock Powder is a reliable tool for those who wish to make a swift exit.")).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& IronHideAmulet = permanent(25s, 8s, ItemTypeBuilder(TEXT("IronHideAmulet"),
	LOCTEXT("IronHideAmulet", "Iron Hide Amulet"),
	LOCTEXT("Desc_IronHideAmulet", "Provides a major boost to defense for a short time."), "Amulets/IronHideAmulet").
	flavour(LOCTEXT("Flavour_IronHideAmulet", "The Iron Hide Amulet is both ancient and timeless. Sand mysteriously and endlessly slips through the cracks in the iron.")).
	archetype(EItemArchetype::Tank).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& TastyBone = permanent(30s, ItemTypeBuilder(TEXT("TastyBone"),
	LOCTEXT("TastyBone", "Tasty Bone"),
	LOCTEXT("Desc_TastyBone", "Summons a wolf to aid you in battle."), "TastyBone").
	flavour(LOCTEXT("Flavour_TastyBone", "You hear distant howling as you hold the Tasty Bone in your hand.")).
	archetype(EItemArchetype::Summoner).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_2)),
	})))));

const ItemType& WonderfulWheat = permanent(30s, ItemTypeBuilder(TEXT("WonderfulWheat"),
	LOCTEXT("WonderfulWheat", "Wonderful Wheat"),
	LOCTEXT("Desc_WonderfulWheat", "Summons a llama to aid you in battle."), "WonderfulWheat").
	flavour(LOCTEXT("Flavour_WonderfulWheat", "This item has the faint smell of an open field, baked bread, and freshly cut crops.")).
	archetype(EItemArchetype::Summoner).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& GolemKit = permanent(30s, ItemTypeBuilder(TEXT("GolemKit"),
	LOCTEXT("GolemKit", "Golem Kit"),
	LOCTEXT("Desc_GolemKit", "Summons an Iron Golem to aid you in battle."), "GolemKit").
	flavour(LOCTEXT("Flavour_GolemKit", "Iron Golems have always protected the Villagers of the Overworld. Their numbers are dwindling as a result of the Arch-Illager's war.")).
	archetype(EItemArchetype::Summoner).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::lostsettlement, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::trialsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& LoveMedallion = permanent(30s, ItemTypeBuilder(TEXT("LoveMedallion"),
	LOCTEXT("LoveMedallion", "Love Medallion"),
	LOCTEXT("Desc_LoveMedallion", "Turn up to three hostile mobs into allies for ten seconds before they disappear."), "LoveMedallion").
	flavour(LOCTEXT("Flavour_LoveMedallion", "A spell radiates from this trinket, enchanting those nearby into a trance where they must protect the holder of the medallion at all costs.")).
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::highblockhalls, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::archhaven, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::bamboobluff, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::trialsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))));

const ItemType& CorruptedBeacon = permanent(2.5s, ItemTypeBuilder(TEXT("CorruptedBeacon"),
	LOCTEXT("CorruptedBeacon", "Corrupted Beacon"),
	LOCTEXT("Desc_CorruptedBeacon", "Fires a high-powered beam that continuously damages mobs."), "CorruptedBeacon").
	flavour(LOCTEXT("Flavour_CorruptedBeacon", "The Corrupted Beacon holds immense power within. It waits for the moment to unleash its wrath.")).
	soulGatherItem().
	soulUseItem(2).
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_3)),
	})))));

const ItemType& CorruptedBeacon_Spooky1 = permanent(2.5s, ItemTypeBuilder(TEXT("CorruptedBeacon_Spooky1"),
	LOCTEXT("CorruptedBeacon_Spooky1", "Corrupted Pumpkin"),
	LOCTEXT("Desc_CorruptedBeacon_Spooky1", "Fires a high-powered beam that continuously damages mobs."), "CorruptedBeacon_Spooky1").
	flavour(LOCTEXT("Flavour_CorruptedBeacon_Spooky1", "The Corrupted Pumpkin glows with supernatural power, illuminating even the darkest nights with its powerful beacon.")).
	eventType(EItemEventType::Spooky).
	soulGatherItem().
	soulUseItem(2).
	unique().
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_2))
	})))));

const ItemType& LightningRod = permanent(2s, ItemTypeBuilder(TEXT("LightningRod"),
	LOCTEXT("LightningRod", "Lightning Rod"),
	LOCTEXT("Desc_LightningRod", "You can spend souls to call down a bolt of lightning onto an area."), "LightningRod").
	flavour(LOCTEXT("Flavour_LightningRod", "Crafted by Illager Geomancers, this item is enchanted with the power of a storming sky.")).
	soulGatherItem().
	soulUseItem(15).
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::obsidianpinnacle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::frozenfjord, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_2))
	})))));

const ItemType& SoulHealer = permanent(5s, ItemTypeBuilder(TEXT("SoulHealer"),
	LOCTEXT("SoulHealer", "Soul Healer"),
	LOCTEXT("Desc_SoulHealer", "Heals the most injured ally nearby, including yourself."), "SoulHealer").
	flavour(LOCTEXT("Flavour_SoulHealer", "The Soul Healer amulet is cold to the touch and trembles with the power of souls.")).
	soulGatherItem().
	soulUseItem(50).
	archetypes({ EItemArchetype::Soul, EItemArchetype::Support }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::overgrowntemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& TotemOfShielding = permanent(20s, 5s, ItemTypeBuilder(TEXT("TotemOfShielding"),
	LOCTEXT("TotemOfShielding", "Totem of Shielding"),
	LOCTEXT("Desc_TotemOfShielding", "This totem has mystical powers that shield those around it from projectiles."), "TotemOfShielding").
	flavour(LOCTEXT("Flavour_TotemOfShielding", "This totem radiates powerful energy that bursts forth as a protective shield around those near it.")).
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_2))
	})))));

const ItemType& TotemOfShielding_Unique1 = permanent(30s, 10s, ItemTypeBuilder(TEXT("TotemOfShielding_Unique1"),
	LOCTEXT("TotemOfShielding_Unique1", "Totem of Resistance"),
	LOCTEXT("Desc_TotemOfShielding_Unique1", "Creates a barrier that shields you and your allies from projectiles. When destroyed, it explodes and damages nearby enemies."), "TotemOfShielding_Unique1").
	flavour(LOCTEXT("Flavour_TotemOfShielding_Unique1", "The Totem of Resistance buzzes with self-destructive energy that bursts forth to protect you, at all costs.")).
	unique().
	archetypes({ EItemArchetype::Support, EItemArchetype::Tank }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	workInProgress());

const ItemType& TotemOfSoulProtection = permanent(1s, 30s, ItemTypeBuilder(TEXT("TotemOfSoulProtection"),
	LOCTEXT("TotemOfSoulProtection", "Totem of Soul Protection"),
	LOCTEXT("Desc_TotemOfSoulProtection", "Creates a space around the totem which, when you or allies die within it, revives the player."), "TotemOfSoulProtection").
	flavour(LOCTEXT("Flavour_TotemOfSoulProtection", "This totem radiates powerful protective magic.")).
	soulGatherItem().
	soulUseItem(40).
	archetype(EItemArchetype::Soul).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooncorecaverns, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1))
	})))).
	workInProgress());

const ItemType& TotemOfRegeneration = permanent(25s, 10s, ItemTypeBuilder(TEXT("TotemOfRegeneration"),
	LOCTEXT("TotemOfRegeneration", "Totem of Regeneration"),
	LOCTEXT("Desc_TotemOfRegeneration", "A totem that creates a circular aura, healing you and your allies."), "TotemOfRegeneration").
	flavour(LOCTEXT("Flavour_TotemOfRegeneration", "This hand-crafted wooden figurine radiates a warmth like that of a crackling campfire, healing those who gather around it.")).
	archetype(EItemArchetype::Support).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::soggyswamp, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& FlamingQuiver = permanent(30s, ItemTypeBuilder(TEXT("FlamingQuiver"),
	LOCTEXT("FlamingQuiver", "Flaming Quiver"),
	LOCTEXT("Desc_FlamingQuiver", "Gives you Burning Arrows."), "FlamingQuiver").
	flavour(LOCTEXT("Flavour_FlamingQuiver", "This quiver is filled with the deadliest of arrows.")).
	archetype(EItemArchetype::Archer).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::pumpkinpastures, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::fieryforge, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& TormentQuiver = permanent(6s, ItemTypeBuilder(TEXT("TormentQuiver"),
	LOCTEXT("TormentQuiver", "Torment Quiver"),
	LOCTEXT("Desc_TormentQuiver", "Gives slow arrows that knocks back mobs and passes through walls."), "TormentQuiver").
	flavour(LOCTEXT("Flavour_TormentQuiver", "The Torment Quiver radiates powerful energy drawn from the eternal source of the Undead.")).
	soulGatherItem().
	soulUseItem(30).
	archetypes({ EItemArchetype::Archer, EItemArchetype::Soul }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::deserttemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::lonelyfortress, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creeperwoods, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
	})))));

const ItemType& ThunderingQuiver = permanent(30s, ItemTypeBuilder(TEXT("ThunderingQuiver"),
	LOCTEXT("ThunderingQuiver", "Thundering Quiver"),
	LOCTEXT("Desc_ThunderingQuiver", "Gives you Thundering Arrows."), "ThunderingQuiver").
	flavour(LOCTEXT("Flavour_ThunderQuiver", "This copper quiver channels the power of lightning.")).
	archetype(EItemArchetype::Archer).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_2))
		})))));

const ItemType& SoulLantern = permanent(5s, 30s, ItemTypeBuilder(TEXT("SoulLantern"),
	LOCTEXT("SoulLantern", "Soul Lantern"),
	LOCTEXT("Desc_SoulLantern", "The Soul Lantern summons a creature of bound souls who will fight by your side for a short time."), "SoulLantern").
	flavour(LOCTEXT("Flavour_SoulLantern", "This lantern, still covered in the sands of some far-flung place, allows those who hold it to summon a creature formed from bound souls.")).
	soulUseItem(60).
	soulGatherItem().
	archetypes({ EItemArchetype::Soul, EItemArchetype::Summoner }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::underhalls, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::creepycrypt, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::cacticanyon, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soulsandvalley, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& BeeNest = permanent(23s, 16s, ItemTypeBuilder(TEXT("BeeNest"),
	LOCTEXT("BeeNest", "Buzzy Nest"),
	LOCTEXT("Desc_BeeNest", "When the Buzzy Nest is placed on the ground, bees who will fight beside you begin to spawn."), "Beenest").
	flavour(LOCTEXT("Flavour_BeeNest", "Bee lovers and the bee-loved alike are fans of the Buzzy Nest, but don't be fooled by the cute bees within - they pack a powerful sting!")).
	archetype(EItemArchetype::Summoner).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::lowertemple, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::dingyjungle, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::soggycave, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& RainbowGrass = permanent(30s, ItemTypeBuilder(TEXT("RainbowGrass"),
	LOCTEXT("RainbowGrass", "Enchanted Grass"),
	LOCTEXT("Desc_RainbowGrass", "Randomly summons one of three sheep allies that can grant either speed, poison, or fire effects."), "RainbowGrass").
	flavour(LOCTEXT("Flavour_RainbowGrass", "Just as there are powerful heroes who answer the call to fight, there are powerful enchanted sheep who will join the fight when summoned.")).
	vendorBlocked().
	archetypes({ EItemArchetype::Summoner, EItemArchetype::Mage }).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_3))
	})))));

const ItemType& SatchelOfTheElements = permanent(10s, ItemTypeBuilder(TEXT("SatchelOfTheElements"),
	LOCTEXT("SatchelOfTheElements", "Satchel Of Elements"),
	LOCTEXT("Desc_SatchelOfTheElements", "Hits nearby mobs with an elemental status effect: struck by lightning, frozen, or set ablaze."), "SatchelOfTheElements").
	flavour(LOCTEXT("Flavour_SatchelOfTheElements", "Mysteries surround this primordial satchel. Will it unleash fire, ice, or something a lot less nice?")).
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::galesanctum, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_2))
	})))));

const ItemType& EnchantersTome = permanent(15s, 8s, ItemTypeBuilder(TEXT("EnchantersTome"),
	LOCTEXT("EnchantersTome", "Enchanters Tome"),
	LOCTEXT("Desc_EnchantersTome", "Cast random enchantments on up to two allies that will aid them in unexpected ways."), "EnchantersTome").
	flavour(LOCTEXT("Flavour_EnchantersTome", "Meant only to be wielded by Enchanters, the magic of this artifact can summon powerful enchantments.")).
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))
	})))));

const ItemType& ChargedRedstoneMines = permanent(12s, ItemTypeBuilder(TEXT("ChargedRedstoneMines"),
	LOCTEXT("ChargedRedstoneMines", "Scatter Mines"),
	LOCTEXT("Desc_ChargedRedstoneMines", "Scatter three explosive mines on the ground around you that detonate when an enemy is in range."), "ChargedRedstoneMines").
	flavour(LOCTEXT("Flavour_ChargedRedstoneMines", "Set your enemies up for the surprise of a lifetime with the explosive power of Scatter Mines.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::endlessrampart, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))
		})))));

const ItemType& NetherWartSporeGrenade = permanent(6s, ItemTypeBuilder(TEXT("NetherWartSporeGrenade"),
	LOCTEXT("NetherWartSporeGrenade", "Blast Fungus"),
	LOCTEXT("Desc_NetherWartSporeGrenade", "Each fungus rolls in a different direction, then explodes and deals damage."), "NetherWartSporeGrenade").
	flavour(LOCTEXT("Flavour_NetherWartSporeGrenade", "Only the bravest of warriors carry the Blast Fungus. Not just because of its toxic spores, but because it smells awful.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::crimsonforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_2))
		})))));

const ItemType& FireworkBomb = permanent(10s, ItemTypeBuilder(TEXT("FireworkBomb"),
	LOCTEXT("FireworkBomb", "Firework Bomb"),
	LOCTEXT("Desc_FireworkBomb", "Throw out a firework bomb which on impact launches firework projectiles"), "FireworkBomb").
	flavour(LOCTEXT("Flavour_FireworkBomb", "Nether something mumbo jumbo")).
	characteristic({ LOCTEXT("firework_projectiles", "Lanches 3 firework projectiles"), EItemRarity::Common }).
	workInProgress().
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::mooshroomisland, FDifficulty(EGameDifficulty::Difficulty_1)),
		})))));

const ItemType& MobMasher = permanent(8s, 15s, ItemTypeBuilder(TEXT("MobMasher"),
	LOCTEXT("MobMasher", "Powershaker"),
	LOCTEXT("Desc_MobMasher", "When the Powershaker is activated, your next few melee attacks cause mobs to explode."), "MobMasher").
	flavour(LOCTEXT("Flavour_MobMasher", "The Powershaker is a smashing good time, though it may not be as fun for your enemies.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::warpedforest, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherfortress, FDifficulty(EGameDifficulty::Difficulty_2))

		})))));
const ItemType& SpinWheel = permanent(5s, ItemTypeBuilder(TEXT("SpinWheel"),
	LOCTEXT("SpinWheel", "Spinblade"),
	LOCTEXT("Desc_SpinWheel", "The Spinblade shoots towards enemies and then ricochets back to you, damaging foes as it flies."), "SpinWheel").
	flavour(LOCTEXT("Flavour_SpinWheel", "This whirling weapon spins across the battlefield, slicing through enemies in its path.")).
	archetype(EItemArchetype::Fighter).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::basaltdeltas, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::netherwastes, FDifficulty(EGameDifficulty::Difficulty_2))
		})))));
const ItemType& UpdraftTome = permanent(10s, ItemTypeBuilder(TEXT("UpdraftTome"),
	LOCTEXT("UpdraftTome", "Updraft Tome"),
	LOCTEXT("Desc_UpdraftTome", "Launch enemies into the air, stunning and damaging them."), "UpdraftTome").
	flavour(LOCTEXT("Flavour_UpdraftTome", "An ancient book filled with illegible glyphs, you feel a strange breeze as you flip through the pages.")).
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::windsweptpeaks, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::gauntletgales, FDifficulty(EGameDifficulty::Difficulty_1))
	}))))
);

const ItemType& GuardianEye = permanent(20s, 4s, ItemTypeBuilder(TEXT("GuardianEye"),
	LOCTEXT("GuardianEye", "Eye of the Guardian"),
	LOCTEXT("Desc_GuardianEye", "Fires a beam of energy that locks you into place as it deals damage to enemies in its path."), "GuardianEye").
	flavour(LOCTEXT("Flavour_GuardianEye", "The Eye of the Guardian holds a power that awakens in the hands of a worthy hero.")).
	archetype(EItemArchetype::Mage).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_3)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
	}))))
);

const ItemType& SatchelOfNourishment = permanent(20s, ItemTypeBuilder(TEXT("SatchelOfNourishment"),
	LOCTEXT("SatchelOfNourishment", "Satchel Of Snacks"),
	LOCTEXT("Desc_SatchelOfNourishment", "Crafts a random food item and throws it on the ground near you."), "SatchelOfNourishment").
	flavour(LOCTEXT("Flavour_SatchelOfNourishment", "The Satchel of Snacks provides a treat when you need it most!")).
	archetype(EItemArchetype::Support).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_2)),
	}))))
);

const ItemType& SatchelOfNeed = permanent(30s, ItemTypeBuilder(TEXT("SatchelOfNeed"),
	LOCTEXT("SatchelOfNeed", "Satchel Of Elixirs"),
	LOCTEXT("Desc_SatchelOfNeed", "Crafts two random potions based on your mission and throws them on the ground near you."), "SatchelOfNeed").
	flavour(LOCTEXT("Flavour_SatchelOfNeed", "The Satchel of Elixirs always contains the exact potions you need! (Well, at least the potions it thinks you need, which is still pretty helpful.)")).
	archetype(EItemArchetype::Support).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_2)),
		levels::allLevelsFor(ELevelNames::radiantravine, FDifficulty(EGameDifficulty::Difficulty_1)),
	}))))
);

const ItemType& HeavyDutyHarpoonQuiver = permanent(30s, ItemTypeBuilder(TEXT("HeavyHarpoonQuiver"),
	LOCTEXT("HeavyHarpoonQuiver", "Harpoon Quiver"),
	LOCTEXT("Desc_HeavyHarpoonQuiver", "Gives you Harpoons."), "HeavyHarpoonQuiver").
	flavour(LOCTEXT("Flavour_HeavyHarpoonQuiver", "This quiver holds a number of harpoons that swiftly pierce through enemies on land or underwater.")).
	archetype(EItemArchetype::Archer).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::coralrise, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::abyssalmonument, FDifficulty(EGameDifficulty::Difficulty_1)),
	}))))
);

const ItemType& ShadowSplinter = permanent(10s, 10s, ItemTypeBuilder(TEXT("ShadowSplinter"),
	LOCTEXT("ShadowSplinter", "Shadow Shifter"),
	LOCTEXT("Desc_ShadowSplinter", "Spend souls to enter Shadow Form"), "ShadowSplinter").
	flavour(LOCTEXT("Flavour_ShadowSplinter", "The Shadow Shifter grants you Shadow Form, which allows you to stay out of sight.")).
	archetype(EItemArchetype::Soul).
	soulGatherItem().
	soulUseItem(0.33f).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
		})))));

const ItemType& TomeOfDuplication = permanent(30s, ItemTypeBuilder(TEXT("TomeOfDuplication"),
	LOCTEXT("TomeOfDuplication", "Tome Of Duplication"),
	LOCTEXT("Desc_TomeOfDuplication", "Creates the last consumable collected."), "TomeOfDuplication").
	flavour(LOCTEXT("Flavour_TomeOfDuplication", "A magical tome which when read is capable of duplicating items such as food, potions and explosives!")).
	archetype(EItemArchetype::Support).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_2)),
		})))));

const ItemType& VoidQuiver = permanent(25s, ItemTypeBuilder(TEXT("VoidQuiver"),
	LOCTEXT("VoidQuiver", "Void Quiver"),
	LOCTEXT("Desc_Void Quiver", "Gives you Void Touched Arrows."), "VoidQuiver").
	flavour(LOCTEXT("Flavour_VoidQuiver", "The Void Quiver is filled with arrows that splash Void Liquid when they hit their target.")).
	archetype(EItemArchetype::Archer).
	restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
		})))));

const ItemType& LostEvocation = permanent(30s, ItemTypeBuilder(TEXT("LostEvocation"),
	LOCTEXT("LostEvocation", "Vexing Chant"),
	LOCTEXT("Desc_LostEvocation", "Summons Guardian Vexes."), "LostEvocation").
	flavour(LOCTEXT("Flavor_LostEvocation", "Summons Guardian Vexes who will fight by your side for a short time.")).
	archetype(EItemArchetype::Summoner).
	restrictTo(RETLAMBDA2((mergeRestrictions({
	levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_3))
		})))));

/////////////////////////////////////////////////
// Instant
/////////////////////////////////////////////////
const ItemType& TNTBox = instant(ItemTypeBuilder(TEXT("TNTBox"), LOCTEXT("TNTBox", "TNT"), LOCTEXT("Desc_TNTBox", "Explodes when thrown, damaging everyone in a circular area."), "TNTBox")
	.restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::dungeons(),
		levels::frozen(),
		levels::mountains(),
		levels::nether(),
		levels::end()
		})))));
const ItemType& Trident = instant(ItemTypeBuilder(TEXT("Trident"), LOCTEXT("Trident", "Trident"), LOCTEXT("Desc_Trident", "Sticks to the first target it hits, then explodes a few seconds later."), "Trident")
	.restrictTo(RETLAMBDA2(levels::oceans())));
const ItemType& Conduit = instant(ItemTypeBuilder(TEXT("Conduit"), LOCTEXT("Conduit", "Conduit"), LOCTEXT("Desc_Conduit", "Creates a sphere of oxygenated water, protecting players from drowning."), "Conduit"));
const ItemType& EyeOfEnder = instant(ItemTypeBuilder(TEXT("EyeOfEnder"), LOCTEXT("EyeOfEnder", "Eye of Ender"), LOCTEXT("Desc_EyeOfEnder", "Collect all the Eyes of Ender to restore the End Portal in the Stronghold."), "EyeOfEnder").
	tag(ItemTag::UIFriendly));
const ItemType& SwiftnessPotion = potion(ItemTypeBuilder(TEXT("SwiftnessPotion"), LOCTEXT("SwiftnessPotion", "Swiftness Potion"), LOCTEXT("Desc_SwiftnessPotion", "Briefly boosts movement speed."), "Potions/SwiftnessPotion").
	duration(20.0f));
const ItemType& StrengthPotion = potion(ItemTypeBuilder(TEXT("StrengthPotion"), LOCTEXT("StrengthPotion", "Strength Potion"), LOCTEXT("Desc_StrengthPotion", "Briefly boosts attack damage."), "Potions/StrengthPotion").
	duration(30.0f));
const ItemType& BackstabbersBrew = potion(ItemTypeBuilder(TEXT("BackstabbersBrew"), LOCTEXT("BackstabbersBrew", "Shadow Brew"), LOCTEXT("Desc_BackstabbersBrew", "Grants invisibility and boosts your next melee attack."), "Potions/BackstabbersBrew").
	duration(10.0f));
const ItemType& DefensePotion = potion(ItemTypeBuilder(TEXT("DefensePotion"), LOCTEXT("OakwoodBrew", "Oakwood Brew"), LOCTEXT("Desc_DefensePotion", "Briefly boosts defense."), "Potions/OakWoodBrew")
	.restrictTo(RETLAMBDA2(levels::jungle()))
	.duration(15.0f));
const ItemType& IcePotion = potion(ItemTypeBuilder(TEXT("IcePotion"), LOCTEXT("IcePotion", "Sweet Brew"), LOCTEXT("Desc_IcePotion", "Grants a small defense boost and resistance to freezing effects."), "Potions/SweetBrew")
	.restrictTo(RETLAMBDA2(levels::frozen()))
	.duration(120.0f));
const ItemType& DenseBrewPotion = potion(ItemTypeBuilder(TEXT("DenseBrewPotion"), LOCTEXT("DenseBrewPotion", "Dense Brew"), LOCTEXT("Desc_DenseBrewPotion", "Grants resistance to pushbacks and melee damage."), "Potions/DenseBrew")
	.restrictTo(RETLAMBDA2(levels::mountains()))
	.duration(10.0f));
const ItemType& WaterBreathingPotion = potion(ItemTypeBuilder(TEXT("WaterBreathingPotion"), LOCTEXT("WaterBreathingPotion", "Potion of Water Breathing"), LOCTEXT("Desc_WaterBreathingPotion", "Grants the ability to breathe freely underwater for a limited time."), "Potions/WaterBreathingPotion")
	.restrictTo(RETLAMBDA2(levels::oceans()))
	.duration(30.0f));
const ItemType& BurningBrewPotion = potion(ItemTypeBuilder(TEXT("BurningBrewPotion"), LOCTEXT("BurningBrewPotion", "Burning Brew"), LOCTEXT("Desc_BurningBrewPotion", "A spicy sip that will set you aflame! Damages enemies that get too close."), "Potions/BurningBrewPotion")
	.restrictTo(RETLAMBDA2(levels::end()))
	.duration(15.0f));
const ItemType& Elytra = instant(ItemTypeBuilder(TEXT("Elytra"), LOCTEXT("Elytra", "Elytra"), LOCTEXT("Desc_Elytra", "Grants the ability to blast off and glide."), "Elytra"));
const ItemType& Food1 = food(ItemTypeBuilder(TEXT("Food1"), LOCTEXT("Food1", "Bread"), LOCTEXT("Desc_Food1", "Heals 100% over 30 seconds"), "Food/Food1")
	.restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::dungeons(),
		levels::frozen(),
		levels::mountains(),
		levels::end()
	}))))
	.dropChance(0.05f));
const ItemType& Food2 = food(ItemTypeBuilder(TEXT("Food2"), LOCTEXT("Food2", "Apple"), LOCTEXT("Desc_Food2", "Heals 20% over 3 seconds"), "Food/Food2")
	.restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::dungeons(),
		levels::jungle(),
		levels::mountains(),
	}))))
	.dropChance(0.3f));
const ItemType& Food3 = food(ItemTypeBuilder(TEXT("Food3"), LOCTEXT("Food3", "Pork"), LOCTEXT("Desc_Food3", "Heals 50% over 10 seconds"), "Food/Food3")
	.restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::dungeons(),
		levels::frozen(),
		levels::mountains(),
		levels::nether()
	}))))
	.dropChance(0.1f));
const ItemType& Food4 = food(ItemTypeBuilder(TEXT("Food4"), LOCTEXT("Food4", "Melon"), LOCTEXT("Desc_Food4", "Heals 75% over 15 seconds"), "Food/Food4")
	.restrictTo(RETLAMBDA2(levels::jungle()))
	.dropChance(0.1f));
const ItemType& Food5 = food(ItemTypeBuilder(TEXT("Food5"), LOCTEXT("Food5", "Cooked Salmon"), LOCTEXT("Desc_Food5", "Heals 35% over 8 seconds"), "Food/Food5")
	.restrictTo(RETLAMBDA2(levels::frozen()))
	.dropChance(0.3f));
const ItemType& Food6 = food(ItemTypeBuilder(TEXT("Food6"), LOCTEXT("Food6", "Sweet Berries"), LOCTEXT("Desc_Food6", "Heals 20% over 10 seconds and boosts speed for 5 seconds"), "Food/Food6")
	.restrictTo(RETLAMBDA2(levels::mountains()))
	.dropChance(0.3f));
const ItemType& Food7 = food(ItemTypeBuilder(TEXT("Food7"), LOCTEXT("Food7", "Tropical Fish"), LOCTEXT("Desc_Food7", "Heals 20% over 2 seconds and replenishes 10% oxygen"), "Food/Food7")
	.restrictTo(RETLAMBDA2(levels::oceans()))
	.dropChance(0.2f));
const ItemType& Food8 = food(ItemTypeBuilder(TEXT("Food8"), LOCTEXT("Food8", "Chorus Fruit"), LOCTEXT("Desc_Food8", "Heals 30% over 1 second"), "Food/Food8")
	.restrictTo(RETLAMBDA2((mergeRestrictions({
		levels::allLevelsFor(ELevelNames::enderwilds, FDifficulty(EGameDifficulty::Difficulty_1)),
		levels::allLevelsFor(ELevelNames::blightedcitadel, FDifficulty(EGameDifficulty::Difficulty_1)),
		}))))
	.dropChance(0.3f));

const ItemType& DifficultyToken = instant(ItemTypeBuilder(TEXT("DifficultyToken"), LOCTEXT("DifficultyToken", "Threat Banner"), LOCTEXT("Desc_DifficultyToken", "Collecting a Threat Banner increases the threat level of the mission instantly!"), "Tokens/DifficultyToken").tag(ItemTag::Token));
const ItemType& MysteryToken = instant(ItemTypeBuilder(TEXT("MysteryToken"), LOCTEXT("MysteryToken", "Mystery Banner"), LOCTEXT("Desc_MysteryToken", "Collecting a Mystery Banner instantly adds a random modifier to the mission!"), "Tokens/MysteryToken").tag(ItemTag::Token));/////////////////////////////////////////////////


/////////////////////////////////////////////////
// WIP
/////////////////////////////////////////////////
const ItemType& EnderPearl = permanent(15s, ItemTypeBuilder(TEXT("EnderPearl"), LOCTEXT("EnderPearl", "Ender Pearl"), LOCTEXT("Desc_EnderPearl", "When thrown, teleports you to its point of impact."), "EnderPearl").
	workInProgress());
const ItemType& SplashSlowingPotion = ItemTypeBuilder(TEXT("SplashSlowingPotion"), LOCTEXT("SplashSlowingPotion", "Splash Potion of Slowing"), LOCTEXT("Desc_SplashSlowingPotion", "When thrown, slows all enemies in an area."), "Potions/SplashSlowingPotion").
	active().
	consumable().
	duration(10.0f).
	workInProgress().
	create();
const ItemType& BurningOilVial = ItemTypeBuilder(TEXT("BurningOilVial"), LOCTEXT("BurningOilVial", "Burning Arrow Oil"), LOCTEXT("Desc_BurningOilVial", "Sets your arrows on fire and makes them pierce through enemies, making them vastly more dangerous."), "BurningOilVial").
	active().
	consumable().
	duration(10.0f).
	workInProgress().
	create();
const ItemType& PlentifulQuiver = ItemTypeBuilder(TEXT("PlentifulQuiver"), LOCTEXT("PlentifulQuiver", "Plentiful Quiver"), LOCTEXT("Desc_PlentifulQuiver", "[PASSIVE] When equipped, slowly replenishes your arrows up to 20."), "PlentifulQuiver").
	active().
	cooldown(10.0f).
	workInProgress().
	create();
const ItemType& RecyclerQuiver = ItemTypeBuilder(TEXT("RecyclerQuiver"), LOCTEXT("RecyclerQuiver", "Recycler Quiver"), LOCTEXT("Desc_RecyclerQuiver", "[PASSIVE] When equipped, getting hit by arrows gives you a chance to gain that arrow."), "RecyclerQuiver").
	active().
	workInProgress().
	create();

/////////////////////////////////////////////////
// Other
/////////////////////////////////////////////////
const ItemType& HealthPotion = ItemTypeBuilder(TEXT("HealthPotion"), LOCTEXT("HealthPotion", "Health Potion"), LOCTEXT("Desc_HealthPotion", "A potion that restores health."), "Potions/HealthPotion").
	consumable(0).
	active().
	cooldown(45.0f).
	tag(ItemTag::HealthPotion).
	slotType(ESlotType::HealthPotion).
	create();
const ItemType& DiamondDust = ItemTypeBuilder(TEXT("DiamondDust"),
	LOCTEXT("DiamondDust", "Diamond Dust"),
	LOCTEXT("Desc_DiamondDust", "Use Diamond Dust to upgrade the power level of a piece of gear."),
	"DiamondDust").
	inventoryOnly().
	workInProgress().
	create();
const ItemType& Emerald = ItemTypeBuilder(TEXT("Emerald"), LOCTEXT("Emerald", "Emerald"), LOCTEXT("Desc_Emerald", "Emeralds are the currency of the Overworld. Use them to buy items and unlocks."), "Emerald").
	consumable().
	tag(ItemTag::Currency).	
	create();
const ItemType& Gold = ItemTypeBuilder(TEXT("Gold"), 
	LOCTEXT("Gold", "Gold"), 
	LOCTEXT("Desc_Gold", "Golds is the object of interest in the Nether. Piglins strive to get it above all other."), "Gold").
	flavour(LOCTEXT("Flavour_Gold", "Take your hard-earned gold to the piglin merchant in camp and see what they have in exchange.")).
	consumable().
	tag(ItemTag::Currency).
	create();
const ItemType& MysteryBoxAny = ItemTypeBuilder(TEXT("MysteryBoxAny"),
	LOCTEXT("MysteryBoxAny", "Random Item"),
	LOCTEXT("Desc_MysteryBoxAny", "Gives you a random artifact, armor, ranged weapon or melee weapon"),
	"MysteryBoxes/MysteryBoxAny")	
	.instant()	
	.localActivateOnly()
	.create();
const ItemType& MysteryBoxArtifact = ItemTypeBuilder(TEXT("MysteryBoxArtifact"),
	LOCTEXT("MysteryBoxArtifact", "Random Artifact"),
	LOCTEXT("Desc_MysteryBoxArtifact", "Gives you a random artifact"),
	"MysteryBoxes/MysteryBoxArtifact")	
	.instant()	
	.localActivateOnly()
	.create();
const ItemType& MysteryBoxGear = ItemTypeBuilder(TEXT("MysteryBoxGear"),
	LOCTEXT("MysteryBoxGear", "Random Gear"),
	LOCTEXT("Desc_MysteryBoxGear", "Gives you a random armor, ranged weapon or melee weapon"),
	"MysteryBoxes/MysteryBoxGear")
	.workInProgress() // we have cut this for simplifiction purposes.
	.instant()	
	.localActivateOnly()
	.create();
const ItemType& MysteryBoxMelee = ItemTypeBuilder(TEXT("MysteryBoxMelee"),
	LOCTEXT("MysteryBoxMelee", "Random Melee Weapon"),
	LOCTEXT("Desc_MysteryBoxMelee", "Gives you a random melee weapon"),
	"MysteryBoxes/MysteryBoxMelee")
	.instant()	
	.localActivateOnly()
	.create();
const ItemType& MysteryBoxRanged = ItemTypeBuilder(TEXT("MysteryBoxRanged"),
	LOCTEXT("MysteryBoxRanged", "Random Ranged Weapon"),
	LOCTEXT("Desc_MysteryBoxRanged", "Gives you a random ranged weapon"),
	"MysteryBoxes/MysteryBoxRanged")
	.instant()	
	.localActivateOnly()
	.create();
const ItemType& MysteryBoxArmor = ItemTypeBuilder(TEXT("MysteryBoxArmor"),
	LOCTEXT("MysteryBoxArmor", "Random Armor"),
	LOCTEXT("Desc_MysteryBoxArmor", "Gives you a random armor"),
	"MysteryBoxes/MysteryBoxArmor")
	.instant()	
	.localActivateOnly()
	.create();

const ItemType& GiftBox = ItemTypeBuilder(TEXT("GiftBox"),
	LOCTEXT("GiftBox", "Gift Box"),
	LOCTEXT("Desc_GiftBox", "Can be used to wrap gifts."),
	"GiftBoxes/GiftBox")
	.flavour(LOCTEXT("Flavour_GiftBox", "A box, some decorative paper and a beautiful string, suitable for wrapping gifts."))
	.instant()	
	.localActivateOnly()
	.create();

#undef LOCTEXT_NAMESPACE

}}}

