#include "Dungeons.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "Engine/AssetManager.h"
#include "Assets/ArmorPropertyAssetFinder.h"
#include "ArmorPropertyTypeDefs.h"
#include "util/DefsUtil.h"


namespace game { namespace armorproperties { namespace type {

class MutableArmorPropertyType : public ArmorPropertyType {

public:
	MutableArmorPropertyType(EArmorPropertyID id, EArmorPropertyClassification classification) 
		: ArmorPropertyType(id, classification) {
	}
	MutableArmorPropertyType& displayName(FText inDisplayName) { DisplayName = inDisplayName; return *this; };
	MutableArmorPropertyType& characteristic(FText inCharacteristic) { Characteristic = inCharacteristic; return *this; };
	MutableArmorPropertyType& tag(EArmorPropertyTag inTag) { Tags.Add(inTag); return *this; };
	MutableArmorPropertyType& disabled() { bDisabled = true; return *this; };
	MutableArmorPropertyType& preventGeneration() { bRandomGenerationPrevented = true; return *this; };
	MutableArmorPropertyType& generationEnabledByLevels(const TSet<ELevelNames>& missionUnlocks) { mRandomGenerationEnabledByLevels.Append(missionUnlocks); return *this; }
	MutableArmorPropertyType& blueprint(FString filename) {
		mRelativeClassPath = game::defs::generateBlueprintRelativePath(filename, "");
		mRelativeIconPath = game::defs::generateTextureRelativePath(filename, "_Icon");
		mRelativeMaterialPath = game::defs::generateMaterialInstanceRelativePath(filename, "_Icon");
		return *this;
	}
};	


static TArray<MutableArmorPropertyType> ArmorPropertyTypes = Util::createZeroedTArrayOfSize<MutableArmorPropertyType>(enum_cast(EArmorPropertyID::Last));

MutableArmorPropertyType& create(EArmorPropertyID id, EArmorPropertyClassification classification) {	
	const int index = enum_cast(id);	
	ArmorPropertyTypes[index] = MutableArmorPropertyType(id, classification);	
	return ArmorPropertyTypes[index];
}


#define LOCTEXT_NAMESPACE "ArmorProperties"

const ArmorPropertyType& Unset = create(EArmorPropertyID::Unset, EArmorPropertyClassification::Negative)
	.blueprint("Unset")
	.displayName(LOCTEXT("Unset", "Unset"))
	.characteristic(LOCTEXT("Unset_description", "Unset"));
const ArmorPropertyType& SoulGatheringBoost = create(EArmorPropertyID::SoulGatheringBoost, EArmorPropertyClassification::Positive)
	.blueprint("SoulGatheringBoost")
	.tag(EArmorPropertyTag::SoulGatheringBoost)
	.displayName(LOCTEXT("SoulGatheringBoost", "Soul Gathering Boost"))
	.characteristic(LOCTEXT("SoulGatheringBoost_description", "{0} souls gathered"));
const ArmorPropertyType& SuperbDamageAbsorption = create(EArmorPropertyID::SuperbDamageAbsorption, EArmorPropertyClassification::Positive)
	.blueprint("SuperbDamageAbsorption")
	.displayName(LOCTEXT("SuperbDamageAbsorption", "Superb Damage Absorption"))
	.characteristic(LOCTEXT("SuperbDamageAbsorption_description", "{0} damage reduction"));
const ArmorPropertyType& MissChance = create(EArmorPropertyID::MissChance, EArmorPropertyClassification::Positive)
	.blueprint("MissChance")
	.displayName(LOCTEXT("MissChance", "Miss Chance"))
	.characteristic(LOCTEXT("MissChance_description", "{0} chance to negate damage"));
const ArmorPropertyType& TeleportChance = create(EArmorPropertyID::TeleportChance, EArmorPropertyClassification::Positive)
	.blueprint("TeleportChance")
	.displayName(LOCTEXT("TeleportChance", "Teleport Chance"))
	.characteristic(LOCTEXT("TeleportChance_description", "{0} chance to teleport away when hit"))
	.disabled();
const ArmorPropertyType& ItemDamageBoost = create(EArmorPropertyID::ItemDamageBoost, EArmorPropertyClassification::Positive)
	.blueprint("ItemDamageBoost")
	.displayName(LOCTEXT("ArtifactDamageBoost", "Artifact Damage Boost"))
	.characteristic(LOCTEXT("ArtifactDamageBoost_description", "{0} artifact damage"));
const ArmorPropertyType& ItemCooldownDecrease = create(EArmorPropertyID::ItemCooldownDecrease, EArmorPropertyClassification::Positive)
	.blueprint("ItemCooldownDecrease")
	.displayName(LOCTEXT("ArtifactCooldownDecrease", "Artifact Cooldown Decrease"))
	.characteristic(LOCTEXT("ArtifactCooldownDecrease_description", "{0} artifact cooldown"));
const ArmorPropertyType& AllyDamageBoost = create(EArmorPropertyID::AllyDamageBoost, EArmorPropertyClassification::Positive)
	.blueprint("AllyDamageBoost")
	.displayName(LOCTEXT("AllyDamageBoost", "Ally Damage Boost"))
	.characteristic(LOCTEXT("AllyDamageBoost_description", "{0} weapon damage boost aura"));
const ArmorPropertyType& IncreasedArrowBundleSize = create(EArmorPropertyID::IncreasedArrowBundleSize, EArmorPropertyClassification::Positive)
	.blueprint("IncreasedArrowBundleSize")
	.displayName(LOCTEXT("IncreasedArrowBundleSize", "Increased Arrow-Bundle Size"))
	.characteristic(LOCTEXT("IncreasedArrowBundleSize_description", "{0} arrows per bundle"));
const ArmorPropertyType& MeleeDamageBoost = create(EArmorPropertyID::MeleeDamageBoost, EArmorPropertyClassification::Positive)
	.blueprint("MeleeDamageBoost")
	.displayName(LOCTEXT("MeleeDamageBoost", "Melee Damage Boost"))
	.characteristic(LOCTEXT("MeleeDamageBoost_description", "{0} melee damage"));
const ArmorPropertyType& MeleeAttackSpeedBoost = create(EArmorPropertyID::MeleeAttackSpeedBoost, EArmorPropertyClassification::Positive)
	.blueprint("MeleeAttackSpeedBoost")
	.displayName(LOCTEXT("MeleeAttackSpeedBoost", "Melee Attack Speed Boost"))
	.characteristic(LOCTEXT("MeleeAttackSpeedBoost_description", "{0} melee attack speed"));
const ArmorPropertyType& RangedDamageBoost = create(EArmorPropertyID::RangedDamageBoost, EArmorPropertyClassification::Positive)
	.blueprint("RangedDamageBoost")
	.displayName(LOCTEXT("RangedDamageBoost", "Ranged Damage Boost"))
	.characteristic(LOCTEXT("RangedDamageBoost_description", "{0} ranged damage"));
const ArmorPropertyType& LifeStealAura = create(EArmorPropertyID::LifeStealAura, EArmorPropertyClassification::Positive)
	.blueprint("LifeStealAura")
	.displayName(LOCTEXT("LifeStealAura", "Life Steal Aura"))
	.characteristic(LOCTEXT("LifeStealAura_description", "{0} life steal aura"));
const ArmorPropertyType& MoveSpeedAura = create(EArmorPropertyID::MoveSpeedAura, EArmorPropertyClassification::Positive)
	.blueprint("MoveSpeedAura")
	.displayName(LOCTEXT("MoveSpeedAura", "Move Speed Aura"))
	.characteristic(LOCTEXT("MoveSpeedAura_description", "{0} movespeed aura"));
const ArmorPropertyType& PetBat = create(EArmorPropertyID::PetBat, EArmorPropertyClassification::Positive)
	.blueprint("PetBat")
	.displayName(LOCTEXT("PetBat", "Pet Bat"))
	.characteristic(LOCTEXT("PetBat_description", "Gives you a pet bat"));
const ArmorPropertyType& AreaHeal = create(EArmorPropertyID::AreaHeal, EArmorPropertyClassification::Positive)
	.blueprint("AreaHeal")
	.displayName(LOCTEXT("AreaHeal", "Area Heal"))
	.characteristic(LOCTEXT("AreaHeal_description", "Health potions heal nearby allies"));
const ArmorPropertyType& MoveSpeedReduction = create(EArmorPropertyID::MoveSpeedReduction, EArmorPropertyClassification::Negative)
	.blueprint("MoveSpeedReduction")
	.displayName(LOCTEXT("MoveSpeedReduction", "Move Speed Reduction"))
	.characteristic(LOCTEXT("MoveSpeedReduction_description", "{0} movespeed"));
const ArmorPropertyType& IncreasedMobTargeting = create(EArmorPropertyID::IncreasedMobTargeting, EArmorPropertyClassification::Negative)
	.blueprint("IncreasedMobTargeting")
	.displayName(LOCTEXT("IncreasedMobTargeting", "Increased Mob Targeting"))
	.characteristic(LOCTEXT("IncreasedMobTargeting_description", "Mobs target you more"));
const ArmorPropertyType& PotionCooldownDecrease = create(EArmorPropertyID::PotionCooldownDecrease, EArmorPropertyClassification::Positive)
	.blueprint("PotionCooldownDecrease")
	.displayName(LOCTEXT("PotionCooldownDecrease", "Potion Cooldown Decrease"))
	.characteristic(LOCTEXT("PotionCooldownDecrease_description", "{0} potion cooldown"));
const ArmorPropertyType& DodgeCooldownIncrease = create(EArmorPropertyID::DodgeCooldownIncrease, EArmorPropertyClassification::Negative)
	.blueprint("DodgeCooldownIncrease")
	.displayName(LOCTEXT("DodgeCooldownIncrease", "Dodge Cooldown Increase"))
	.characteristic(LOCTEXT("DodgeCooldownIncrease_description", "{0} longer roll cooldown"));
const ArmorPropertyType& DodgeSpeedIncrease = create(EArmorPropertyID::DodgeSpeedIncrease, EArmorPropertyClassification::Positive)
	.blueprint("DodgeSpeedIncrease")
	.displayName(LOCTEXT("DodgeSpeedIncrease", "Dodge Speed Increase"))
	.characteristic(LOCTEXT("DodgeSpeedIncrease_description", "{0} faster roll"))
	.generationEnabledByLevels({ ELevelNames::dingyjungle, ELevelNames::overgrowntemple });
const ArmorPropertyType& DodgeInvulnerability = create(EArmorPropertyID::DodgeInvulnerability, EArmorPropertyClassification::Positive)
	.blueprint("DodgeInvulnerability")
	.displayName(LOCTEXT("DodgeInvulnerability", "Dodge Invulnerability"))
	.characteristic(LOCTEXT("DodgeInvulnerability_description", "Brief invulnerability when rolling"))
	.generationEnabledByLevels({ ELevelNames::dingyjungle, ELevelNames::overgrowntemple });
const ArmorPropertyType& DamageAbsorption = create(EArmorPropertyID::DamageAbsorption, EArmorPropertyClassification::Positive)
	.blueprint("DamageAbsorption")
	.displayName(LOCTEXT("DamageAbsorption", "Damage Absorption"))
	.characteristic(LOCTEXT("DamageAbsorption_description", "{0} damage reduction"))
	.preventGeneration();
const ArmorPropertyType& SlowResistance = create(EArmorPropertyID::SlowResistance, EArmorPropertyClassification::Positive)
	.blueprint("SlowResistance")
	.displayName(LOCTEXT("FreezingResistance", "Freezing Resistance"))
	.characteristic(LOCTEXT("FreezingResistance_description", "{0} Freezing Resistance"))
	.generationEnabledByLevels({ ELevelNames::lonelyfortress, ELevelNames::lostsettlement });
const ArmorPropertyType& DodgeGhostform = create(EArmorPropertyID::DodgeGhostForm, EArmorPropertyClassification::Positive)
	.blueprint("DodgeGhostForm")
	.displayName(LOCTEXT("DodgeGhostForm", "Ghost Form Dodge"))
	.characteristic(LOCTEXT("DodgeGhostForm_description", "Briefly gain Ghost Form when rolling"));
const ArmorPropertyType& Beekeeper = create(EArmorPropertyID::Beekeeper, EArmorPropertyClassification::Positive)
	.blueprint("Beekeeper")
	.displayName(LOCTEXT("Beekeeper", "Beekeeper"))
	.characteristic(LOCTEXT("Beekeeper_description", "{0} chance to summon a bee when hit (max {1})"));
const ArmorPropertyType& DodgeRoot = create(EArmorPropertyID::DodgeRoot, EArmorPropertyClassification::Positive)
	.blueprint("DodgeRoot")
	.displayName(LOCTEXT("DodgeRoot", "Dodge Root"))
	.characteristic(LOCTEXT("DodgeRoot_description", "Traps and poisons nearby mobs when rolling"))
	.generationEnabledByLevels({ ELevelNames::soulsandvalley, ELevelNames::warpedforest,  ELevelNames::netherwastes, ELevelNames::netherfortress,  ELevelNames::crimsonforest, ELevelNames::basaltdeltas });
const ArmorPropertyType& Heavyweight = create(EArmorPropertyID::Heavyweight, EArmorPropertyClassification::Positive).blueprint("Heavyweight")
	.displayName(LOCTEXT("Heavyweight", "Heavyweight"))
	.characteristic(LOCTEXT("Heavyweight_description", "Resists any form of knockback by {0}"))
	.generationEnabledByLevels({ ELevelNames::windsweptpeaks, ELevelNames::galesanctum, ELevelNames::endlessrampart });
const ArmorPropertyType& FallResistence = create(EArmorPropertyID::FallResistance, EArmorPropertyClassification::Positive).blueprint("FallResistance")
	.displayName(LOCTEXT("EnviromentalProtection", "Enviromental Protection"))
	.characteristic(LOCTEXT("EnviromentalProtection_description", "{0} Enviromental damage reduction"))
	.generationEnabledByLevels({ ELevelNames::windsweptpeaks, ELevelNames::galesanctum, ELevelNames::endlessrampart });
const ArmorPropertyType& EmeraldShield = create(EArmorPropertyID::EmeraldShield, EArmorPropertyClassification::Positive).blueprint("EmeraldShield")
	.displayName(LOCTEXT("EmeraldShield", "Emerald Shield"))
	.characteristic(LOCTEXT("EmeraldShield_description", "Brief damage immunity when collects an emerald"))
	.generationEnabledByLevels({ ELevelNames::windsweptpeaks, ELevelNames::galesanctum, ELevelNames::endlessrampart });
const ArmorPropertyType& SquidRoll = create(EArmorPropertyID::SquidRoll, EArmorPropertyClassification::Positive).blueprint("SquidRoll")
	.displayName(LOCTEXT("SquidRoll", "Squid Roll"))
	.characteristic(LOCTEXT("SquidRoll_description", "Release a squid ink cloud when rolling."))
	.generationEnabledByLevels({ ELevelNames::coralrise, ELevelNames::abyssalmonument, ELevelNames::radiantravine });
const ArmorPropertyType& EnvironmentalProtection = create(EArmorPropertyID::EnvironmentalProtection, EArmorPropertyClassification::Positive)
	.blueprint("EnvironmentalProtection")
	.displayName(LOCTEXT("EnvironmentalProtection", "Environmental Protection"))
	.characteristic(LOCTEXT("EnvironmentalProtection_description", "Environmental damage resistance"))
	.generationEnabledByLevels({ ELevelNames::windsweptpeaks, ELevelNames::galesanctum, ELevelNames::endlessrampart });
const ArmorPropertyType& ItemCooldownReset = create(EArmorPropertyID::ItemCooldownReset, EArmorPropertyClassification::Positive)
	.blueprint("ItemCooldownReset")
	.displayName(LOCTEXT("ItemCooldownReset", "Refreshing Brew"))
	.characteristic(LOCTEXT("ItemCooldownReset_description", "Reset Artifact cooldown on Potion use"))
	.generationEnabledByLevels({ ELevelNames::soulsandvalley, ELevelNames::warpedforest,  ELevelNames::netherwastes, ELevelNames::netherfortress,  ELevelNames::crimsonforest, ELevelNames::basaltdeltas });
const ArmorPropertyType& SquidRollQuick = create(EArmorPropertyID::SquidRollQuick, EArmorPropertyClassification::Positive)
	.blueprint("SquidRollQuick")
	.displayName(LOCTEXT("SquidRollQuick", "Quick Squid Roll"))
	.characteristic(LOCTEXT("SquidRollQuick_description", "Release an ink cloud when rolling"))
	.generationEnabledByLevels({ ELevelNames::coralrise, ELevelNames::abyssalmonument, ELevelNames::radiantravine });
const ArmorPropertyType& SquidRollLimited = create(EArmorPropertyID::SquidRollLimited, EArmorPropertyClassification::Positive)
	.blueprint("SquidRollLimited")
	.displayName(LOCTEXT("SquidRollLimited", "Limited Squid Roll"))
	.characteristic(LOCTEXT("SquidRollLimited_description", "Release an ink cloud when rolling"))
	.generationEnabledByLevels({ ELevelNames::coralrise, ELevelNames::abyssalmonument, ELevelNames::radiantravine });
const ArmorPropertyType& HealingAura = create(EArmorPropertyID::HealingAura, EArmorPropertyClassification::Positive)
	.blueprint("HealingAura")
	.displayName(LOCTEXT("HealingAura", "Healing Aura"))
	.characteristic(LOCTEXT("HealingAura_description", "{0} healing boost"))
	.generationEnabledByLevels({ ELevelNames::coralrise, ELevelNames::abyssalmonument, ELevelNames::radiantravine });
const ArmorPropertyType& ImmunityBoost = create(EArmorPropertyID::ImmunityBoost, EArmorPropertyClassification::Positive)
	.blueprint("ImmunityBoost")
	.displayName(LOCTEXT("ImmunityBoost", "Resilience"))
	.characteristic(LOCTEXT("ImmunityBoost_description", "-{0} negative status effect duration"))
	.generationEnabledByLevels({ ELevelNames::thestronghold });
const ArmorPropertyType& Resonant = create(EArmorPropertyID::Resonant, EArmorPropertyClassification::Positive)
	.blueprint("Resonant")
	.displayName(LOCTEXT("Resonant", "Harmony"))
	.characteristic(LOCTEXT("Resonant_description", "+{0} positive status effect duration"))
	.generationEnabledByLevels({ ELevelNames::thestronghold });
const ArmorPropertyType& InstantTransmission = create(EArmorPropertyID::InstantTransmission, EArmorPropertyClassification::Positive)
	.blueprint("InstantTransmission")
	.displayName(LOCTEXT("InstantTransmission", "Teleport"))
	.characteristic(LOCTEXT("InstantTransmission_description", "Roll to teleport"))
	.generationEnabledByLevels({ ELevelNames::enderwilds, ELevelNames::blightedcitadel });
#undef LOCTEXT_NAMESPACE


const ArmorPropertyType& getArmorPropertyType(EArmorPropertyID id) {
	const int index = enum_cast(id);
	ensure(index >= 0 && index < ArmorPropertyTypes.Num());
	return ArmorPropertyTypes[index];
}

const TArray<ArmorPropertyType> getArmorPropertiesOfClassification(EArmorPropertyClassification classification) {
	return algo::copy_if_map_tarray(ArmorPropertyTypes,
		RETLAMBDA(&it != &Unset && !it.isDisabled() && it.getClassification() == classification),
		RETLAMBDA(getArmorPropertyType(it.getId()))
	);
}

const TArray<ArmorPropertyType> getArmorProperties() {
	return algo::map_as<TArray<ArmorPropertyType>>(ArmorPropertyTypes, RETLAMBDA(getArmorPropertyType(it.getId())));
}

void PreloadArmorPropertyTypes()
{
	//Pre-stream all of the armor property caches to prevent load stalls & async flushes
	TArray<FSoftObjectPath> Names = IDungeonsModule::Get().GetArmorPropertyAssetFinder()->GetAssets();

	UAssetManager::GetStreamableManager().RequestAsyncLoad(Names, [Names]() {
		for (const auto& name : Names) {
			if (auto object = name.ResolveObject()) {
				object->AddToRoot();
			}
		}
	});
}

}}}
