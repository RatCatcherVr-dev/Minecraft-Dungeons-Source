#pragma once
#include "common_header.h"
#include <UnrealString.h>
#include "game/difficulty/Difficulty.h"

enum class ItemTag : uint8;
enum class ItemTagLevel : uint8;
enum class ESlotType : uint8;

UENUM(BlueprintType)
enum class EEnchantmentClassification : uint8 {
	Unset,
	Damage,
	Offense,
	Defense
};
ENUM_NAME(EEnchantmentClassification);


UENUM(BlueprintType)
enum class EEnchantmentRarity : uint8 {
	Common,
	Powerful,	
};
ENUM_NAME(EEnchantmentRarity);


UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EEnchantmentCategory : uint8 {
	Unset = 0,
	Melee = 0x01,
	Ranged = 0x02,
	Aoe = 0x04,
	Armor = 0x08,
	Permanent = 0x40
};
ENUM_CLASS_FLAGS(EEnchantmentCategory);


UENUM(BlueprintType)
enum class EEnchantmentTag : uint8 {
	Stacking,	
	Multi,
	SoulUsage,
	ChargingWeaponsOnly
};
ENUM_NAME(EEnchantmentTag);

UENUM(BlueprintType)
enum class EEnchantmentSource : uint8 {
	Unset,
	Permanent,
	Generated,
	Netherite,
	Dust
};
ENUM_NAME(EEnchantmentSource);

UENUM(BlueprintType)
enum class EEnchantersTomeFlags : uint8 {
	None = 0x0,
	Absolute = 0x1,
	Pet = 0x2,
	Player = 0x4,
};
ENUM_CLASS_FLAGS(EEnchantersTomeFlags)

UENUM(BlueprintType)
enum class EEnchantmentTypeID : uint8 {
	Unset,
	// Melee
	Sharpness,
	Knockback,
	Looting,
	Prospector,
	FireAspect,
	Rampaging,
	Exploding,
	CriticalHit,
	Freezing,
	PoisonedMelee,
	JunglePoisonMelee,
	Leeching,
	Aiding,
	GravityMelee,
	EnigmaResonatorMelee,
	AnimaConduitMelee,
	Stunning,
	CaveSpiderPoisonEnchantment,
	Swirling,
	Smiting,
	Committed,
	SoulSiphon,
	RadianceMelee,
	Chains,
	Thundering,
	Echo,
	Shockwave,
	Weakening,
	BusyBee,
	DynamoMelee,
	BaneOfIllagers,
	Rushdown,
	SpongeStrike,
	Heavyweight,
	DamageSynergy,
	PainCycle,
	GuardingStrike,
	PotionThirstMelee,
	WitherEnchantmentMelee,
	VoidTouchedMelee,
	SharedPain,
	BlindMelee,
	Backstabber,
	ShadowFlash,
	DamageCounter,
	ShadowFeast,
	// Ranged
	TempoTheft,
	AlacrityAdjustment,
	BowsBoon,
	Ricochet,
	Power,
	Punch,
	Infinity,
	MultiShot,
	Piercing,
	ProjectileCounter,
	ChainReaction,
	Gravity,
	HuntingBowEnchantment,
	EnigmaResonatorRanged,
	AnimaConduitRanged,
	PoisonedRanged,
	JunglePoisonRanged,
	FreezingRanged,
	RapidFire,
	Supercharge,
	BonusShot,
	Unchanting,
	FuseShot,
	RadianceRanged,
	Accelerating,
	Growing,
	WildRage,
	SlowBowEnchantment,
	DynamoRanged,
	BurstBowstring,
	ChargingAcceleration,
	CogCrossbowEnchantment,
	WindBowEnchantment,
	ReliableRicochet,
	MultiCharge,
	PotionThirstRanged,
	CooldownShot,
	ArtifactCharge,
	WitherEnchantmentRanged,
	ShockWeb,
	VoidTouchedRanged,
	ShadowShot,
	ShadowBarbRanged,
	LevitationShot,
	DippingPoison,
	// Aoe
	FreezingAoe,
	// Armor
	Protection,
	Celerity,
	FinalShout,
	Deflecting,
	Regeneration,
	Thorns,
	AncientGuardianThorns,
	Altruistic,
	Shielding,
	Barrier,
	HuntingBowTaggedEnchantment,
	Recycler,
	Chilling,
	Cowardice,
	Electrified,
	Burning,
	Snowing,
	GravityPulse,
	FireTrail,
	Frenzied,
	Swiftfooted,
	SpiritSpeed,
	PotionFortification,
	FoodReserves,
	SurpriseGift,
	DoubleDamage,
	FastAttack,
	Quick,
	HealthSynergy,
	SpeedSynergy,
	Explorer,
	VesselTrail,
	SlowResistance,
	SlowImmunity,
	TumbleBee,
	BagOfSouls,
	Acrobat,
	PushVolumeImmunity,
	WindResistance,
	WindImmunity,
	RollCharge,
	MultiDodge,
	EmeraldDivination,
	DeathBarter,
	ResurrectionSurge,
	Huge,
	ResurrectSurroundingMobs,
	Invisible,
	PoisonFocus,
	FireFocus,
	SoulFocus,
	LightningFocus,
	Flee,
	BeastSurge,
	BeastBurst,
	BeastBoss,
	Reckless,
	CurrentImmunity,
	CurrentResistance,
	ThriveUnderPressure,
	VoidBlast,
	ShulkerSentry,
	LuckOfTheSea,
	UnderwaterImmunity,
	PlayerIdle,
	BardIdle,
	BardUnique1Idle,
	Last
};
ENUM_NAME(EEnchantmentTypeID);


class EnchantmentType {
public:
	EnchantmentType();
	EnchantmentType(EEnchantmentTypeID);

	bool hasTag(EEnchantmentTag inTag) const;
	bool hasAnyTags() const;
	bool isRarity(EEnchantmentRarity rarity) const;

	const FName& getRelativeClassPath() const { return mRelativeClassPath; }
	const FName& getRelativeIconPath() const { return mRelativeIconPath; }
	const FName& getRelativeMaterialPath() const { return mRelativeMaterialPath; }
	bool hasEnchantersTomeFlags(EEnchantersTomeFlags flags) const;
	const FText& getDisplayName() const;
	const FText& getDescription() const;
	const FText& getCharacteristicText() const;
	bool isDisabledForPlayers() const;
	bool isHiddenInInspector() const { return bHideFromInspector; }
	bool canBeUsedByMobs() const;
	int getSoulGatherCount() const;
	const EEnchantmentTypeID getEnchantmentTypeID() const;
	const EEnchantmentClassification getEnchantmentClassification() const;
	const EEnchantmentCategory getEnchantmentCategory() const;
	const EEnchantmentRarity getRarity() const;
	bool isAlwaysEnchantable() const;
	game::FDifficulty difficultyThreshold() const;

	const FText getLevelEffectTemplate(int level = 0) const;

	bool isWorkInProgress() const;
protected:
	EEnchantmentCategory Category = EEnchantmentCategory::Unset;

	EEnchantmentClassification Classification = EEnchantmentClassification::Unset;

	EEnchantmentRarity Rarity = EEnchantmentRarity::Common;

	EEnchantmentTypeID TypeID = EEnchantmentTypeID::Unset;

	EEnchantersTomeFlags EnchantersTomeFlags = EEnchantersTomeFlags::None;

	TArray<EEnchantmentTag> Tags;

	//Localization
	FText DisplayName;
	FText Description;
	FText CharacteristicText;

	FText LevelEffectTemplate;
	FText mTempVal;
	TFunction<const FText(int)> GetEffectTemplate;

	//Settings
	bool bIsDisabledForPlayer = false;
	bool bIsAlwaysEnchantable = false;
	bool bCanBeUsedByMobs = true;

	bool bHideFromInspector = false;

	bool bWorkInProgress = false;

	int mSoulGatherAmount = 0;

	game::FDifficulty DifficultyThreshold = game::FDifficulty::LOWEST;

	FName mRelativeClassPath;
	FName mRelativeIconPath;
	FName mRelativeMaterialPath;
};
