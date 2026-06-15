#include "Dungeons.h"
#include "BehaviorFactory.h"
#include "BehaviorFactoryWip.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "world/entity/EntityClassTree.h"
#include "entities/CreeperBehavior.h"
#include "entities/AnimalBehavior.h"
#include "entities/BabyZombieBehavior.h"
#include "entities/SkeletonBehavior.h"
#include "entities/SpiderBehavior.h"
#include "entities/VexBehavior.h"
#include "entities/ZombieBehavior.h"
#include "entities/NecromancerBehavior.h"
#include "entities/RedstoneGolemBehavior.h"
#include "entities/RedstoneMonstrosityBehavior.h"
#include "entities/EnchanterBehavior.h"
#include "entities/EnchanterMultiBehavior.h"
#include "entities/EvokerBehavior.h"
#include "entities/EvokerFangBehavior.h"
#include "entities/GeomancerBehavior.h"
#include "entities/GeomancerWallBehavior.h"
#include "entities/GeomancerBombBehavior.h"
#include "entities/WraithBehavior.h"
#include "entities/PiggyBankBehavior.h"
#include "entities/BabyKeyBehavior.h"
#include "entities/WitchBehavior.h"
#include "entities/NamelessKingBehavior.h"
#include "entities/FalseKingBehavior.h"
#include "entities/VanguardBehavior.h"
#include "entities/WolfBehavior.h"
#include "entities/PetBehavior.h"
#include "entities/SheepRedBehavior.h"
#include "entities/MobSpawnerBehavior.h"
#include "entities/BlazeSpawnerBehavior.h"
#include "entities/SkeletonHorsemanBehavior.h"
#include "entities/OrdinaryHorseBehavior.h"
#include "entities/CauldronBossBehavior.h"
#include "entities/SlimeCauldronBehavior.h"
#include "entities/VindicatorBehavior.h"
#include "entities/ArchIllagerBehavior.h"
#include "entities/ArchVesselBehavior.h"
#include "entities/JackOLanternBehavior.h"
#include "entities/LlamaBehavior.h"
#include "entities/EndermanBehavior.h"
#include "entities/MagmaCubeBehavior.h"
#include "entities/BlazeBehavior.h"
#include "entities/GhastBehavior.h"
#include "entities/VineBehavior.h" // D11.DB
#include "entities/PoisonVineBehavior.h" // D11.DB
#include "entities/EntangleBehavior.h" // D11.DB
#include "entities/AbominationVineBehavior.h" // D11.DB
#include "entities/LeaperBehavior.h" // D11.DB
#include "entities/WhispererBehavior.h" // D11.DB
#include "entities/JungleAbominationBehavior.h" // D11.DB
#include "entities/PandaBehavior.h" // D11.DB
#include "entities/OcelotBehavior.h" // D11.CH
#include "entities/FrozenZombieBehavior.h" // DB.CH
#include "entities/PolarBearBehavior.h" // DB.CH
#include "entities/GoatBehavior.h" // D11.RS
#include "entities/GoatAncientBehavior.h"
#include "entities/RavagerBehavior.h" // D11.RS
#include "entities/MountaineerBehaviour.h" // D11.TT
#include "entities/WindCallerBehaviour.h" // D11.TT
#include "entities/SquallGolemBehavior.h" // D11.TT
#include "entities/TempestGolemBehaviour.h" // D11.TT
#include "entities/IllusionerBehavior.h" // DB.DB
#include "entities/ChillagerBehavior.h" // D11.CM
#include "entities/WickedWraithBehavior.h" // D11.DB
#include "entities/SquidBehavior.h" // D11.RS
#include "entities/GuardianBehavior.h" // D11.RS
#include "entities/PufferfishBehavior.h" // D11.RS
#include "entities/AncientGuardianBehavior.h" // D11.TT
#include "entities/BlastlingBehavior.h"
#include "entities/SnarelingBehavior.h"
#include "entities/EndlingBehavior.h"
#include "entities/ShulkerBehavior.h"
#include "entities/EndermiteBehavior.h" // D11.DJB
#include "entities/PerfectFormHeartBehavior.h" //D11.DJB
#include "entities/BiomineBehavior.h" // D11.TT
#include "game/component/SplitComponent.h"
#include "game/component/PickupComponent.h"
#include "game/component/HealthDisplayComponent.h"
#include "game/component/GrowAttackComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/actor/HorsemenTargetProvider.h"
#include "game/component/drop/ItemDropComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "entities/BeeBehavior.h"
#include "entities/HoglinBehavior.h"
#include "entities/FrostWardenBehavior.h"
#include "entities/DrownedBehavior.h"
#include "entities/TridentDrownedBehavior.h"
#include "entities/DolphinBehavior.h"
#include "entities/TurtleBehavior.h"
#include "entities/PiglinRangedBehavior.h"
#include "entities/EndersentBehavior.h"
#include "entities/DrownedNecromancerBehavior.h"

namespace bt { namespace behavior {

EntityType getEntityTypeFromClassPath(const std::string& path) {
	auto end = path.rfind('/');
	auto start = (end != std::string::npos) ? path.rfind('/', end - 1) : end;

	if (start == std::string::npos) {
		return EntityType::Undefined;
	}
	auto revisionIndex = path.find("_Revision");
	if (revisionIndex != std::string::npos) {
		end = revisionIndex;
	}
	auto typeName = Util::toLower(path.substr(start + 1, end - start - 1));
	return EntityTypeFromString(typeName);
}

EntityType getEntityType(AMobCharacter& mob) {
	if (mob.EntityType != EntityType::Undefined) {
		return mob.EntityType;
	}
	EntityType entityTypeFromClassPath = getEntityTypeFromClassPath(TCHAR_TO_UTF8(*mob.GetClass()->GetPathName()));
	if (entityTypeFromClassPath != EntityType::Undefined) {
		return entityTypeFromClassPath;
	}

	FString name = mob.GetClass()->GetName();

	if (name.Contains("necromancer")) {
		return EntityType::Necromancer;
	}

	return EntityType::Undefined;
}

void decorateMob(AMobCharacter& mob) {
	UHealthComponent* HealthComponent = mob.GetHealthComponent();
	if (!HealthComponent) {	HealthComponent = mob.FindComponentByClass<UHealthComponent>();	}

	UAbilitySystemComponent* abilitySystem = mob.GetAbilitySystemComponent();

	if (const auto growAttackComponent = mob.FindComponentByClass<UGrowAttackComponent>()) {
		HealthComponent->OnDeath.AddUObject(growAttackComponent, &UGrowAttackComponent::Disenchant);
		HealthComponent->OnDamageReceived.AddUObject(growAttackComponent, &UGrowAttackComponent::HandleDamage);
	}

	if (const auto splitComponent = mob.FindComponentByClass<USplitComponent>()) {
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(splitComponent, &USplitComponent::OnAttributeHealthChange);
	}

	if (const auto pickupComponent = mob.FindComponentByClass<UPickupComponent>()) {
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(pickupComponent, &UPickupComponent::OnAttributeHealthChange);
	}

	if (const auto healthDisplayComponent = mob.FindComponentByClass<UHealthDisplayComponent>()) {
		abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(healthDisplayComponent, &UHealthDisplayComponent::OnAttributeHealthChange);
	}

	if (mob.EntityType == EntityType::SkeletonHorseman) {
		if (auto targetProvider = actorquery::getFirstActor<AHorsemenTargetProvider>(mob.GetWorld())) {
			abilitySystem->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::HealthAttribute()).AddUObject(targetProvider, &AHorsemenTargetProvider::OnAttributeHealthChange);
		}
	}
}

BehaviorTuple createBehaviorFor(AMobCharacter& mob) {
	const auto entityType = getEntityType(mob);
	mob.EntityType = entityType;
	decorateMob(mob);
	return createBehaviorForAs(mob, entityType);
}

BehaviorTuple createBehaviorForAs(AMobCharacter& mob, EntityType entityType) {
	if (auto wipBehavior = internal::wip_createBehaviorForAs(mob, entityType)) {
		return wipBehavior;
	}

	const auto options = mob.FindComponentByClass<UBehaviorOptionsComponent>();

	switch (entityType) {
	case EntityType::ChickenJockey:
	case EntityType::ChickenJockeyTower:
	case EntityType::BabyZombie:  
	case EntityType::BabyDrowned:
	case EntityType::BabyZombieAncient:
	case EntityType::ChickenJockeyTowerAncient:		return entities::createBabyZombie(mob);
	case EntityType::ChargedCreeper:
	case EntityType::Creeper:       return entities::createCreeper(mob, *options);
	case EntityType::NamelessKing:  return entities::createNamelessKing(mob, *options);
	case EntityType::FalseKing:		return entities::createFalseKing(mob, *options);
	case EntityType::Necromancer:   
	case EntityType::NecromancerAncient:
	case EntityType::TheOneWhoCalls:return entities::createNecromancer(mob, *options);
	case EntityType::DrownedNecromancer: return entities::createDrownedNecromancer(mob, *options);
	case EntityType::Evoker:        return entities::createEvoker(mob, *options);
	case EntityType::EvokerFang:    return entities::createEvokerFang(mob, *options);
	case EntityType::PiglinRangedVariant0:
	case EntityType::PiglinRangedVariant1:
	case EntityType::PiglinRangedVariant2: return entities::createPiglinRanged(mob, *options);
	case EntityType::ZombifiedPiglinRangedVariant0:
	case EntityType::ZombifiedPiglinRangedVariant1:
	case EntityType::ZombifiedPiglinRangedVariant2:
	case EntityType::Pillager:
	case EntityType::PillagerVariant0:
	case EntityType::PillagerVariant1:
	case EntityType::PillagerVariant2:
	case EntityType::PillagerAncient:
	case EntityType::WitherSkeletonRanged:
	case EntityType::Skeleton:
	case EntityType::SkeletonVariant0:
	case EntityType::SkeletonVariant1:
	case EntityType::SkeletonVariant2:
	case EntityType::SkeletonAncient: return entities::createSkeleton(mob, *options);
	case EntityType::PiglinFungusThrower:
	case EntityType::ZombifiedPiglinFungusThrower:
	case EntityType::Witch:         
	case EntityType::WitchAncient: return entities::createWitch(mob, *options);
	case EntityType::Spider: 
	case EntityType::SpiderAncient: return entities::createSpider(mob, *options);
	case EntityType::SkeletonVanguard:
	case EntityType::SkeletonVanguardAncient:
	case EntityType::RoyalGuard:
	case EntityType::RoyalGuardAncient: return entities::createVanguard(mob, *options);
	case EntityType::ArchIllager:   return entities::createArchIllager(mob, *options);
	case EntityType::ArchVessel:    return entities::createArchVessel(mob, *options);
	case EntityType::HoglinAncient: 
	case EntityType::Hoglin:		return entities::createHoglin(mob, *options);
	case EntityType::CaveSpider:
	case EntityType::RedstoneCube:
	case EntityType::SlimeLarge:
	case EntityType::SlimeMedium:
	case EntityType::SlimeSmall:
	case EntityType::SlimeSmallAncient:
	case EntityType::Mooshroom:
	case EntityType::MooshroomAncient:
	case EntityType::Husk:
	case EntityType::PiglinMeleeVariant0:
	case EntityType::PiglinMeleeVariant1:
	case EntityType::PiglinMeleeVariant2:
	case EntityType::ZombifiedPiglinMeleeVariant0:
	case EntityType::ZombifiedPiglinMeleeVariant1:
	case EntityType::ZombifiedPiglinMeleeVariant2:
	case EntityType::Zombie:
	case EntityType::ZombieVariant0:
	case EntityType::ZombieVariant1:
	case EntityType::ZombieVariant2:
	case EntityType::ZombieAncient:  return entities::createZombie(mob, *options);
	case EntityType::MagmaCubeLarge:
	case EntityType::MagmaCubeMedium:
	case EntityType::MagmaCubeSmall: return entities::createMagmaCube(mob, *options);
	case EntityType::Enderman:      return entities::createEnderman(mob, *options);
	case EntityType::Blaze: return entities::createBlaze(mob, *options);
	case EntityType::Ghast:	return entities::createGhast(mob, *options);
	case EntityType::HoveringInferno:			return entities::createSkeleton(mob, *options);
	case EntityType::BlazeSpawner:			
	case EntityType::BlazeSpawnerAncient: return entities::createBlazeSpawner(mob, *options);
	case EntityType::WitherSkeleton:
	case EntityType::Vindicator:
	case EntityType::VindicatorVariant0:
	case EntityType::VindicatorVariant1:
	case EntityType::VindicatorVariant2:	
	case EntityType::VindicatorChef:
	case EntityType::RampartCaptain: return entities::createVindicator(mob, *options);
	case EntityType::Vex:			
	case EntityType::VexAncient: return entities::createVex(mob);
	case EntityType::RedstoneGolem: 
	case EntityType::RedstoneGolemAncient: return entities::createRedstoneGolem(mob, *options);
	case EntityType::MooshroomMonstrosity:
	case EntityType::RedstoneMonstrosity: return entities::createRedstoneMonstrosity(mob, *options);
	case EntityType::Enchanter:     return entities::createEnchanter(mob, *options);
	case EntityType::EnchanterAncient: return entities::createEnchanterMulti(mob, *options);
	case EntityType::Geomancer:     return entities::createGeomancer(mob, *options);
	case EntityType::GeomancerAncient:   return entities::createFrostWarden(mob, *options);
	case EntityType::GeomancerWall: return entities::createGeomancerWall(mob, *options);
	case EntityType::GeomancerBomb: return entities::createGeomancerBomb(mob);
	case EntityType::Wraith:		
	case EntityType::WraithAncient:	return entities::createWraith(mob, *options);
	case EntityType::PiggyBank:		return entities::createPiggyBank(mob, *options);
	case EntityType::GoldBabyKey:
	case EntityType::SilverBabyKey: return entities::createBabyKey(mob, *options);
	case EntityType::Llama:         return entities::createLlama(mob, *options);
	case EntityType::SoulWizard:         return entities::createLlama(mob, *options);
	case EntityType::IronGolem:
	case EntityType::SheepPoisonGreen:
	case EntityType::SheepSpeedBlue:
	case EntityType::Wolf:          return entities::createWolf(mob, *options);
	case EntityType::SheepFireRed:  return entities::createSheepRed(mob, *options);
	case EntityType::MobSpawner: return entities::createMobSpawner(mob, *options);
	case EntityType::SkeletonHorseman: return entities::createSkeletonHorseman(mob, *options);
	case EntityType::OrdinaryHorse: return entities::createOrdinaryHorse(mob, *options);
	case EntityType::CauldronBoss: return entities::createCauldronBoss(mob, *options);
	case EntityType::JackOLantern: return entities::createJackOLantern(mob, *options);
	case EntityType::SlimeCauldron: return entities::createSlimeCauldron(mob, *options);
	case EntityType::Bat: return entities::createWolf(mob, *options);
	case EntityType::Bee: return entities::createBee(mob, *options);
	case EntityType::ArchVisage: return entities::createGeomancerWall(mob, *options);
	case EntityType::BabyChicken: return entities::createPet(mob, *options);
	case EntityType::BabyPig: return entities::createPet(mob, *options);
	case EntityType::BabyGhast: return entities::createPet(mob, *options);

	// D11.DB - D11 MOBS
    case EntityType::JungleZombie: return entities::createZombie(mob, *options);
    case EntityType::MossySkeleton: return entities::createSkeleton(mob, *options);
	case EntityType::QuickGrowingVine: return entities::createVine(mob, *options);
	case EntityType::QuickGrowingVineSimple: return entities::createVineSimple( mob, *options);
	case EntityType::QuickGrowingKelp: return entities::createVineSimple(mob, *options);
	case EntityType::PoisonQuillVine: return entities::createPoisonVine(mob, *options);
	case EntityType::PoisonQuillVineSimple: return entities::createPoisonVineSimple( mob, *options);
	case EntityType::PoisonAnemone: return entities::createPoisonVineSimple(mob, *options);
	case EntityType::EntangleVine: return entities::createEntangleVine(mob, *options);
    case EntityType::Leaper: 
	case EntityType::LeaperAncient: return entities::createLeaper(mob, *options);
	case EntityType::Whisperer: return entities::createWhisperer(mob, *options);
	case EntityType::WaveWhisperer: return entities::createWhisperer(mob, *options);
	case EntityType::JungleAbomination: return entities::createJungleAbomination(mob, *options);
	case EntityType::AbominationVine: return entities::createAbominationVine(mob, *options);
	case EntityType::Panda:
    case EntityType::PlayfulPanda:
    case EntityType::LazyPanda:
    case EntityType::BabyPanda:
	case EntityType::BrownPanda: return entities::createPanda( mob, *options );
	case EntityType::Parrot: return entities::createPet(mob, *options);
	case EntityType::Ocelot: return entities::createOcelot(mob, *options);

	case EntityType::FrozenZombie: return entities::createFrozenZombie(mob, *options); // D11.CH
	case EntityType::IcyCreeper: return entities::createCreeper(mob, *options); // D11.CH
	case EntityType::Rabbit: return entities::createDefaultAnimal(mob);
	case EntityType::Stray: return entities::createSkeleton(mob, *options);
	case EntityType::PolarBear: return entities::createPolarBear(mob, *options);
	case EntityType::BabyGoat: return entities::createPet(mob, *options); // D11.TT
	case EntityType::GoatAncient: return entities::createGoatAncient(mob, *options);
	case EntityType::Goat: return entities::createGoat(mob, *options); // D11.RS
	case EntityType::Ravager: return entities::createRavager(mob, *options); // D11.RS
	case EntityType::Mountaineer:
	case EntityType::MountaineerVariant0:
	case EntityType::MountaineerVariant1:
	case EntityType::MountaineerVariant2: return entities::createMountaineer(mob, *options); // D11.TT
	case EntityType::WindCaller: return entities::createWindCaller(mob, *options); // D11.TT
	case EntityType::SquallGolem: return entities::createSquallGolem(mob, *options); //D11.TT
	case EntityType::TempestGolem: return entities::createTempestGolem(mob, *options); // D11.TT
	case EntityType::WoolyCow: return entities::createDefaultAnimal(mob);
	case EntityType::LlamaMob: return entities::createLlamaMob(mob, *options);
	// D11.CH - END
    case EntityType::Illusioner: return entities::createIllusioner(mob, *options);
    case EntityType::IllusionerClone: return entities::createIllusioner(mob, *options, true);
	case EntityType::WickedWraith: return entities::createWickedWraith(mob, *options);
	// D11.DB - D11 MOBS END

	// #D11.CM - Start
	case EntityType::ArcticFox: return entities::createPet(mob, *options);
	case EntityType::Chillager: return entities::createChillager(mob, *options);
	// #D11.CM - End

	// D11.RS
	case EntityType::TropicalSlimeLarge:
	case EntityType::TropicalSlimeMedium:
	case EntityType::TropicalSlimeSmall: return entities::createZombie(mob, *options);
	case EntityType::Squid: return entities::createSquid(mob, *options);
	case EntityType::Dolphin: return entities::createDolphin(mob, *options);
	case EntityType::Turtle: return entities::createTurtle(mob, *options);
	case EntityType::BabyTurtle: return entities::createTurtle(mob, *options);
	case EntityType::Pufferfish: return entities::createPufferfish(mob, *options);
	case EntityType::Drowned: return entities::createDrowned(mob, *options);
	case EntityType::DrownedAncient: return entities::createDrowned(mob, *options);
	case EntityType::DrownedVariant0: return entities::createDrowned(mob, *options);
	case EntityType::DrownedVariant1: return entities::createDrowned(mob, *options);
	case EntityType::DrownedVariant2: return entities::createDrowned(mob, *options);
	case EntityType::TridentDrowned: return  entities::createTridentDrowned(mob, *options);
	case EntityType::TridentDrownedVariant0: return  entities::createTridentDrowned(mob, *options);
	case EntityType::TridentDrownedVariant1: return  entities::createTridentDrowned(mob, *options);
	case EntityType::TridentDrownedVariant2: return  entities::createTridentDrowned(mob, *options);
	case EntityType::Guardian: return  entities::createGuardian(mob, *options);
	case EntityType::GuardianAncient:	return entities::createGuardian(mob, *options);
	case EntityType::ElderGuardian: return  entities::createGuardian(mob, *options);
	case EntityType::SunkenSkeleton: return  entities::createSkeleton(mob, *options);
	case EntityType::SunkenSkeletonVariant0: return  entities::createSkeleton(mob, *options);
	case EntityType::SunkenSkeletonVariant1: return  entities::createSkeleton(mob, *options);
	case EntityType::SunkenSkeletonVariant2: return  entities::createSkeleton(mob, *options);
	case EntityType::BabyTurtlePet: return  entities::createPet(mob, *options);
	case EntityType::GlowSquid: return entities::createSquid(mob, *options);
	case EntityType::AncientGuardian: return  entities::createAncientGuardian(mob, *options);
	case EntityType::Biomine: return  entities::createBiomine(mob, *options);
	// D11.RS - End

	// D11.AS

	case EntityType::VindicatorRaidCaptain: return entities::createMountaineer(mob, *options);
	case EntityType::PillagerRaidCaptain: return entities::createSkeleton(mob, *options);

	// D11.AS - END


	// D11.RS - End

	// DLC6
	case EntityType::Blastling: return entities::createBlastling(mob, *options);
	case EntityType::Snareling: 
	case EntityType::SnarelingAncient: return entities::createSnareling(mob, *options);
	case EntityType::Endling: 
	case EntityType::EndlingAncient: return entities::createEndling(mob, *options);
	case EntityType::Shulker: return entities::createShulker(mob, *options);
	case EntityType::ShulkerRespawning: return entities::createShulker(mob, *options);
	case EntityType::Silverfish:
	case EntityType::SilverfishAncient: return entities::createEndermite(mob, *options);
	case EntityType::SilverfishSmart: return entities::createEndermite(mob, *options, true);
	case EntityType::EndermiteSmart: return entities::createEndermite(mob, *options, true);
	case EntityType::Endersent: 
	case EntityType::EndersentThornblight:
	case EntityType::EndersentVoidstrike:
	case EntityType::EndersentWritherot:
	case EntityType::EndersentEverfire:
	case EntityType::EndersentSurgefiend:
	case EntityType::EndersentDeadeye: return entities::createEndersent(mob, *options);
	case EntityType::Seravex: return entities::createWolf(mob, *options, false);
	case EntityType::Endermite: return entities::createEndermite(mob, *options);
	case EntityType::EndermiteMinion: return entities::createEndermite(mob, *options, false, true);
	case EntityType::PerfectFormHeart: return entities::createPerfectFormHeart(mob, *options);
	case EntityType::Friendermite: return entities::createPet(mob, *options);
	// DLC6 - END


	// SPOOKY FALL 2
	case EntityType::ZombifiedBabyPig: return entities::createPet(mob, *options);

	// SPOOKY FALL 2 - End


	default:
		if (EntityType::Animal == EntityClassTree::getMobCategory(entityType)) {
			return entities::createDefaultAnimal(mob);
		}
	}

	return{};
}

}}
