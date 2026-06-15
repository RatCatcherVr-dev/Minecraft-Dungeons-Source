#include "Dungeons.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentType.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/ItemTypeDefs.h"
#include "util/CollectionUtils.h"
#include "CommonTypes.h"
#include "util/DefsUtil.h"

namespace game { namespace enchantment { namespace type {
	EEnchantmentCategory EnchantmentCategoryFromSlotType(ESlotType type) {
		switch (type)
		{
		case ESlotType::MeleeWeapon:
			return EEnchantmentCategory::Melee;
		case ESlotType::RangedWeapon:
			return EEnchantmentCategory::Ranged;
		case ESlotType::Armor:
			return EEnchantmentCategory::Armor;
		default:
			return EEnchantmentCategory::Unset;
		}
	}

	EEnchantmentCategory EnchantmentCategoryFromItemTag(ItemTag type) {
		switch (type)
		{
		case ItemTag::MeleeWeapon:
			return EEnchantmentCategory::Melee;
		case ItemTag::RangedWeapon:
			return EEnchantmentCategory::Ranged;
		case ItemTag::Armor:
			return EEnchantmentCategory::Armor;
		default:
			return EEnchantmentCategory::Unset;
		}
	}

	class MutableEnchantmentType : public EnchantmentType {
		using EnchantmentType::EnchantmentType;

	public:

		MutableEnchantmentType(EEnchantmentTypeID typeId) : EnchantmentType(typeId) {}

		MutableEnchantmentType& tags(TArray<EEnchantmentTag> enchantmentTags) {
			Tags = enchantmentTags;
			return *this;
		}

		MutableEnchantmentType& disabledForPlayers() {
			bIsDisabledForPlayer = true;
			return *this;
		}

		MutableEnchantmentType& disabledForMobs() {
			bCanBeUsedByMobs = false;
			return *this;
		}

		MutableEnchantmentType& mobDifficultyThreshold(const FDifficulty& difficulty) {
			DifficultyThreshold = difficulty;
			return *this;
		}

		MutableEnchantmentType& alwaysEnchantable() {
			bIsAlwaysEnchantable = true;
			return *this;
		}

		MutableEnchantmentType& hideFromInspector() {
			bHideFromInspector = true;
			return *this;
		}

		MutableEnchantmentType& category(EEnchantmentCategory category) {
			Category = category;
			return *this;
		}

		MutableEnchantmentType& classification(EEnchantmentClassification classification) {
			Classification = classification;
			return *this;
		}

		MutableEnchantmentType& rarity(EEnchantmentRarity rarity) {
			Rarity = rarity;
			return *this;
		}

		MutableEnchantmentType& blueprint(FString filename) {
			mRelativeClassPath = game::defs::generateBlueprintRelativePath(filename, "");
			mRelativeIconPath = game::defs::generateTextureRelativePath(filename, "_Icon");
			mRelativeMaterialPath = game::defs::generateMaterialInstanceRelativePath(filename, "_Icon");
			return *this;
		}


		MutableEnchantmentType& name(FText name) {
			DisplayName = name;
			return *this;
		}

		MutableEnchantmentType& description(FText description) {
			Description = description;
			return *this;
		}

		MutableEnchantmentType& characteristic(FText characteristic) {
			CharacteristicText = characteristic;
			return *this;
		}

		MutableEnchantmentType& effect(FText levelEffectTemplate) {
			LevelEffectTemplate = levelEffectTemplate;
			return *this;
		}	

		MutableEnchantmentType& effect(TFunction<const FText(int)> f) {
			GetEffectTemplate = std::move(f);
			return *this;
		}

		MutableEnchantmentType& workInProgress() {
			bWorkInProgress = true;
			return *this;
		}

		MutableEnchantmentType& soulGathering(int amount) {
			mSoulGatherAmount = amount;
			return *this;
		}
		
		MutableEnchantmentType& enchantersTomeFlags(EEnchantersTomeFlags enchantersTomeFlags) {
			EnchantersTomeFlags = enchantersTomeFlags;
			return *this;
		}
	};

	auto enchantmentTypesMutable = Util::createDefaultedTArrayOfSize<Unique<MutableEnchantmentType>>(enum_cast(EEnchantmentTypeID::Last));

	MutableEnchantmentType& create(EEnchantmentTypeID enchantmentId) {
		const int index = enum_cast(enchantmentId);		
		enchantmentTypesMutable[index] = make_unique<MutableEnchantmentType>(enchantmentId);
		return *enchantmentTypesMutable[index];
	}

#define LOCTEXT_NAMESPACE "Enchantment"
	// label reuse for frequency
	static const FText getLabelDuration() { return LOCTEXT("label_duration", "{0} duration"); }
	static const FText getLabelChanceToTrigger() { return LOCTEXT("label_chanceToTrigger", "{0} chance to trigger"); }
	static const FText getLabelDamagePerSecond() { return LOCTEXT("label_damagePerSecond", "{0} damage per second"); }
	
	// Melee
	const EnchantmentType& Unset = create(EEnchantmentTypeID::Unset)
		.category(EEnchantmentCategory::Unset)
		.blueprint("Unset")
		.name(LOCTEXT("Unset", "Unset"))
		.disabledForMobs()
		.disabledForPlayers();

	const EnchantmentType& Sharpness = create(EEnchantmentTypeID::Sharpness)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Damage)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Sharpness")
		.name(LOCTEXT("Sharpness", "Sharpness"))
		.effect(LOCTEXT("Sharpness_effect", "{0} increased damage"))
		.description(LOCTEXT("Sharpness_desc", "Makes your weapon sharper, causing it to deal more damage."))
		.characteristic(NSLOCTEXT("ItemType", "extra_damage", "Extra Damage"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Knockback = create(EEnchantmentTypeID::Knockback)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Knockback")
		.name(LOCTEXT("Knockback", "Knockback"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("Knockback_desc", "Every once in awhile, enemies will be knocked back after a melee attack."))
		.characteristic(NSLOCTEXT("ItemType", "knocks_enemies_back", "Knocks enemies back"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Rampaging = create(EEnchantmentTypeID::Rampaging)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Rampaging")
		.name(LOCTEXT("Rampaging", "Rampaging"))
		.effect(getLabelDuration())
		.description(LOCTEXT("Rampaging_desc", "After defeating a mob, there is a {0} chance to increase your attack speed by {1} for a short time."))
		.characteristic(NSLOCTEXT("ItemType", "increases_attack_speed", "Increases attack speed"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Freezing = create(EEnchantmentTypeID::Freezing)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("Freezing")
		.name(LOCTEXT("Freezing", "Freezing"))
		.effect(LOCTEXT("Freezing_effect", "{0} mob speed"))
		.description(LOCTEXT("Freezing_desc", "Slows mobs after hit for {0}."))
		.characteristic(NSLOCTEXT("ItemType", "freezes_mobs", "Slows Mobs"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& PoisonedMelee = create(EEnchantmentTypeID::PoisonedMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("PoisonedMelee")
		.name(LOCTEXT("Poisoned", "Poison Cloud"))
		.effect(getLabelDamagePerSecond())
		.description(LOCTEXT("Poisoned_desc", "Has a {0} chance to summon a poison cloud that deals damage to enemies in an area for {1}."))
		.characteristic(NSLOCTEXT("ItemType", "spawns_poison_clouds", "Spawns Poison Clouds"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.1f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& JunglePoisonMelee = create(EEnchantmentTypeID::JunglePoisonMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("JunglePoisonMelee")
		.name(LOCTEXT("JunglePoisonMelee", "Poison"))
		.characteristic(NSLOCTEXT("ItemType", "inflicts_poison", "Inflicts Poison"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Leeching = create(EEnchantmentTypeID::Leeching)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Leeching")
		.name(LOCTEXT("Leeching", "Leeching"))
		.effect(LOCTEXT("Leeching_effect", "{0} of mob max health"))
		.description(LOCTEXT("Leeching_desc", "Defeating a mob heals you a small portion of the mob's max health."))	//D11.PS changed the ' character as it was an illegal character	
		.characteristic(NSLOCTEXT("ItemType", "leeches_health_from_mobs", "Leeches Health from Mobs"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Aiding = create(EEnchantmentTypeID::Aiding)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Multi })
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Aiding")
		.name(LOCTEXT("Aiding", "Aiding"))		
		.disabledForMobs()
		.disabledForPlayers();

	const EnchantmentType& GravityMelee = create(EEnchantmentTypeID::GravityMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("GravityMelee")
		.name(LOCTEXT("GravityMelee", "Gravity"))
		.effect(getLabelDuration())
		.description(LOCTEXT("GravityMelee_desc", "This effect pulls mobs in range toward's the weapon's impact point."))
		.characteristic(NSLOCTEXT("ItemType", "pulls_in_enemies", "Pulls in enemies"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& EnigmaResonatorMelee = create(EEnchantmentTypeID::EnigmaResonatorMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("EnigmaResonatorMelee")
		.name(LOCTEXT("EnigmaMelee", "Enigma Resonator"))
		.effect(LOCTEXT("EnigmaMelee_effect", "0-{0} chance to trigger"))
		.description(LOCTEXT("Enigma_desc", "Gives a chance to deal {0} damage based on the number of souls you have."))
		.characteristic(NSLOCTEXT("ItemType", "souls_critical_boost", "Souls Critical Boost"))
		.soulGathering(1)
		.disabledForMobs();

	const EnchantmentType& AnimaConduitMelee = create(EEnchantmentTypeID::AnimaConduitMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("AnimaConduitMelee")
		.name(LOCTEXT("Anima", "Anima Conduit"))
		.effect(LOCTEXT("Anima_effect", "{0} health gained"))
		.description(LOCTEXT("Anima_desc", "Each soul you absorb grants a small amount of health."))
		.characteristic(NSLOCTEXT("ItemType", "soul_heal_increase", "Gain health from souls"))
		.soulGathering(1)
		.disabledForMobs();

	const EnchantmentType& Stunning = create(EEnchantmentTypeID::Stunning)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({})
		.blueprint("Stunning")
		.name(LOCTEXT("Stunning", "Stunning"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("Stunning_desc", "Chance to temporarily stun enemies."))
		.characteristic(NSLOCTEXT("ItemType", "stuns_mobs", "Stuns Mobs"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& CaveSpiderPoisonEnchantment = create(EEnchantmentTypeID::CaveSpiderPoisonEnchantment)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("CaveSpiderPoisonEnchantment")
		.name(LOCTEXT("Cave", "Cave Spider"))		
		.description(LOCTEXT("Cave_desc", "Spider Poison Enchantment"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Swirling = create(EEnchantmentTypeID::Swirling)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Damage)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Swirling")
		.name(LOCTEXT("Swirling", "Swirling"))		
		.effect(LOCTEXT("Swirling_effect", "Deals {0} damage"))
		.description(LOCTEXT("Swirling_desc", "Hitting an enemy with the last attack in a combo performs a swirling attack, damaging nearby enemies."))
		.characteristic(NSLOCTEXT("ItemType", "chance_to_spawn_area_damage", "Last Attack Deals Area Damage"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.1f));

	const EnchantmentType& RadianceMelee = create(EEnchantmentTypeID::RadianceMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Stacking })
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("RadianceMelee")
		.name(LOCTEXT("RadianceMelee", "Radiance"))
		.effect(LOCTEXT("RadianceMelee_effect", "{0} health healed"))
		.description(LOCTEXT("RadianceMelee_desc", "Has a {0} chance to spawn a circular area that heals you and all allies within the area."))
		.characteristic(NSLOCTEXT("ItemType", "heals_allies_in_the_area", "Heals Allies in the Area"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_1, 0.8f));

	const EnchantmentType& Chains = create(EEnchantmentTypeID::Chains)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Stacking, EEnchantmentTag::Multi })
		.blueprint("Chains")
		.name(LOCTEXT("Chains", "Chains"))
		.effect(getLabelDuration())		
		.description(LOCTEXT("Chains_desc", "Has a {0} chance to chain a cluster of mobs together and keep them bound for a short time."))
		.characteristic(NSLOCTEXT("ItemType", "binds_and_chains_enemies", "Binds and Chains Enemies"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.4f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Thundering = create(EEnchantmentTypeID::Thundering)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("Thundering")
		.name(LOCTEXT("Thundering", "Thundering"))
		.effect(LOCTEXT("Thundering_effect", "Deals {0} damage"))		
		.description(LOCTEXT("Thundering_desc", "Has a {0} chance to summon a lightning strike that damages nearby enemies."))
		.characteristic(NSLOCTEXT("ItemType", "fires_lightning_bolts", "Fires Lightning Bolts"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.3f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Echo = create(EEnchantmentTypeID::Echo)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("Echo")
		.name(LOCTEXT("Echo", "Echo"))
		.effect(LOCTEXT("Echo_effect", "{0} cooldown"))
		.description(LOCTEXT("Echo_desc", "Some of your attacks can be followed up by another attack in rapid succession."))
		.characteristic(NSLOCTEXT("ItemType", "sometimes_strikes_twice", "Sometimes Strikes Twice"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.1f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Shockwave = create(EEnchantmentTypeID::Shockwave)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Shockwave")
		.name(LOCTEXT("Shock", "Shockwave"))
		.effect(LOCTEXT("Shock_effect", "Deals {0} damage"))
		.description(LOCTEXT("Shock_desc", "The last attack in a combo launches a shockwave, damaging enemies."))
		.characteristic(NSLOCTEXT("ItemType", "casts_shockwaves", "Casts Shockwaves"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.5f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& BusyBee = create(EEnchantmentTypeID::BusyBee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("BusyBee")
		.name(LOCTEXT("BusyBee", "Busy Bee"))
		.effect(LOCTEXT("BusyBee_effect", "{0} chance to summon"))
		.description(LOCTEXT("BusyBee_desc", "Chance to summon a bee after defeating a mob, with up to {0} bees joining your side."))
		.characteristic(NSLOCTEXT("ItemType", "sometimes_summons_bee", "Chance to Summon a Bee"))
		.disabledForMobs();

	const EnchantmentType& DynamoMelee = create(EEnchantmentTypeID::DynamoMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("DynamoMelee")
		.name(LOCTEXT("DynamoMelee", "Dynamo"))
		.effect(LOCTEXT("DynamoMelee_effect", "{0} damage"))
		.description(LOCTEXT("DynamoMelee_desc", "Adds damage to the next attack after rolling, with multiple rolls stacking the damage effect."))
		.characteristic(NSLOCTEXT("ItemType", "rolling_makes_next_attack_stronger", "Rolling makes next attack stronger"))
		.disabledForMobs();

const EnchantmentType& BaneOfIllagers = create(EEnchantmentTypeID::BaneOfIllagers)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("BaneOfIllagers")
		.name(LOCTEXT("BaneOfIllagers", "Illager's Bane"))
		.effect(LOCTEXT("BaneOfIllagers_effect", "{0} increased damage"))
		.description(LOCTEXT("BaneOfIllagers_desc", "Attacks deal extra damage to Illagers."))
		.characteristic(NSLOCTEXT("ItemType", "extra_damage_to_illagers", "Extra Damage to Illagers"))
		.disabledForMobs();

	const EnchantmentType& WitherEnchantmentMelee = create(EEnchantmentTypeID::WitherEnchantmentMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("WitherEnchantmentMelee")
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Rushdown = create(EEnchantmentTypeID::Rushdown)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("Rushdown")
		.name(LOCTEXT("Rushdown", "Rushdown"))
		.effect(LOCTEXT("Rushdown_effect", "200% movement speed"))
		.description(LOCTEXT("Rushdown_desc", "Defeating a mob increases the players movement speed by 100% temporarily."))
		.characteristic(NSLOCTEXT("ItemType", "rushdown_speed_increase", "Speed burst after mob defeated"))
		.disabledForMobs()
		.disabledForPlayers();

	const EnchantmentType& DamageSynergy = create(EEnchantmentTypeID::DamageSynergy)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("DamageSynergy")
		.name(LOCTEXT("DamageSynergy", "Artifact Synergy"))
		.effect(LOCTEXT("DamageSynergy_effect", "Deals {0} extra damage"))
		.description(LOCTEXT("DamageSynergy_desc", "Whenever you use an artifact, your next attack deals bonus damage."))
		.characteristic(NSLOCTEXT("ItemType", "artifact_use_boost_damage", "Artifact Use Boosts Damage"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet)
		.disabledForMobs();
	
	const EnchantmentType& PainCycle = create(EEnchantmentTypeID::PainCycle)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("PainCycle")
		.name(LOCTEXT("PainCycle", "Pain Cycle"))
		.effect(LOCTEXT("PainCycle_effect", "Max stacks does {0} damage"))
		.description(LOCTEXT("PainCycle_desc", "Attacking drains your life to grant one stack of Pain Cycle. At {0} stacks, your pain is channeled into your next attack."))
		.characteristic(NSLOCTEXT("ItemType", "deal_extra_damage", "Deal Extra Damage"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet)
		.disabledForMobs();
	
	const EnchantmentType& SpongeStrike = create(EEnchantmentTypeID::SpongeStrike)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("SpongeStrike")
		.name(LOCTEXT("SpongeStrike", "Sponge Strike"))
		.description(LOCTEXT("SpongeStrike_desc", "Deals the damage taken, during combo, on last attack."))
		.characteristic(NSLOCTEXT("ItemType", "absords_damage_taken", "Deals absorbed damage during attack combo"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Heavyweight = create(EEnchantmentTypeID::Heavyweight)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("HeavyweightEnchantment")
		.name(LOCTEXT("Heavyweight", "Heavyweight"))
		.characteristic(NSLOCTEXT("ItemType", "absords_knockback", "Absorbs some knockback"))
		.description(LOCTEXT("Heavyweight_desc", "Resists any form of knockback by {0}"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& GuardingStrike = create(EEnchantmentTypeID::GuardingStrike)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("GuardingStrike")
		.name(LOCTEXT("GuardingStrike", "Guarding Strike"))
		.effect(LOCTEXT("GuardingStrike_effect", "{0} shield duration"))
		.characteristic(NSLOCTEXT("ItemType", "decrease_incoming_damage_short_duration", "Grants temporary shield"))
		.description(LOCTEXT("GuardingStrike_desc", "When you defeat a mob, you are given a temporary shield that reduces damage by 50%."));

	const EnchantmentType& PotionThirstMelee = create(EEnchantmentTypeID::PotionThirstMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("PotionThirstMelee")
		.name(LOCTEXT("PotionThirstMelee", "Refreshment"))
		.effect(LOCTEXT("PotionThirstMelee_effect", "{0} health potion cooldown"))
		.description(LOCTEXT("PotionThirstMelee_desc", "Defeating a mob reduces your health potion's cooldown time, allowing you to heal again more quickly."))
		.characteristic(NSLOCTEXT("ItemType", "decrease_cooldown_potion_short_duration", "Reduce health potion cooldown"))
		.disabledForMobs();


	const EnchantmentType& VoidTouchedMelee = create(EEnchantmentTypeID::VoidTouchedMelee)
		.category(EEnchantmentCategory::Melee)
		.rarity(EEnchantmentRarity::Powerful)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("VoidTouchedMelee")
		.name(LOCTEXT("VoidStrikeMelee", "Void Strike"))
		.effect(LOCTEXT("VoidStrikeMelee_effect", "+{0} max damage multiplier"))
		.description(LOCTEXT("VoidStrikeMelee_desc", "Attacking a target applies a damage multiplier that rises steadily to a max level, then is removed."))
		.characteristic(NSLOCTEXT("ItemType", "Void_Strike", "Applies Rising Damage Multiplier"));

	const EnchantmentType& SharedPain = create(EEnchantmentTypeID::SharedPain)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("SharedPain")
		.name(LOCTEXT("SharedPain", "Shared Pain"))
		.effect(LOCTEXT("SharedPain_effect", "SHAREDPAIN_EFFECT"))
		.description(LOCTEXT("SharedPain_desc", "SHAREDPAIN_DESC"))
		.characteristic(NSLOCTEXT("ItemType", "Shared_Pain", "Excess damage hits nearby mobs"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& BlindMelee = create(EEnchantmentTypeID::BlindMelee)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("BlindMelee")
		.name(LOCTEXT("BlindMelee", "Blind"))
		.effect(LOCTEXT("BlindMelee_effect", "BLIND_EFFECT"))
		.description(LOCTEXT("BlindMelee_desc", "BLIND_DESC"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Backstabber = create(EEnchantmentTypeID::Backstabber)
		.category(EEnchantmentCategory::Melee)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("Backstabber")
		.name(LOCTEXT("Backstabber", "Ambush"))
		.effect(LOCTEXT("Backstabber_effect", "{0}% Damage Bonus"))
		.description(LOCTEXT("Backstabber_desc", "Attacks on mobs that are not actively targeting you deal increased damage."))
		.characteristic(NSLOCTEXT("ItemType", "Ambush", "Extra damage on unsuspecting enemies"));

	const EnchantmentType& DamageCounter = create(EEnchantmentTypeID::DamageCounter)
		.category(EEnchantmentCategory::Permanent)
		.blueprint("DamageCounter")
		.name(LOCTEXT("DamageCounter", "Damage Counter"))
		.disabledForPlayers()
		.disabledForMobs();

	// Ranged;
	const EnchantmentType& TempoTheft = create(EEnchantmentTypeID::TempoTheft)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("TempoTheft")
		.name(LOCTEXT("Tempo", "Tempo Theft"))
		.effect(LOCTEXT("Tempo_effect", "{0} stolen speed"))
		.description(LOCTEXT("Tempo_desc", "Steals a small amount of a mob's movement speed and gives it to you for {0}."))
		.characteristic(NSLOCTEXT("ItemType", "steals_speed", "Steals Speed"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.1f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& AlacrityAdjustment = create(EEnchantmentTypeID::AlacrityAdjustment)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("AlacrityAdjustment")
		.name(LOCTEXT("Alacrity", "Alacrity Adjustment"))				
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& BowsBoon = create(EEnchantmentTypeID::BowsBoon)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::Multi })
		.blueprint("BowsBoon")
		.name(LOCTEXT("Bow", "Bow's Boon"))
		.description(LOCTEXT("Bow_desc", "Arrows passing through allies gives them a small amount of damage boost."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Ricochet = create(EEnchantmentTypeID::Ricochet)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Ricochet")
		.name(LOCTEXT("Ricochet", "Ricochet"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("Ricochet_desc", "Small chance for arrows to ricochet off mobs."))
		.characteristic(NSLOCTEXT("ItemType", "hits_multiple_targets", "Hits Multiple Targets"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Power = create(EEnchantmentTypeID::Power)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Power")
		.name(LOCTEXT("Power", "Power"))
		.effect(LOCTEXT("Power_effect", "{0} damage"))
		.description(LOCTEXT("Power_desc", "Power boosts arrow damage."))
		.characteristic(NSLOCTEXT("ItemType", "greater_damage", "Greater damage"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Punch = create(EEnchantmentTypeID::Punch)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Punch")
		.name(LOCTEXT("Punch", "Punch"))
		.effect(LOCTEXT("Punch_effect", "{0} pushback"))
		.description(LOCTEXT("Punch_desc", "Boosts arrow pushback."))
		.characteristic(NSLOCTEXT("ItemType", "additional_knockback", "Additional Knockback"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player)
		.disabledForMobs();

	const EnchantmentType& Infinity = create(EEnchantmentTypeID::Infinity)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Infinity")
		.name(LOCTEXT("Infinity", "Infinity"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("Infinity_desc", "Chance to immediately replenish an arrow after shooting."))
		.characteristic(NSLOCTEXT("ItemType", "chance_to_regain_arrows", "Chance to Regain Arrows"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& MultiShot = create(EEnchantmentTypeID::MultiShot)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("MultiShot")
		.name(LOCTEXT("Multi", "Multishot"))
		.effect(getLabelChanceToTrigger())		
		.description(LOCTEXT("Multi_desc", "Grants the chance to fire {0} arrows at once."))
		.characteristic(NSLOCTEXT("ItemType", "chance_for_multishot", "Chance for Multishot"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.1f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Piercing = create(EEnchantmentTypeID::Piercing)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Piercing")
		.name(LOCTEXT("Piercing", "Piercing"))
		.effect([](int level){ return level > 0? LOCTEXT("Piercing_effect","Every {0} arrow"): LOCTEXT("Piercing_effect_every","Every arrow");})
		.description(LOCTEXT("Piercing_desc", "Fired arrows sometimes gain the piercing effect, which allows them to fly through multiple mobs."))
		.characteristic(NSLOCTEXT("ItemType", "chance_to_fire_piercing_bolts", "Chance to fire piercing bolts"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& ProjectileCounter = create(EEnchantmentTypeID::ProjectileCounter)
		.category(EEnchantmentCategory::Permanent)
		.blueprint("ProjectileCounter")
		.name(LOCTEXT("ProjectileCounter", "Projectile Counter"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& ChainReaction = create(EEnchantmentTypeID::ChainReaction)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("ChainReaction")
		.name(LOCTEXT("ChainReaction", "Chain Reaction"))
		.effect(getLabelChanceToTrigger())		
		.description(LOCTEXT("Chain_desc", "Has a chance to fire {0} arrows in all directions on impact."))
		.characteristic(NSLOCTEXT("ItemType", "chance_of_chain_reaction", "Chance of Chain Reaction"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.7f))
		.alwaysEnchantable()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& GravityRanged = create(EEnchantmentTypeID::Gravity)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Gravity")
		.name(LOCTEXT("GravityRanged", "Gravity"))
		.effect(getLabelDuration())
		.description(LOCTEXT("GravityRanged_desc", "This effect pulls mobs in range toward's the weapon's impact point."))
		.characteristic(NSLOCTEXT("ItemType", "pulls_in_enemies", "Pulls in enemies"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& HuntingBowEnchantment = create(EEnchantmentTypeID::HuntingBowEnchantment)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("HuntingBowEnchantment")
		.name(LOCTEXT("HuntingBowEnchantment", "Hunting Bow Enchantment"))		
		.characteristic(NSLOCTEXT("ItemType", "pets_attack_targeted_mobs", "Pets Attack Targeted Mobs"))
		.disabledForPlayers()
		.alwaysEnchantable()
		.disabledForMobs();

	const EnchantmentType& SlowBowEnchantment = create(EEnchantmentTypeID::SlowBowEnchantment)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("SlowBowEnchantment")
		.name(LOCTEXT("SlowBowEnchantment", "Slow Bow Enchantment"))
		.characteristic(NSLOCTEXT("ItemType", "freezes_on_impact", "Freezes on Impact"))
		.disabledForPlayers()
		.alwaysEnchantable()
		.disabledForMobs();

	const EnchantmentType& WindBowEnchantment = create(EEnchantmentTypeID::WindBowEnchantment)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("WindBowEnchantment")
		.name(LOCTEXT("WindBowEnchantment", "Wind Bow Enchantment"))
		.characteristic(NSLOCTEXT("ItemType", "fires_gale_arrows", "Charged arrows pull in enemies"))
		.disabledForPlayers()
		.alwaysEnchantable()
		.disabledForMobs();

	const EnchantmentType& EnigmaResonatorRanged = create(EEnchantmentTypeID::EnigmaResonatorRanged)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("EnigmaResonatorRanged")
		.name(LOCTEXT("EnigmaRanged", "Enigma Resonator"))
		.effect(LOCTEXT("EnigmaRanged_effect", "0-{0} chance to trigger"))
		.description(LOCTEXT("EnigmaRanged_desc", "Gives a chance to deal {0} damage based on the number of souls you have."))
		.characteristic(NSLOCTEXT("ItemType", "souls_critical_boost", "Souls Critical Boost"))
		.soulGathering(1)
		.disabledForMobs();

	const EnchantmentType& AnimaConduitRanged = create(EEnchantmentTypeID::AnimaConduitRanged)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("AnimaConduitRanged")
		.name(LOCTEXT("Anima", "Anima Conduit"))
		.effect(LOCTEXT("AnimaRanged_effect", "{0} of max health per soul"))
		.description(LOCTEXT("AnimaRanged_desc", "Grants a small amount of health for each soul you absorb."))
		.characteristic(NSLOCTEXT("ItemType", "soul_heal_increase", "Gain health from souls"))
		.soulGathering(1)
		.disabledForMobs();

	const EnchantmentType& PoisonedRanged = create(EEnchantmentTypeID::PoisonedRanged)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("PoisonedRanged")
		.name(LOCTEXT("Poisoned", "Poison Cloud"))
		.effect(getLabelDamagePerSecond())
		.description(LOCTEXT("Poisoned_desc", "Has a {0} chance to summon a poison cloud that deals damage to enemies in an area for {1}."))
		.characteristic(NSLOCTEXT("ItemType", "spawns_poison_clouds", "Spawns Poison Clouds"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_1, 0.9f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& JunglePoisonRanged = create( EEnchantmentTypeID::JunglePoisonRanged )
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("JunglePoisonRanged")
		.name(LOCTEXT("JunglePoison", "Poison"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& FreezingRanged = create(EEnchantmentTypeID::FreezingRanged)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("FreezingRanged")
		.name(LOCTEXT("FreezingRanged", "Freezing Ranged"))
		.characteristic(NSLOCTEXT("ItemType", "freezes_mobs", "Slows Mobs"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& RapidFire = create(EEnchantmentTypeID::RapidFire)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("RapidFire")
		.name(LOCTEXT("RapidFire", "Rapid Fire"))
		.effect(LOCTEXT("RapidFire_effect", "{0} attack speed"))
		.description(LOCTEXT("RapidFire_desc", "Increases your attack speed."))
		.characteristic(NSLOCTEXT("ItemType", "increased_fire_rate", "Increased Fire Rate"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Supercharge = create(EEnchantmentTypeID::Supercharge)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("Supercharge")
		.name(LOCTEXT("Supercharge", "Supercharge"))
		.effect(LOCTEXT("Supercharge_effect", "{0} increased damage"))
		.description(LOCTEXT("Supercharge_desc", "Charged shots deal more damage and have more pushback."))
		.characteristic(NSLOCTEXT("ItemType", "super_charged_arrows", "Super Charged Arrows"))
		.disabledForMobs();

	const EnchantmentType& BonusShot = create(EEnchantmentTypeID::BonusShot)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("BonusShot")
		.name(LOCTEXT("BonusShot", "Bonus Shot"))
		.effect(LOCTEXT("BonusShot_effect", "{0} damage per shot"))
		.description(LOCTEXT("BonusShot_desc", "Firing a shot also fires a second shot at a nearby enemy. The second shot has reduced damage."))
		.characteristic(NSLOCTEXT("ItemType", "fires_dual_arrows", "Shoots Two Enemies At Once"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& FuseShot = create(EEnchantmentTypeID::FuseShot)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("FuseShot")
		.name(LOCTEXT("FuseShot", "Fuse Shot"))
		.effect([](int level){ return level > 0? LOCTEXT("FuseShot_effect", "Every {0} shot"): LOCTEXT("FuseShot_effect_allTheTime", "Every shot"); })
		.description(LOCTEXT("FuseShot_desc", "Every few shots has a timed charge that explodes {0} after impact, dealing {1} damage to nearby mobs."))
		.characteristic(NSLOCTEXT("ItemType", "chance_for_arrows_to_explode", "Chance for Arrows to Explode"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& RadianceRanged = create(EEnchantmentTypeID::RadianceRanged)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("RadianceRanged")
		.name(LOCTEXT("RadianceRanged", "Radiance Shot"))
		.effect(LOCTEXT("RadianceRanged_effect", "{0} health healed"))
		.description(LOCTEXT("RadianceRanged_desc", "Has a {0} chance to spawn a circular area that heals you and all allies within the area."))
		.characteristic(NSLOCTEXT("ItemType", "heals_allies_in_the_area", "Heals Allies in the Area"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Accelerating = create(EEnchantmentTypeID::Accelerating)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Accelerating")
		.name(LOCTEXT("Accelerate", "Accelerate"))
		.effect(LOCTEXT("Accelerating_effect", "{0} increase per shot"))
		.description(LOCTEXT("Accelerating_desc", "Increases attack speed for each consecutive shot. Resets {0} after the attack."))
		.characteristic(NSLOCTEXT("ItemType", "accelerated_fire_rate", "Accelerated Fire Rate"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Growing = create(EEnchantmentTypeID::Growing)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("Growing")
		.name(LOCTEXT("Growing", "Growing"))
		.effect(LOCTEXT("Growing_effect", "{0} max damage"))
		.description(LOCTEXT("Growing_desc", "The fired shot grows in the air, dealing extra damage to distant targets."))
		.characteristic(NSLOCTEXT("ItemType", "arrows_grow_size", "Arrows grow size"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& WildRage = create(EEnchantmentTypeID::WildRage)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("WildRage")
		.name(LOCTEXT("WildRage", "Wild Rage"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("WildRage_desc", "Hitting an enemy has a chance to send it into a rage, making it hostile towards everyone."))
		.characteristic(NSLOCTEXT("ItemType", "chance_enrage_mobs", "Chance to Enrage Mobs"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& DynamoRanged = create(EEnchantmentTypeID::DynamoRanged)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Offense)
		.blueprint("DynamoRanged")
		.name(LOCTEXT("DynamoRanged", "Dynamo"))
		.effect(LOCTEXT("DynamoRanged_effect", "{0} damage"))
		.description(LOCTEXT("DynamoRanged_desc", "Adds damage to the next attack after rolling."))
		.characteristic(NSLOCTEXT("ItemType", "rolling_makes_next_attack_stronger", "Rolling makes next attack stronger"))
		.disabledForMobs();
		
	const EnchantmentType& BurstBowstring = create(EEnchantmentTypeID::BurstBowstring)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("BurstBowstring")
		.name(LOCTEXT("BurstBowstring", "Burst Bowstring"))
		.effect(LOCTEXT("BurstBowstring_effect", "{0} targets"))
		.description(LOCTEXT("BurstBowstring_desc", "Shoots nearby mobs when you roll, costing {number_word} arrow per roll. Projectiles deal {percentage} of a charged shot's damage."))
		.characteristic(NSLOCTEXT("ItemType", "shoots_when_rolling", "Shoots arrows when rolling"))
		.disabledForMobs();

	const EnchantmentType& ChargingAcceleration = create(EEnchantmentTypeID::ChargingAcceleration)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("ChargingAcceleration")
		.name(LOCTEXT("ChargingAcceleration", "ChargingAcceleration"))
		.effect(LOCTEXT("ChargingAcceleration_effect", "{0} times increased"))
		.description(LOCTEXT("ChargingAcceleration_desc", "Attack speed charges up over time when not firing, decreases again when firing."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& CogCrossbowEnchantment = create(EEnchantmentTypeID::CogCrossbowEnchantment)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("CogCrossbowEnchantment")
		.name(LOCTEXT("CogCrossbowEnchantment", "CogCrossbowEnchantment"))
		.effect(LOCTEXT("CogCrossbowEnchantment_effect", "{0} times increased"))
		.description(LOCTEXT("CogCrossbowEnchantment_desc", "Charges up projectiles that fire quickly."))
		.characteristic(NSLOCTEXT("ItemType", "wind_up_attack", "Wind-Up Attack"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& CooldownShot = create(EEnchantmentTypeID::CooldownShot)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("CooldownShot")
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.name(LOCTEXT("CooldownShot", "Cooldown Shot"))
		.effect(LOCTEXT("CooldownShot_effect", "{0} artifact cooldown"))
		.description(LOCTEXT("CooldownShot_desc", "When you attack with a charged shot, artifact cooldowns are decreased."))
		.characteristic(NSLOCTEXT("ItemType", "cooldown_reduction_charged", "Charged Shot Reduces Cooldown"))
		.disabledForMobs();

	const EnchantmentType& ArtifactCharge = create(EEnchantmentTypeID::ArtifactCharge)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("ArtifactCharge")
		.name(LOCTEXT("ArtifactCharge", "Artifact Charge"))
		.effect([](int level) {return level == 1 ? LOCTEXT("ArtifactCharge_effect","Next attack charged") : LOCTEXT("ArtifactCharge_effect_plural","Next {0} attacks charged");})
		.description(LOCTEXT("ArtifactCharge_desc", "Whenever you use an artifact, your next few attacks become charged."))
		.characteristic(NSLOCTEXT("ItemType", "charged_on_item_use", "Charged Attacks After Artifact Use"))
		.disabledForMobs();

	const EnchantmentType& WitherEnchantmentRanged = create(EEnchantmentTypeID::WitherEnchantmentRanged)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("WitherEnchantmentRanged")
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& ReliableRicochet = create(EEnchantmentTypeID::ReliableRicochet)
		.category(EEnchantmentCategory::Ranged)
		.blueprint("ReliableRicochet")
		.name(LOCTEXT("ReliableRicochet", "Reliable Ricochet"))
		.characteristic(NSLOCTEXT("ItemType", "hits_multiple_reliable", "Hits Multiple Targets when charged"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& MultiCharge = create(EEnchantmentTypeID::MultiCharge)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("MultiCharge")
		.name(LOCTEXT("MultiCharge", "Overcharge"))
		.effect(LOCTEXT("MultiCharge_effect", "Can charge weapon {0} times."))
		.description(LOCTEXT("MultiCharge_desc", "Continue to hold the ranged attack button to double, triple, or quadruple charge your bow, increasing its damage each time."))
		.characteristic(NSLOCTEXT("ItemType", "multi_charge", "Boost charge shot"))
		.disabledForMobs();

	const EnchantmentType& PotionThirstRanged = create(EEnchantmentTypeID::PotionThirstRanged)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("PotionThirstRanged")
		.name(LOCTEXT("PotionThirstRanged", "Refreshment"))
		.effect(LOCTEXT("PotionThirstRanged_effect", "{0} health potion cooldown"))
		.description(LOCTEXT("PotionThirstRanged_desc", "Defeating a mob reduces your health potion's cooldown time, allowing you to heal again more quickly."))
		.characteristic(NSLOCTEXT("ItemType", "decrease_cooldown_potion_short_duration_ranged", "Reduce health potion cooldown"))
		.disabledForMobs();

	const EnchantmentType& ShockWeb = create(EEnchantmentTypeID::ShockWeb)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("ShockWeb")
		.name(LOCTEXT("ShockWeb", "Shock Web"))
		.effect(LOCTEXT("ShockWeb_effect", "Up to {0} beams"))
		.description(LOCTEXT("ShockWeb_desc", "Beams of lightning connect the last few charged arrows fired, which deal lightning damage to enemies who touch a beam."))
		.characteristic(NSLOCTEXT("ItemType", "beams_cause_damage", "Beams Cause Damage"))
		.disabledForMobs();

	const EnchantmentType& VoidTouchedRanged = create(EEnchantmentTypeID::VoidTouchedRanged)
		.category(EEnchantmentCategory::Ranged)
		.rarity(EEnchantmentRarity::Powerful)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("VoidTouchedRanged")
		.name(LOCTEXT("VoidStrikeRanged", "Void Strike"))
		.effect(LOCTEXT("VoidStrikeRanged_effect", "+{0} max damage multiplier"))
		.description(LOCTEXT("VoidStrikeRanged_desc", "Attacking a target applies a damage multiplier that rises steadily to a max level, then is removed."))
		.characteristic(NSLOCTEXT("ItemType", "Void_Strike", "Applies Rising Damage Multiplier"));

	const EnchantmentType& ShadowShot = create(EEnchantmentTypeID::ShadowShot)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("ShadowShot")
		.name(LOCTEXT("ShadowShot", "Shadow Shot"))
		.effect(LOCTEXT("ShadowShot_effect", "SHADOWSHOT_EFFECT"))
		.description(LOCTEXT("ShadowShot_desc", "A mob killed by an arrow from this weapon will make the player enter shadow form"))
		.characteristic(NSLOCTEXT("ItemType", "Shadow_Shot", "Chance of Shadow Form on kills"))
		.disabledForMobs()
		.disabledForPlayers();

	const EnchantmentType& ShadowBarbRanged = create(EEnchantmentTypeID::ShadowBarbRanged)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("ShadowBarbRanged")
		.name(LOCTEXT("ShadowBarbRanged", "Shadow Barb"))
		.effect(LOCTEXT("ShadowBarbRanged_effect", "SHADOWBARB_EFFECT"))
		.description(LOCTEXT("ShadowBarbRanged_desc", "Fire this weapon once without leaving Shadow Form."))
		.characteristic(NSLOCTEXT("ItemType", "Shadow_Barb", "First shot won't break Shadow Form"))
		.disabledForMobs()
		.disabledForPlayers();

	const EnchantmentType& LevitationShot = create(EEnchantmentTypeID::LevitationShot)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("LevitationShot")
		.name(LOCTEXT("LevitationShot", "Levitation Shot"))
		.effect(LOCTEXT("LevitationShot_effect", "{0} Fall Damage"))
		.description(LOCTEXT("LevitationShot_desc", "After rolling, your next attack will cause the target to levitate briefly."))
		.characteristic(NSLOCTEXT("ItemType", "Levitation_Shot", "Levitation Shot"));

	const EnchantmentType& DippingPoison = create(EEnchantmentTypeID::DippingPoison)
		.category(EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Damage)
		.blueprint("DippingPoison")
		.name(LOCTEXT("DippingPoison", "Dipping Poison"))
		.effect(LOCTEXT("DippingPoison_effect", "{0} poison arrows"))
		.description(LOCTEXT("DippingPoison_desc", "When you use a health potion, you are given a quiver of Poison Arrows."))
		.characteristic(NSLOCTEXT("ItemType", "DippingPoison", "Health Potion gives Poison Arrows"))
		.disabledForMobs();

	// Melee AND Ranged

	const EnchantmentType& Committed = create(EEnchantmentTypeID::Committed)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Damage)
		.blueprint("Committed")
		.name(LOCTEXT("Committed", "Committed"))
		.effect(LOCTEXT("Committed_effect", "0-{0} bonus damage"))
		.description(LOCTEXT("Committed_desc", "Deal increased damage against already wounded enemies."))
		.characteristic(NSLOCTEXT("ItemType", "increased_damage_to_wounded_mobs", "Increased damage to wounded mobs"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_1, 0.7f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& CriticalHit = create(EEnchantmentTypeID::CriticalHit)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Damage)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("CriticalHit")
		.name(LOCTEXT("Critical", "Critical Hit"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("Critical_desc", "Gives you a chance to inflict critical hits dealing {0} damage."))
		.characteristic(NSLOCTEXT("ItemType", "increases_critical_hit_chance", "Gives critical hit chance"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_1, 0.8f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Exploding = create(EEnchantmentTypeID::Exploding)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Offense)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Exploding")
		.name(LOCTEXT("Exploding", "Exploding"))
		.effect(LOCTEXT("Exploding_effect", "Deals {0} of enemy health as damage"))
		.description(LOCTEXT("Exploding_desc", "Mobs explode after they are defeated."))
		.characteristic(NSLOCTEXT("ItemType", "defeated_mobs_explode", "Defeated Mobs Explode"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& FireAspect = create(EEnchantmentTypeID::FireAspect)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Offense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("FireAspect")
		.name(LOCTEXT("Fire", "Fire Aspect"))
		.effect(getLabelDamagePerSecond())
		.description(LOCTEXT("Fire_desc", "Sets mobs on fire for {0}, dealing damage over time."))
		.characteristic(NSLOCTEXT("ItemType", "burns_mobs", "Burns Mobs"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Looting = create(EEnchantmentTypeID::Looting)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Looting")
		.name(LOCTEXT("Looting", "Looting"))
		.description(LOCTEXT("Looting_desc", "Increases the chance for mobs to drop consumables."))
		.characteristic(NSLOCTEXT("ItemType", "drop_more_consumables", "Drop More Consumables"))
		.disabledForMobs()
		.effect(LOCTEXT("Looting_effect", "{0} increased chance"));

	const EnchantmentType& Prospector = create(EEnchantmentTypeID::Prospector)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged | EEnchantmentCategory::Armor)
		.classification(EEnchantmentClassification::Defense)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Prospector")
		.name(LOCTEXT("Prospector", "Prospector"))
		.description(LOCTEXT("Prospector_desc", "Find more Emeralds on fallen enemies."))
		.characteristic(NSLOCTEXT("ItemType", "mobs_drop_more_emeralds", "Mobs Drop More Emeralds"))
		.effect(LOCTEXT("Prospector_effect", "{0} increased chance"))
		.disabledForMobs();

	const EnchantmentType& Smiting = create(EEnchantmentTypeID::Smiting)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Damage)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Smiting")
		.name(LOCTEXT("Smiting", "Smiting"))
		.effect(LOCTEXT("Smiting_effect", "{0} increased damage"))
		.description(LOCTEXT("Smiting_desc", "Increases damage against the Undead."))
		.characteristic(NSLOCTEXT("ItemType", "extra_damage_to_undead", "Extra Damage to Undead"))
		.disabledForMobs();

	const EnchantmentType& SoulSiphon = create(EEnchantmentTypeID::SoulSiphon)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Damage)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("SoulSiphon")
		.name(LOCTEXT("Soul", "Soul Siphon"))
		.effect(LOCTEXT("Soul_effect", "{0} souls"))
		.soulGathering(1)
		.disabledForMobs()
		.description(LOCTEXT("Soul_desc", "When you hit an enemy, gain a {0} chance to grant you a bunch of extra souls."))
		.characteristic(NSLOCTEXT("ItemType", "chance_to_gain_souls", "Chance to Gain Souls"));


	const EnchantmentType& Weakening = create(EEnchantmentTypeID::Weakening)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.classification(EEnchantmentClassification::Defense)
		.blueprint("Weakening")
		.name(LOCTEXT("Weakening", "Weakening"))
		.effect(LOCTEXT("Weakening_effect", "{0} mob damage"))
		.description(LOCTEXT("Weakening_desc", "Your attacks decrease the attack damage of all nearby enemies for {0}."))
		.characteristic(NSLOCTEXT("ItemType", "weakens_attack_damage_of_mobs", "Weakens Enemy Attacks"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Unchanting = create(EEnchantmentTypeID::Unchanting)
		.category(EEnchantmentCategory::Melee | EEnchantmentCategory::Ranged)
		.blueprint("Unchanting")
		.name(LOCTEXT("Unchanting", "Unchanting"))
		.effect(LOCTEXT("Unchanting_effect", "{0} bonus damage"))
		.description(LOCTEXT("Unchanting_desc", "Projectiles deal more damage to enchanted enemies."))
		.characteristic(NSLOCTEXT("ItemType", "enchanted_mobs_take_more_damage", "Strong vs Enchanted Mobs"))
		.disabledForMobs();


    // Aoe

	const EnchantmentType& FreezingAoe = create(EEnchantmentTypeID::FreezingAoe)
		.category(EEnchantmentCategory::Aoe)
		.blueprint("FreezingAoe")
		.name(LOCTEXT("Freezing", "Freezing"))
		.description(LOCTEXT("Freezing_desc", "Slows mobs after hit for {0}."))
		.characteristic(LOCTEXT("Freezing_characteristic", "Slows mobs after hit"))
		.disabledForPlayers()
		.disabledForMobs();

	// Armor;

	const EnchantmentType& Protection = create(EEnchantmentTypeID::Protection)
		.category(EEnchantmentCategory::Armor)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Protection")
		.name(LOCTEXT("Protection", "Protection"))
		.effect(LOCTEXT("Protection_effect", "{0} damage taken"))
		.description(LOCTEXT("Protection_desc", "Reduces damage taken."))
		.characteristic(NSLOCTEXT("ItemType", "damage_reduction", "Damage reduction"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Celerity = create(EEnchantmentTypeID::Celerity)
		.category(EEnchantmentCategory::Armor)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Celerity")
		.name(LOCTEXT("Cool Down", "Cool Down"))
		.effect(LOCTEXT("Celerity_effect", "{0} artifact cooldown"))
		.description(LOCTEXT("Celerity_desc", "Reduces the cooldown time between uses of your artifacts."))
		.characteristic(NSLOCTEXT("ItemType", "artifact_cooldown_reduction", "Reduced artifact cooldown"))
		.disabledForMobs();

	const EnchantmentType& FinalShout = create(EEnchantmentTypeID::FinalShout)
		.category(EEnchantmentCategory::Armor)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("FinalShout")
		.name(LOCTEXT("FinalShout", "Final Shout"))
		.effect(LOCTEXT("FinalShout_effect", "Up to every {0}"))
		.description(LOCTEXT("FinalShout_desc", "When your health drops below {0}, all your artifacts are used (ignoring cooldown periods)."))		
		.characteristic(NSLOCTEXT("ItemType", "force_activates_artifacts", "Artifacts activate on low HP"))
		.disabledForMobs();

	const EnchantmentType& Deflecting = create(EEnchantmentTypeID::Deflecting)
		.category(EEnchantmentCategory::Armor)
		.tags({  })
		.blueprint("Deflecting")
		.name(LOCTEXT("Deflect", "Deflect"))
		.effect(getLabelChanceToTrigger())		
		.description(LOCTEXT("Deflecting_desc", "Grants a small chance to deflect incoming projectiles."))
		.characteristic(NSLOCTEXT("ItemType", "deflect_chance", "Deflect enemy projectiles"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.9f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Regeneration = create(EEnchantmentTypeID::Regeneration)
		.category(EEnchantmentCategory::Armor)
		.tags({  })
		.blueprint("Regeneration")
		.name(LOCTEXT("Regeneration", "Regeneration"))
		.effect(LOCTEXT("Regeneration_effect", "{0} health per second"))
		.description(LOCTEXT("Regeneration_desc", "If you avoid taking damage for {0}, you will start regenerating health."))
		.characteristic(NSLOCTEXT("ItemType", "health_regeneration", "Health regeneration"))
		.disabledForPlayers();

	const EnchantmentType& Thorns = create(EEnchantmentTypeID::Thorns)
		.category(EEnchantmentCategory::Armor)
		.tags({ EEnchantmentTag::Stacking })
		.blueprint("Thorns")
		.name(LOCTEXT("Thorns", "Thorns"))
		.effect(LOCTEXT("Thorns_effect", "{0} damage returned"))		
		.description(LOCTEXT("Thorns_desc", "Whenever you take damage, you deal damage back to the attacker."))
		.characteristic(NSLOCTEXT("ItemType", "inflics_damage_to_attacker", "Inflicts damage to attacker"))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.2f))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& AncientGuardianThorns = create(EEnchantmentTypeID::AncientGuardianThorns)
		.category(EEnchantmentCategory::Armor)
		.blueprint("AncientGuardianThorns")
		.name(LOCTEXT("AncientGuardianThorns", "Ancient Guardian Thorns"))
		.effect(LOCTEXT("AncientGuardianThorns_effect", "{0} damage returned"))
		.description(LOCTEXT("AncientGuardianThorns_desc", "Whenever you take damage, you deal damage back to the attacker."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Altruistic = create(EEnchantmentTypeID::Altruistic)
		.category(EEnchantmentCategory::Armor)
		.tags({ EEnchantmentTag::Multi })
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Altruistic")
		.name(LOCTEXT("Altruistic", "Heals Allies"))
		.effect(LOCTEXT("Altruistic_effect", "{0} of damage becomes healing"))
		.description(LOCTEXT("Altruistic_desc", "Whenever you take damage you heal your allies."))
		.disabledForPlayers();

	const EnchantmentType& Shielding = create(EEnchantmentTypeID::Shielding)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Shielding")
		.name(LOCTEXT("Shielding", "Shielding"))
		.effect(LOCTEXT("Shielding_effect", "{0} damage"))		
		.description(LOCTEXT("Shielding_desc", "Adds damage resistance to you and nearby allies."))
		.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_2, 0.3f))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Barrier = create(EEnchantmentTypeID::Barrier)
		.category(EEnchantmentCategory::Armor)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Barrier")
		.name(LOCTEXT("Barrier", "Barrier"))
		.effect(LOCTEXT("Barrier_effect", "{0} resisted damage per enemy"))
		.description(LOCTEXT("Barrier_desc", "Your damage resistance increase with the number of nearby enemies."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& HuntingBowTaggedEnchantment = create(EEnchantmentTypeID::HuntingBowTaggedEnchantment)
		.category(EEnchantmentCategory::Armor)
		.blueprint("HuntingBowTaggedEnchantment")
		.name(LOCTEXT("HuntingBowTaggedEnchantment", "HuntingBowTaggedEnchantment"))		
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& Recycler = create(EEnchantmentTypeID::Recycler)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Recycler")
		.name(LOCTEXT("Recycler", "Recycler"))
		.effect(LOCTEXT("Recycler_effect", "Every {0} projectile"))
		.description(LOCTEXT("Recycler_desc", "Being hit by damage-inflicting projectiles will occasionally craft a small quiver of arrows."))
		.characteristic(NSLOCTEXT("ItemType", "crafts_arrows_on_hit", "Crafts arrows on hit"))
		.disabledForMobs();

	const EnchantmentType& Chilling = create(EEnchantmentTypeID::Chilling)
		.category(EEnchantmentCategory::Armor)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("Chilling")
		.name(LOCTEXT("Chilling", "Chilling"))
		.effect(LOCTEXT("Chilling_effect", "{0} mob speed"))
		.description(LOCTEXT("Chilling_desc", "Emits a blast every {0} that reduces the movement and attack speed of nearby enemies for {1}."))
		.characteristic(NSLOCTEXT("ItemType", "emits_a_chilling_aura", "Emits a Chilling Aura"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Cowardice = create(EEnchantmentTypeID::Cowardice)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Cowardice")
		.name(LOCTEXT("Cowardice", "Cowardice"))
		.effect(LOCTEXT("Cowardice_effect", "{0} increased ranged and melee damage"))
		.description(LOCTEXT("Cowardice_desc", "While you are at full HP, you deal increased ranged and melee damage."))
		.characteristic(NSLOCTEXT("ItemType", "full_hp_increases_damage", "Increase damage at full HP"))
		.disabledForMobs();

	const EnchantmentType& Electrified = create(EEnchantmentTypeID::Electrified)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Electrified")
		.name(LOCTEXT("Electrified", "Electrified"))
		.effect(LOCTEXT("Electrified_effect", "Deals {0} damage"))
		.description(LOCTEXT("Electrified_desc", "Rolling zaps {0} nearby enemies with lightning bolts, dealing damage."))
		.characteristic(NSLOCTEXT("ItemType", "zaps_enemies_when_rolling", "Zaps enemies when rolling"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& Burning = create(EEnchantmentTypeID::Burning)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Burning")
		.name(LOCTEXT("Burning", "Burning"))
		.effect(LOCTEXT("Burning_effect", "{0} damage"))
		.description(LOCTEXT("Burning_desc", "Every {0} damages all nearby enemies."))
		.characteristic(NSLOCTEXT("ItemType", "burns_nearby_enemies", "Burns Nearby Enemies"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Snowing = create(EEnchantmentTypeID::Snowing)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Snowing")
		.name(LOCTEXT("Snowing", "Snowball"))
		.effect(LOCTEXT("Snowing_effect", "Triggers every {0}"))
		.disabledForMobs()
		.description(LOCTEXT("Snowing_desc", "Fires a snowball at a nearby enemy every few seconds, briefly stunning it."))
		.characteristic(NSLOCTEXT("ItemType", "spawns_a_snowy_companion", "Spawns a Snowy Companion"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& GravityPulse = create(EEnchantmentTypeID::GravityPulse)
		.category(EEnchantmentCategory::Armor)
		.rarity(EEnchantmentRarity::Powerful)
		.blueprint("GravityPulse")
		.name(LOCTEXT("GravityPulse", "Gravity Pulse"))
		.effect(LOCTEXT("GravityPulse_effect", "{0} range"))
		.description(LOCTEXT("GravityPulse_desc", "A blast occurs every {0} that pulls nearby enemies towards you."))
		.characteristic(NSLOCTEXT("ItemType", "pulls_enemies_in", "Pulls enemies in"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& FireTrail = create(EEnchantmentTypeID::FireTrail)
		.category(EEnchantmentCategory::Armor)
		.blueprint("FireTrail")
		.name(LOCTEXT("FireTrail", "Fire Trail"))
		.effect(LOCTEXT("FireTrail_effect", "{0} fire damage per second"))
		.description(LOCTEXT("FireTrail_desc", "Rolling creates a trail of fire behind you, which deals damage to mobs for {0}."))
		.characteristic(NSLOCTEXT("ItemType", "spawns_fire_on_roll", "Spawns fire on roll"))
		.enchantersTomeFlags(EEnchantersTomeFlags::Pet);

	const EnchantmentType& Frenzied = create(EEnchantmentTypeID::Frenzied)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Frenzied")
		.name(LOCTEXT("Frenzied", "Frenzied"))
		.effect(LOCTEXT("Frenzied_effect", "{0} attack speed"))
		.description(LOCTEXT("Frenzied_desc", "While you are at less than {0} health, your melee and ranged attack speeds are increased."))
		.characteristic(NSLOCTEXT("ItemType", "increses_attack_speed_at_low_health", "Attack speed boost at low HP"));

	const EnchantmentType& Swiftfooted = create(EEnchantmentTypeID::Swiftfooted)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Swiftfooted")
		.name(LOCTEXT("Swiftfooted", "Swiftfooted"))
		.effect(LOCTEXT("Swiftfooted_effect", "{0} movement speed"))
		.description(LOCTEXT("Swiftfooted_desc", "Rolling makes you move faster for {0}."))
		.characteristic(NSLOCTEXT("ItemType", "gains_speed_after_dodge", "Gains Speed after Dodge"))
		.disabledForMobs()
		.enchantersTomeFlags(EEnchantersTomeFlags::Player);

	const EnchantmentType& SpiritSpeed = create(EEnchantmentTypeID::SpiritSpeed)
		.category(EEnchantmentCategory::Armor)
		.tags({ EEnchantmentTag::SoulUsage })
		.blueprint("SpiritSpeed")
		.name(LOCTEXT("SpiritSpeed", "Soul Speed"))
		.effect(getLabelDuration())
		.description(LOCTEXT("SpiritSpeed_desc", "When you gather a soul, you gain a {0} stacking movement speed boost for a short time."))
		.characteristic(NSLOCTEXT("ItemType", "speed_boost_when_gathering_souls", "Speed boost on soul pickup"))
		.soulGathering(1)
		.disabledForMobs();

	const EnchantmentType& PotionFortification = create(EEnchantmentTypeID::PotionFortification)
		.category(EEnchantmentCategory::Armor)
		.blueprint("PotionFortification")
		.name(LOCTEXT("PotionFortification", "Potion Barrier"))
		.effect(getLabelDuration())
		.description(LOCTEXT("PotionFortification_desc", "Whenever you use a healing potion, you take {0} damage for a short duration."))
		.characteristic(NSLOCTEXT("ItemType", "potion_use_boosts_defense", "Potion Use Boosts Defense"))
		.disabledForMobs();

	const EnchantmentType& FoodReserves = create(EEnchantmentTypeID::FoodReserves)
		.category(EEnchantmentCategory::Armor)
		.blueprint("FoodReserves")
		.name(LOCTEXT("FoodReserves", "Food Reserves"))
		.effect(LOCTEXT("FoodReserves_effect", "{0} items crafted"))
		.description(LOCTEXT("FoodReserves_desc", "Whenever you use a healing potion, you also create random food items."))
		.characteristic(NSLOCTEXT("ItemType", "spawn_food_on_potion_use", "Spawn food on potion use"))
		.disabledForMobs();

	const EnchantmentType& SurpriseGift = create(EEnchantmentTypeID::SurpriseGift)
		.category(EEnchantmentCategory::Armor)
		.blueprint("SurpriseGift")
		.name(LOCTEXT("SurpriseGift", "Surprise Gift"))
		.effect(getLabelChanceToTrigger())
		.description(LOCTEXT("SurpriseGift_desc", "Whenever you use a healing potion, you sometimes create random consumables."))
		.characteristic(NSLOCTEXT("ItemType", "chance_to_gain_consumables_on_consuming_potion", "Can get consumable on potion use"))
		.disabledForMobs();

	const EnchantmentType& DoubleDamage = create(EEnchantmentTypeID::DoubleDamage)
		.category(EEnchantmentCategory::Armor)
		.blueprint("DoubleDamage")
		.name(LOCTEXT("DoubleDamage", "Double Damage"))		
		.description(LOCTEXT("DoubleDamage_desc", "DoubleDamage_desc_NOT_USED"))
		.disabledForPlayers()
		.enchantersTomeFlags(EEnchantersTomeFlags::Absolute);

	const EnchantmentType& FastAttack = create(EEnchantmentTypeID::FastAttack)
		.category(EEnchantmentCategory::Armor)
		.blueprint("FastAttack")
		.name(LOCTEXT("FastAttack", "Fast Attack"))
		.description(LOCTEXT("FastAttack_desc", "FastAttack_desc_NOT_USED"))
		.disabledForPlayers()
		.enchantersTomeFlags(EEnchantersTomeFlags::Absolute);

	const EnchantmentType& Quick = create(EEnchantmentTypeID::Quick)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Quick")
		.name(LOCTEXT("Quick", "Quick"))
		.description(LOCTEXT("Quick_desc", "Quick_desc_NOT_USED"))
		.disabledForPlayers()
		.enchantersTomeFlags(EEnchantersTomeFlags::Absolute);

	const EnchantmentType& HealthSynergy = create(EEnchantmentTypeID::HealthSynergy)
		.category(EEnchantmentCategory::Armor)
		.blueprint("HealthSynergy")
		.name(LOCTEXT("HealthSynergy", "Health Synergy"))
		.effect(LOCTEXT("HealthSynergy_effect", "{0} health regained"))
		.description(LOCTEXT("HealthSynergy_desc", "When activating any artifact, you regain a small amount of health."))
		.characteristic(LOCTEXT("healthsynergy_characteristic", "Health Regen on Artifact Use"))
		.disabledForMobs();

	const EnchantmentType& SpeedSynergy = create(EEnchantmentTypeID::SpeedSynergy)
		.category(EEnchantmentCategory::Armor)
		.blueprint("SpeedSynergy")
		.name(LOCTEXT("SpeedSynergy", "Speed Synergy"))
		.effect(getLabelDuration())
		.description(LOCTEXT("SpeedSynergy_desc", "When activating any artifact, you gain {0} movement speed for a short time."))
		.characteristic(NSLOCTEXT("ItemType", "speed_boost_on_artifact_use", "Speed boost on artifact use"))
		.disabledForMobs();

	const EnchantmentType& Explorer = create(EEnchantmentTypeID::Explorer)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Explorer")
		.name(LOCTEXT("Explorer", "Explorer"))
		.effect(LOCTEXT("Explorer_effect", "{0} health regained"))
		.description(LOCTEXT("Explorer_desc", "For every one hundred blocks explored on the map, you regain a small amount of health."))
		.characteristic(NSLOCTEXT("ItemType", "gain_health_when_exploring", "Gain health when exploring"))
		.disabledForMobs();

	const EnchantmentType& VesselTrail = create(EEnchantmentTypeID::VesselTrail)
		.disabledForMobs()
		.disabledForPlayers()
		.category(EEnchantmentCategory::Armor)
		.blueprint("VesselTrail")
		.name(LOCTEXT("VesselTrail", "Vessel Trail"))
		.effect(LOCTEXT("VesselTrail_effect", ""))
		.description(LOCTEXT("VesselTrail_desc", ""));

	const EnchantmentType& ResurrectionSurge = create(EEnchantmentTypeID::ResurrectionSurge)
		.category(EEnchantmentCategory::Armor)
		.blueprint("ResurrectionSurge")
		.name(LOCTEXT("ResurrectionSurge", "Life Boost"))
		.description(LOCTEXT("ResurrectionSurge_desc", "Each time you respawn, your maximum health increases. Health goes back to normal after each mission."))
		.characteristic(NSLOCTEXT("ItemType", "increased_max_health_when_expending_life", "Health boost after respawn"))
		.effect(LOCTEXT("ResurrectionSurge_effect", "{0} extra health"))
		.disabledForMobs();

	const EnchantmentType& SlowImmunity = create(EEnchantmentTypeID::SlowImmunity)
		.category(EEnchantmentCategory::Armor)
		.blueprint("SlowImmunity")
		.name(LOCTEXT("SlowImmunity", "Slow Immunity"))
		.description(LOCTEXT("SlowImmunity_desc", "Grants the user immunity to slow effects."))
		.characteristic(LOCTEXT("SlowImmunity_characteristic", "Immune to Slow"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& PushVolumeImmunity = create(EEnchantmentTypeID::PushVolumeImmunity)
		.category(EEnchantmentCategory::Armor)
		.blueprint("PushVolumeImmunity")
		.name(LOCTEXT("PushVolumeImmunity", "Push Volume Immunity"))
		.description(LOCTEXT("PushVolumeImmunity_desc", "Grants the user immunity to push volumes."))
		.characteristic(LOCTEXT("PushVolumeImmunity_characteristic", "Immune to Push Volume"))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& WindResistance = create(EEnchantmentTypeID::WindResistance)
		.category(EEnchantmentCategory::Armor)
		.blueprint("WindResistance")
		.name(LOCTEXT("WindResistance", "Wind Resistance"))
		.description(LOCTEXT("WindResistance_desc", "Grants the user {0}% resistance to Wind."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& WindImmunity = create(EEnchantmentTypeID::WindImmunity)
		.category(EEnchantmentCategory::Armor)
		.blueprint("WindImmunity")
		.name(LOCTEXT("WindImmunity", "Wind Immunity"))
		.description(LOCTEXT("WindImmunity_desc", "Grants the user immunity to being pushed by the Wind."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& SlowResistance = create(EEnchantmentTypeID::SlowResistance)
		.category(EEnchantmentCategory::Armor)
		.blueprint("SlowResistanceEnchantment")
		.name(LOCTEXT("SlowResistance", "Slow Resistance"))
		.description(LOCTEXT("SlowResistance_desc", "Grants the user {0}% resistance to slow effects."))
		.disabledForPlayers()
		.disabledForMobs();

	const EnchantmentType& TumbleBee = create(EEnchantmentTypeID::TumbleBee)
		.category(EEnchantmentCategory::Armor)
		.blueprint("TumbleBee")
		.name(LOCTEXT("TumbleBee", "Tumble Bee"))
		.effect(LOCTEXT("TumbleBee_effect", "{0} chance to summon"))
		.description(LOCTEXT("TumbleBee_desc", "Chance to summon a bee when you roll, with up to {0} bees joining your side."))
		.characteristic(NSLOCTEXT("ItemType", "spawns_bees_when_rolling", "Spawn bees when rolling"))
		.disabledForMobs();

	const EnchantmentType& BagOfSouls = create(EEnchantmentTypeID::BagOfSouls)
		.category(EEnchantmentCategory::Armor)
		.blueprint("BagOfSouls")
		.name(LOCTEXT("BagOfSouls", "Bag Of Souls"))
		.description(LOCTEXT("BagOfSouls_desc", "Increases the maximum number of souls that can be carried."))
		.characteristic(NSLOCTEXT("ItemType", "increase_maximum_souls", "Increase maximum souls"))
		.effect(LOCTEXT("BagOfSoul_effect", "{0} extra souls"))
		.soulGathering(1)
		.disabledForMobs();
	
	const EnchantmentType& Acrobat = create(EEnchantmentTypeID::Acrobat)
		.category(EEnchantmentCategory::Armor)
		.blueprint("Acrobat")
		.name(LOCTEXT("Acrobat", "Acrobat"))
		.effect(LOCTEXT("Acrobat_effect", "{0} roll cooldown"))
		.description(LOCTEXT("Acrobat_desc", "Reduces the cooldown time between your rolls."))
		.characteristic(NSLOCTEXT("ItemType", "reduced_roll_cooldown", "Reduced roll cooldown"))
		.disabledForMobs();

	const EnchantmentType& RollCharge = create(EEnchantmentTypeID::RollCharge)
		.category(EEnchantmentCategory::Ranged)
		.tags({ EEnchantmentTag::ChargingWeaponsOnly })
		.blueprint("RollCharge")
		.name(LOCTEXT("RollCharge", "Roll Charge"))
		.effect(LOCTEXT("RollCharge_effect", "Charged Shot for {0}"))
		.description(LOCTEXT("RollCharge_desc", "After you perform a roll, all your ranged attacks are automatically fully charged for a short duration."))
		.characteristic(NSLOCTEXT("ItemType", "rolling_charges_next_arrow", "Charged arrows after roll"))
		.disabledForMobs();

	const EnchantmentType& MultiDodge = create(EEnchantmentTypeID::MultiDodge)
		.category(EEnchantmentCategory::Armor)
		.blueprint("MultiDodge")
		.rarity(EEnchantmentRarity::Powerful)
		.name(LOCTEXT("MultiDodge", "Multi-Roll"))
		.effect(LOCTEXT("MultiDodge_effect", "Grants {0} rolls"))
		.description(LOCTEXT("MultiDodge_desc", "Grants extra rolls."))
		.characteristic(NSLOCTEXT("ItemType", "extra_dodge", "Gives an extra roll"))
		.disabledForMobs();

	const EnchantmentType& EmeraldDivination = create(EEnchantmentTypeID::EmeraldDivination)
		.category(EEnchantmentCategory::Armor)
		.blueprint("EmeraldDivination")
		.name(LOCTEXT("EmeraldDivination", "Lucky Explorer"))
		.effect(LOCTEXT("EmeraldDivination_effect", "{0} emeralds spawned"))
		.description(LOCTEXT("EmeraldDivination_effect_desc", "Chance to spawn emeralds with every block explored."))
		.characteristic(NSLOCTEXT("ItemType", "may_spawn_emerald", "Chance to spawn emeralds when exploring"))
		.disabledForMobs();

const EnchantmentType& DeathBarter = create(EEnchantmentTypeID::DeathBarter)
	.category(EEnchantmentCategory::Armor)
	.blueprint("DeathBarter")
	.rarity(EEnchantmentRarity::Powerful)
	.name(LOCTEXT("DeathBarter", "Death Barter"))
	.effect(LOCTEXT("DeathBarter_effect", "Costs {0} emeralds"))
	.description(LOCTEXT("DeathBarter_effect_desc", "The first set amount of Emeralds collected are stored and then spent to save the player from death."))
	.characteristic(NSLOCTEXT("ItemType", "death_barter", "Can spend emeralds to cheat death"))
	.disabledForMobs();

const EnchantmentType& Huge = create(EEnchantmentTypeID::Huge)
	.category(EEnchantmentCategory::Armor)
	.blueprint("Huge")
	.name(LOCTEXT("Huge", "Huge"))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& ResurrectSurroundingMobs = create(EEnchantmentTypeID::ResurrectSurroundingMobs)
	.category(EEnchantmentCategory::Armor)
	.blueprint("ResurrectSurroundingMobs")
	.name(LOCTEXT("ResurrectSurroundingMobs", "Mob Resurrection Aura"))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& LuckOfTheSea = create(EEnchantmentTypeID::LuckOfTheSea)
		.category(EEnchantmentCategory::Armor)
		.blueprint("LuckOfTheSea")
		.name(LOCTEXT("LuckOfTheSea", "Luck Of The Sea"))
		.effect(LOCTEXT("LuckOfTheSea_effect", "{0} luck increase"))
		.description(LOCTEXT("LuckOfTheSea_desc", "Increases your luck, which boosts the chance that rare drops become unique."))
		.characteristic(NSLOCTEXT("ItemType", "luck_of_the_sea", "Increases Luck"))
		.disabledForMobs();

const EnchantmentType& Invisible = create(EEnchantmentTypeID::Invisible)
	.category(EEnchantmentCategory::Armor)
	.blueprint("Invisible")
	.rarity(EEnchantmentRarity::Powerful)
	.mobDifficultyThreshold(FDifficulty(EGameDifficulty::Difficulty_3, 0.1f))
	.name(LOCTEXT("Invisible", "Invisible"))
	.disabledForPlayers()
	.disabledForMobs();
	
const EnchantmentType& Venomancer = create(EEnchantmentTypeID::PoisonFocus)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.rarity(EEnchantmentRarity::Powerful)
	.blueprint("PoisonFocus")
	.name(LOCTEXT("PoisonFocus", "Poison Focus"))
	.effect(LOCTEXT("PoisonFocus_effect", "{0} increased poison damage"))
	.description(LOCTEXT("PoisonFocus_desc", "The poison damage you deal is increased."))
	.characteristic(NSLOCTEXT("ItemType", "extra_poison_damage", "Increases Poison Damage"))
	.disabledForMobs();


const EnchantmentType& Pyromancer = create(EEnchantmentTypeID::FireFocus)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.rarity(EEnchantmentRarity::Powerful)
	.blueprint("FireFocus")
	.name(LOCTEXT("FireFocus", "Fire Focus"))
	.effect(LOCTEXT("FireFocus_effect", "{0} increased fire damage"))
	.description(LOCTEXT("FireFocus_desc", "The fire damage you deal is increased."))
	.characteristic(NSLOCTEXT("ItemType", "extra_fire_damage", "Increases Fire Damage"))
	.disabledForMobs();

const EnchantmentType& Animancer = create(EEnchantmentTypeID::SoulFocus)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.rarity(EEnchantmentRarity::Powerful)
	.blueprint("SoulFocus")
	.name(LOCTEXT("SoulFocus", "Soul Focus"))
	.effect(LOCTEXT("SoulFocus_effect", "{0} increased soul damage"))
	.description(LOCTEXT("SoulFocus_desc", "Increases the amount of soul damage you deal."))
	.characteristic(NSLOCTEXT("ItemType", "extra_soul_damage", "Increases Soul Damage"))
	.disabledForMobs();

const EnchantmentType& Electromancer = create(EEnchantmentTypeID::LightningFocus)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.rarity(EEnchantmentRarity::Powerful)
	.blueprint("LightningFocus")
	.name(LOCTEXT("LightningFocus", "Lightning Focus"))
	.effect(LOCTEXT("LightningFocus_effect", "{0} increased lightning damage"))
	.description(LOCTEXT("LightningFocus_desc", "The lightning damage you deal is increased."))
	.characteristic(NSLOCTEXT("ItemType", "extra_lightning_damage", "Increases Lightning Damage"))
	.disabledForMobs();

const EnchantmentType& Reckless = create(EEnchantmentTypeID::Reckless)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.rarity(EEnchantmentRarity::Powerful)
	.blueprint("Reckless")
	.name(LOCTEXT("Reckless", "Reckless"))
	.effect(LOCTEXT("Reckless_effect", "Melee damage {0}"))
	.description(LOCTEXT("ShardArmor_desc", "Your health is reduced by {0}, but your melee damage is increased."))
	.characteristic(NSLOCTEXT("ItemType", "extra_damage_cost_health", "Melee Damage Increased"))
	.disabledForMobs();

const EnchantmentType& BeastBurst = create(EEnchantmentTypeID::BeastBurst)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.blueprint("BeastBurst")
	.name(LOCTEXT("BeastBurst", "Beast Burst"))
	.effect(LOCTEXT("BeastBurst_effect", "{0} explosion damage"))
	.description(LOCTEXT("BeastBurst_desc", "Using a healing potion causes an explosion at your pets' locations, dealing damage to mobs around them."))
	.characteristic(NSLOCTEXT("ItemType", "explosion_damage_at_pets", "Explosion at Pets' Location"))
	.disabledForMobs();

const EnchantmentType& BeastBoss = create(EEnchantmentTypeID::BeastBoss)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.blueprint("BeastBoss")
	.name(LOCTEXT("BeastBoss", "Beast Boss"))
	.effect(LOCTEXT("BeastBoss_effect", "+{0} pet damage"))
	.description(LOCTEXT("BeastBoss_desc", "Your pets deal more damage."))
	.characteristic(NSLOCTEXT("ItemType", "pets_do_more_damage", "Pets Deal More Damage"))
	.disabledForMobs();
	

const EnchantmentType& BeastSurge = create(EEnchantmentTypeID::BeastSurge)
	.category(EEnchantmentCategory::Armor)
	.classification(EEnchantmentClassification::Damage)
	.blueprint("BeastSurge")
	.name(LOCTEXT("BeastSurge", "Beast Surge"))
	.effect(LOCTEXT("BeastSurge_effect", "+{0} pet speed"))
	.description(LOCTEXT("BeastSurge_desc", "Using your health potion boosts your pets' attack and movement speed for 10 seconds."))
	.characteristic(NSLOCTEXT("ItemType", "enhanced_pets_attack", "Pet Attack and Move Boost"))
	.disabledForMobs();


const EnchantmentType& Flee = create(EEnchantmentTypeID::Flee)
	.category(EEnchantmentCategory::Armor)
	.blueprint("Flee")
	.name(LOCTEXT("Flee", "Rush"))
	.effect(LOCTEXT("Flee_effect", "{0} movement speed"))
	.description(LOCTEXT("Flee_effect_desc", "Increases movement speed for {0} after taking damage."))
	.characteristic(NSLOCTEXT("ItemType", "speed_boost_when_damaged", "Boost Speed after Hit"));

const EnchantmentType& CurrentResistance = create(EEnchantmentTypeID::CurrentResistance)
	.category(EEnchantmentCategory::Armor)
	.blueprint("CurrentResistance")
	.name(LOCTEXT("CurrentResistance", "Current Resistance"))
	.description(LOCTEXT("CurrentResistance_desc", "Grants the user {0}% resistance to ocean currents."))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& CurrentImmunity = create(EEnchantmentTypeID::CurrentImmunity)
	.category(EEnchantmentCategory::Armor)
	.blueprint("CurrentImmunity")
	.name(LOCTEXT("CurrentImmunity", "Current Immunity"))
	.description(LOCTEXT("CurrentImmunity_desc", "Grants the user immunity to being pushed by ocean currents."))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& ThriveUnderPressure = create(EEnchantmentTypeID::ThriveUnderPressure)
	.category(EEnchantmentCategory::Armor)
	.blueprint("ThriveUnderPressure")
	.name(LOCTEXT("ThriveUnderPressure", "Thrive Under Pressure"))
	.effect(LOCTEXT("ThriveUnderPressure_effect", "THRIVEUNDERPRESSURE_EFFECT"))
	.description(LOCTEXT("ThriveUnderPressure_desc", "Take 50% less damage when closely surrounded by at least 4 mobs."))
	.characteristic(NSLOCTEXT("ItemType", "Thrive_Underpressure", "Take less damage when swarmed"))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& VoidBlast = create(EEnchantmentTypeID::VoidBlast)
	.category(EEnchantmentCategory::Armor)
	.blueprint("VoidBlast")
	.name(LOCTEXT("VoidBlast", "Void Blast"))
	.effect(LOCTEXT("VoidBlast_effect", "VOIDBLAST_EFFECT"))
	.description(LOCTEXT("VoidBlast_desc", "VOIDBLAST_DESC"))
	.characteristic(NSLOCTEXT("ItemType", "Void_Blast", "Trigger explosion on rolling teleport"))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& ShulkerSentry = create(EEnchantmentTypeID::ShulkerSentry)
	.category(EEnchantmentCategory::Armor)
	.blueprint("ShulkerSentry")
	.name(LOCTEXT("ShulkerSentry", "Shulker Sentry"))
	.effect(LOCTEXT("ShulkerSentry_effect", "SHULKERSENTRY_EFFECT"))
	.description(LOCTEXT("ShulkerSentry_desc", "SHULKERSENTRY_DESC"))
	.characteristic(NSLOCTEXT("ItemType", "Shulker_Sentry", "Fire shulker projectiles at nearby enemies"))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& UnderwaterImmunity = create(EEnchantmentTypeID::UnderwaterImmunity)
	.category(EEnchantmentCategory::Armor)
	.blueprint("UnderwaterImmunity")
	.name(LOCTEXT("UnderwaterImmunity", "Underwater Immunity"))
	.description(LOCTEXT("UnderwaterImmunity_desc", "Grants the user drowning and underwater physics immunity."))
	.disabledForPlayers()
	.disabledForMobs();

const EnchantmentType& ShadowFlash = create(EEnchantmentTypeID::ShadowFlash)
	.category(EEnchantmentCategory::Armor)
	.blueprint("ShadowFlash")
	.name(LOCTEXT("ShadowFlash", "Shadow Blast"))
	.effect(LOCTEXT("shadowflash_effect", "{0} block blast radius"))
	.description(LOCTEXT("ShadowFlash_desc", "Leaving Shadow Form triggers a powerful blast that damages nearby mobs."))
	.characteristic(NSLOCTEXT("ItemType", "ShadowFlash", "Leaving Shadow Form causes blast"))
	.disabledForMobs();

const EnchantmentType& ShadowFeast = create(EEnchantmentTypeID::ShadowFeast)
	.category(EEnchantmentCategory::Armor)
	.blueprint("ShadowFeast")
	.name(LOCTEXT("ShadowFeast", "Shadow Surge"))
	.effect(LOCTEXT("shadowfeast_effect", "{0} souls per mob"))
	.description(LOCTEXT("ShadowFeast_desc", "Mobs defeated while in Shadow Form grant extra souls."))
	.characteristic(NSLOCTEXT("ItemType", "ShadowFeast", "Shadow Form grants more Souls"))
	.disabledForMobs();

const EnchantmentType& PlayerIdle = create(EEnchantmentTypeID::PlayerIdle)
	.category(EEnchantmentCategory::Permanent)
	.blueprint("PlayerIdle")
	.alwaysEnchantable()
	.hideFromInspector()
	.disabledForMobs()
	.disabledForPlayers();

const EnchantmentType& BardIdle = create(EEnchantmentTypeID::BardIdle)
	.category(EEnchantmentCategory::Permanent)
	.blueprint("BardIdle")
	.alwaysEnchantable()
	.hideFromInspector()
	.disabledForMobs()
	.disabledForPlayers();

const EnchantmentType& BardUnique1Idle = create(EEnchantmentTypeID::BardUnique1Idle)
	.category(EEnchantmentCategory::Permanent)
	.blueprint("BardUnique1Idle")
	.alwaysEnchantable()
	.hideFromInspector()
	.disabledForMobs()
	.disabledForPlayers();

#undef LOCTEXT_NAMESPACE


	const EnchantmentType& getEnchantmentType(EEnchantmentTypeID typeID) {
		const int index = enum_cast(typeID);
		return *enchantmentTypesMutable[index].get();
	}	

	static TArray<EnchantmentType> enchantmentTypes;

	const TArray<EnchantmentType>& getAvailableEnchantments() {
		if (enchantmentTypes.Num() <= 0) {
			for (auto&& enchtype : enchantmentTypesMutable) {
				if (enchtype)
				{
					enchantmentTypes.Add(*enchtype.get());
				}
			}
		}
		return enchantmentTypes;
	}
	
}}} // namespace game::enchantment::type

TAutoConsoleVariable<int32> CVarDebugDrawEnchantments(
	TEXT("Dungeons.DebugDraw.Enchantments"),
	0,
	TEXT("Enables debug drawing the enchantents e.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);
