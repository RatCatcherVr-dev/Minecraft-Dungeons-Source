/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include <string>

UENUM()
enum class EntityType : uint32 {
	Undefined = 1,

	// Mobs up to 63, rest on top of that
	ItemEntity = 64,
	PrimedTnt = 65,
	FallingBlock = 66,
	MovingBlock = 67,
	ExperiencePotion = 68,
	Experience = 69,
	EyeOfEnder = 70,
	EnderCrystal = 71,
	ShulkerBullet = 76,
	FishingHook = 77,
	Chalkboard = 78, // Legacy, used for converting old Chalkboards to BlockEntities

	DragonFireball = 79,
	Arrow = 80,
	Snowball = 81,
	ThrownEgg = 82,
	Painting = 83,
	LargeFireball = 85,	//used to be "Fireball", which has pure virtual members, and thus could not exist in a savegame
	ThrownPotion = 86,
	Enderpearl = 87,
	LeashKnot = 88,
	WitherSkull = 89,

	// (NEW ID's) D5: I hope this isn't in use in the future!!!
	BoatRideable = 90,

	WitherSkullDangerous = 91,

	LightningBolt = 93,
	SmallFireball = 94,
	AreaEffectCloud = 95,

	//	@Note: Minecarts occupy 84 & 96 - 99

	LingeringPotion = 101,

	// Mob types (Category, maybe different Enum to avoid clashes)
	// NOTE: Because of legacy serialization, mob type values may not overlap (base type bits are ignored)
	TypeMask = 0x000001ff,	// supports up to 511 different entity ids

	Mob = 0x00000200,
	Monster = 0x00000800 | Mob,	// safely casted to (Monster*), NOTE: Different from Java
															// project (example Slime.java vs Slime.h)
	Animal = 0x00001000 | Mob,		// safely casted to (Animal*)
	WaterAnimal = 0x00002000 | Mob,
	TamableAnimal = 0x00004000 | Animal,
	Ambient = 0x00008000 | Mob,				// safely casted to (AmbientCreature*)
	UndeadMob = 0x00010000 | Monster,
	ZombieMonster = 0x00020000 | UndeadMob,
	Arthropod = 0x00040000 | Monster,

	Minecart = 0x00080000,

	SkeletonMonster = 0x00100000 | UndeadMob,
	EquineAnimal = 0x00200000 | TamableAnimal,	// horse type

	Chicken = 10 | Animal,
	Cow = 11 | Animal,
	Pig = 12 | Animal,
	Sheep = 13 | Animal,
	Wolf = 14,
	Villager = 15 | Mob,
	Mooshroom = 16,
	MooshroomAncient = 270,
	Rabbit = 18,
	Bat = 19,
	IronGolem = 20 | Mob,
	SnowGolem = 21 | Mob,
	Horse = 23 | EquineAnimal,

	//These are temporary and only used for mob spawner items
	Donkey = 24 | EquineAnimal,
	Mule = 25 | EquineAnimal,
	SkeletonHorse = 26 | EquineAnimal | UndeadMob,
	ZombieHorse = 27 | EquineAnimal | UndeadMob,

	RedstoneGolem = 28 | Monster,
	RedstoneGolemAncient = 260 | Monster,
	Enchanter = 29 | Monster,
	EnchanterAncient = 500 | Mob,
	Vindicator = 30 | Monster,
	VindicatorVariant0 = 125 | Monster,
	VindicatorVariant1 = 126 | Monster,
	VindicatorVariant2 = 127 | Monster,

	// note that type Monster == (Monster | PathfinderMob | Mob)
	BabyZombie = 31 | ZombieMonster,
	BabyDrowned = 324 | ZombieMonster,
	BabyZombieAncient = 190 | Mob,
	Zombie = 32 | ZombieMonster,
	ZombieVariant0 = 110 | ZombieMonster,
	ZombieVariant1 = 111 | ZombieMonster,
	ZombieVariant2 = 112 | ZombieMonster,
	ZombieAncient = 113 | ZombieMonster,
	Creeper = 33 | Monster,
	Skeleton = 34 | SkeletonMonster,
	SkeletonVariant0 = 115 | SkeletonMonster,
	SkeletonVariant1 = 116 | SkeletonMonster,
	SkeletonVariant2 = 117 | SkeletonMonster,
	SkeletonAncient = 118 | SkeletonMonster,
	Spider = 35 | Arthropod,
	SpiderAncient = 280 | Arthropod,
	ZombifiedPiglin = 36 | UndeadMob,
	Silverfish = 39 | Arthropod,
	SilverfishAncient = 499 | Arthropod,
	CaveSpider = 40 | Arthropod,
	Ghast = 41 | Monster,

	Blaze = 43 | Monster,
	ZombieVillager = 44 | ZombieMonster,
	Witch = 45 | Monster,
	WitchAncient = 150 | Mob,

	//These are temporary and only used for mob spawner items
	Stray = 46 | SkeletonMonster,
	Husk = 47 | ZombieMonster,
	WitherSkeleton = 48 | SkeletonMonster,
	WitherSkeletonRanged = 262 | SkeletonMonster,
	PiglinFungusThrower = 261 |Monster,
	ZombifiedPiglinFungusThrower = 263 | Monster,
	Guardian = 49 | Monster,
	GuardianAncient = 487 | Monster,
	ElderGuardian = 50 | Monster,

	Npc = 51 | Mob,
	WitherBoss = 52 | UndeadMob,
	Dragon = 53 | Monster,
	Shulker = 54 | Monster,
	Endermite = 55 | Arthropod,
	Agent = 56 | Mob,

	Necromancer = 57 | SkeletonMonster,
	NecromancerAncient = 140 | Mob,
	TheOneWhoCalls = 141 | Necromancer,
	Vex = 58 | Monster,
	VexAncient = 160 | Mob,
	Evoker = 59 | Monster,
	EvokerFang = 60 | Monster,
	Geomancer = 61 | Monster,
	GeomancerAncient = 530 | Monster,

	TripodCamera = 62 | Mob,
	Player = 63 | Mob,

	GeomancerWall = 64 | Mob, // mob?
	GeomancerBomb = 65 | Mob, // mob?
	ChickenJockey = 66 | ZombieMonster,
	ChickenJockeyTower = 100 | ZombieMonster,
	ChickenJockeyTowerAncient = 250 | Mob,

	SlimeLarge = 67 | Monster,
	SlimeMedium = 68 | Monster,
	SlimeSmall = 69 | Monster,
	SlimeSmallAncient = 510 | Monster,

	RedstoneMonstrosity = 70 | Monster,
	Wraith = 71 | SkeletonMonster,
	WraithAncient = 180 | Mob,
	RedstoneCube = 72 | Monster,
	PiggyBank = 73 | Mob,
	GoldBabyKey = 74 | Mob,
	SilverBabyKey = 75 | Mob,
	NamelessKing = 76 | SkeletonMonster,
	Pillager = 77 | Monster,
	PillagerVariant0 = 120 | Monster,
	PillagerVariant1 = 121 | Monster,
	PillagerVariant2 = 122 | Monster,
	PillagerAncient = 123 | Monster,
	FalseKing = 78 | SkeletonMonster,
	SkeletonVanguard = 79 | SkeletonMonster,
	SkeletonVanguardAncient = 290 | Mob,
	MobSpawner = 80 | Monster,
	SkeletonHorseman = 81 | Monster | UndeadMob,
	OrdinaryHorse = 82 | Monster,
	CauldronBoss = 83 | Monster,
	SlimeCauldron = 84 | Monster,
	ArchIllager = 85 | Monster,
	ArchVessel = 128 | Monster,
	ArchVisage = 129 | Monster,
	JackOLantern = 86 | Monster,
	RoyalGuard = 87 | Monster,
	RoyalGuardAncient = 170 | Mob,
	VindicatorChef = 89 | Monster,
	Llama = 90 | Monster,
	SoulWizard = 226 | Monster,
	Enderman = 92 | Monster,

	MinecartHopper = 96 | Minecart,
	MinecartTNT = 97 | Minecart,
	MinecartChest = 98 | Minecart,
	MinecartFurnace = 99 | Minecart,
	MooshroomMonstrosity = 131 | Monster,
	BabyChicken = 218,
	ChargedCreeper = 219 | Monster,
	BabyPig = 232,

	Dummy = 132,

	// D11 Mobs;
	JungleZombie = 200 | ZombieMonster,
	MossySkeleton = 201 | SkeletonMonster,
	QuickGrowingVine = 202 | Mob,
	PoisonQuillVine = 203 | Mob,
	Ocelot = 204,
	PlayfulPanda = 205,
	LazyPanda = 206,
	BrownPanda = 207,
	Leaper = 208,
	LeaperAncient = 520,
	Whisperer = 209,
	WaveWhisperer = 327,
	PoisonAnemone = 328,
	QuickGrowingKelp = 329,
	JungleAbomination = 210,
	QuickGrowingVineSimple = 211,
	PoisonQuillVineSimple = 212,
	EntangleVine = 213,
	AbominationVine = 214,
	BabyPanda = 215,
	Parrot = 216,
	Panda = 217,

	FrozenZombie = 220 | ZombieMonster,
	IcyCreeper = 221 | Monster,
	PolarBear = 222,
	ArcticFox = 223,
	Illusioner = 224,
	IllusionerClone = 225,
	Chillager = 226,
	WickedWraith = 227,

	Bee = 228,
	SheepFireRed = 229,
	SheepPoisonGreen = 230,
	SheepSpeedBlue = 231,

	Goat = 199,
	GoatAncient = 540,
	Ravager = 233,
	WoolyCow = 234,
	LlamaMob = 235,
	WindCaller = 237,
	BabyGoat = 238,
	SquallGolem = 239,
	Mountaineer = 240,
	MountaineerVariant0 = 241,
	MountaineerVariant1 = 242,
	MountaineerVariant2 = 243,

	TempestGolem = 244,
	RampartCaptain = 245,

	BlazeSpawner = 249 | Monster,
	BlazeSpawnerAncient = 550 | Monster,
	HoveringInferno = 250 | Monster,
	Hoglin = 246,
	HoglinAncient = 560,

	PiglinMelee = 251,
	PiglinMeleeVariant0 = 252,
	PiglinMeleeVariant1 = 253,
	PiglinMeleeVariant2 = 254,
	PiglinRanged = 255,
	PiglinRangedVariant0 = 256,
	PiglinRangedVariant1 = 257,
	PiglinRangedVariant2 = 258,

	ZombifiedPiglinMelee = 259 | UndeadMob,
	ZombifiedPiglinMeleeVariant0 = 260 | UndeadMob,
	ZombifiedPiglinMeleeVariant1 = 261 | UndeadMob,
	ZombifiedPiglinMeleeVariant2 = 262 | UndeadMob,
	ZombifiedPiglinRanged = 263 | UndeadMob,
	ZombifiedPiglinRangedVariant0 = 264 | UndeadMob,
	ZombifiedPiglinRangedVariant1 = 265 | UndeadMob,
	ZombifiedPiglinRangedVariant2 = 266 | UndeadMob,
	BabyGhast = 267,

	MagmaCubeLarge = 280 | Mob,
	MagmaCubeMedium = 281 | Mob,
	MagmaCubeSmall = 282 | Mob,

	TropicalSlimeLarge = 300 | Monster,
	TropicalSlimeMedium = 301 | Monster,
	TropicalSlimeSmall = 302 | Monster,
	Squid = 303 | WaterAnimal,
	Turtle = 304 | WaterAnimal,
	BabyTurtle = 305 | WaterAnimal,
	Pufferfish = 306 | WaterAnimal,
	Drowned = 307 | ZombieMonster,
	DrownedAncient = 491 | ZombieMonster,
	Dolphin = 308 | WaterAnimal,
	TridentDrowned = 309 | ZombieMonster,
	SunkenSkeleton = 310 | SkeletonMonster,
	DrownedNecromancer = 311 | SkeletonMonster,
	BabyTurtlePet = 312,
	VindicatorRaidCaptain = 313 | Monster,
	PillagerRaidCaptain = 314 | Monster,
	GlowSquid = 315 | WaterAnimal,
	AncientGuardian = 316,
	AncientGuardianMine = 317,
	Biomine = 318,
	Blastling = 319 | Mob,
	Snareling = 320 | Mob,
	SnarelingAncient = 503 | Mob,
	Endling = 321 | Mob,
	EndlingAncient = 509 | Mob,	
	EndermiteSmart = 322 | Mob,
	SilverfishSmart = 323 | Mob,
	Endersent = 324 | Mob,
	EndersentThornblight = 325 | Mob,
	EndersentVoidstrike = 326 | Mob,
	EndersentWritherot = 327 | Mob,
	EndersentEverfire = 328 | Mob,
	EndersentSurgefiend = 329 | Mob,
	EndersentDeadeye = 330 | Mob,
	Seravex = 331 | Mob,
	PerfectFormHeart = 333 | Monster,
	EndermiteMinion = 332 | Arthropod,
	DrownedVariant0 = 334 | ZombieMonster,
	DrownedVariant1 = 335 | ZombieMonster,
	DrownedVariant2 = 336 | ZombieMonster,
	TridentDrownedVariant0 = 337 | ZombieMonster,
	TridentDrownedVariant1 = 338 | ZombieMonster,
	TridentDrownedVariant2 = 339 | ZombieMonster,
	SunkenSkeletonVariant0 = 340 | SkeletonMonster,
	SunkenSkeletonVariant1 = 341 | SkeletonMonster,
	SunkenSkeletonVariant2 = 342 | SkeletonMonster,
	Friendermite = 343,
	ShulkerRespawning = 344| Monster,

	ZombifiedBabyPig = 355,
};

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

enum class EntityTypeNamespaceRules : int {
	ReturnWithoutNamespace,
	ReturnWithNamespace,
};

struct EntityMapping {
	std::string mNamespace;
	std::string mPrimaryName;
	std::string mAlternateName;

	EntityMapping(const std::string& primary, const std::string& alt = "")
		: EntityMapping("minecraft", primary, alt)
	{}

	EntityMapping(const std::string& space, const std::string& primary, const std::string& alt)
		: mNamespace(space)
		, mPrimaryName(primary)
		, mAlternateName(alt)
	{}

	bool operator==(const EntityMapping& other) {
		return mNamespace == other.mNamespace
			&& mPrimaryName == other.mPrimaryName
			&& mAlternateName == other.mAlternateName;
	}

	size_t operator()(EntityMapping const& key) const {
		size_t seed = 0;
		seed = Math::hash_accumulate(seed, key.mNamespace);
		seed = Math::hash_accumulate(seed, key.mPrimaryName);
		seed = Math::hash_accumulate(seed, key.mAlternateName);
		return seed;
	}

	inline std::string getMappingName(EntityTypeNamespaceRules namespaceRule) const {
		if (namespaceRule == EntityTypeNamespaceRules::ReturnWithNamespace) {
			return mNamespace + ":" + mPrimaryName;
		} else {
			return mPrimaryName;
		}
	}
};

enum_bitwise_operators(EntityType, int);

// String conversion
std::string EntityTypeToString(EntityType entityType, EntityTypeNamespaceRules namespaceRule = EntityTypeNamespaceRules::ReturnWithoutNamespace);
std::vector<std::string> EntityTypeStrings(EntityType);

EntityType EntityTypeFromString(const std::string& fromString);
TOptional<EntityType> MaybeEntityTypeFromString(const std::string&);

std::string EntityTypeResolveAlias(const std::string& fromString, EntityTypeNamespaceRules namespaceRule = EntityTypeNamespaceRules::ReturnWithNamespace);
