// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "MobLoaderComponent.h"
#include "game/GameTypes.h"
#include "game/level/GameLevelDef.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/mission/MissionDef.h"
#include "Engine/AssetManager.h"
#include "game/Enchantments/mobs/MobEnchantmentConfigs.h"
#include "game/mobspawn/alpha/HyperSpawner.h"
#include "game/mobspawn/event/EventMobSpawner.h"
#include "game/actor/DarkSpawner.h"
#include "lovika/RegionPredicates.h"
#include "world/entity/MobTags.h"
#include "game/affector/AffectorTypesUtil.h"

UMobLoaderComponent::UMobLoaderComponent() {
	PrimaryComponentTick.bCanEverTick = false;

	AdditionalTypesToLoad.Add(EntityType::NamelessKing, {EntityType::FalseKing});
	AdditionalTypesToLoad.Add(EntityType::Necromancer, { EntityType::Zombie, EntityType::BabyZombie, EntityType::Husk, EntityType::ChickenJockey, EntityType::Skeleton});
	AdditionalTypesToLoad.Add(EntityType::NecromancerAncient, { EntityType::Zombie, EntityType::BabyZombie, EntityType::Husk, EntityType::ChickenJockey, EntityType::Skeleton});
	AdditionalTypesToLoad.Add(EntityType::ArchIllager, {EntityType::PillagerVariant0});
	AdditionalTypesToLoad.Add(EntityType::ArchVessel, {EntityType::ArchVisage});
	AdditionalTypesToLoad.Add(EntityType::CauldronBoss, {EntityType::SlimeCauldron});
	AdditionalTypesToLoad.Add(EntityType::SlimeLarge, {EntityType::SlimeMedium, EntityType::SlimeSmall});
	AdditionalTypesToLoad.Add(EntityType::Geomancer, {EntityType::GeomancerBomb, EntityType::GeomancerWall});
	AdditionalTypesToLoad.Add(EntityType::GeomancerAncient, {EntityType::GeomancerBomb, EntityType::GeomancerWall});
	AdditionalTypesToLoad.Add(EntityType::Illusioner, {EntityType::IllusionerClone});
	AdditionalTypesToLoad.Add(EntityType::JungleAbomination, {EntityType::Whisperer, EntityType::Leaper, EntityType::AbominationVine});
	AdditionalTypesToLoad.Add(EntityType::OrdinaryHorse, {EntityType::SkeletonHorseman});
	AdditionalTypesToLoad.Add(EntityType::RedstoneMonstrosity, {EntityType::RedstoneCube});
	AdditionalTypesToLoad.Add(EntityType::Whisperer, {EntityType::EntangleVine, EntityType::QuickGrowingVineSimple, EntityType::PoisonQuillVineSimple});
	AdditionalTypesToLoad.Add(EntityType::WickedWraith, {EntityType::Stray, EntityType::Wraith});
	AdditionalTypesToLoad.Add(EntityType::ChickenJockey, {EntityType::ChickenJockeyTower});
}

TSet<EntityType> UMobLoaderComponent::ExtractMissionMobs() const {
	const auto* gameBP = GetGame();
	const auto* game = gameBP->GetGame();
	const auto& levelDef = gameBP->GetLevelDef();
	
	TSet<EntityType> ExtractedTypes;
	
	//Hyperspawner mobs
	if (const auto* mHyperSpawner = game->hyperSpawner()) {
		for (const auto& mobGroup : mHyperSpawner->mobGroups()) {
			ExtractedTypes.Append(ExtractEntitiesFromGroup(mobGroup));
		}
	}

	//Normal mob groups which cover tiles and objectives
	for (auto& mobGroup : levelDef->levelDef.data.mobGroups) {
		game::mobspawn::prepareMobGroup(mobGroup);
		ExtractedTypes.Append(ExtractEntitiesFromGroup(mobGroup));
	}

	//Special hack to find keys
	if (algo::any_of(levelDef->levelDef.data.objectives, RETLAMBDA(it.click && it.click->keyType))) {
		ExtractedTypes.Add(EntityType::GoldBabyKey);
		ExtractedTypes.Add(EntityType::SilverBabyKey);
	}

	//Special hack for vines
	if (algo::any_of(game->tiles().getTiles(), RETLAMBDA(it->tilePlacement().hasRegion(regionpredicates::isFixedMob())))) {
		ExtractedTypes.Add(EntityType::QuickGrowingVine);
		ExtractedTypes.Add(EntityType::EntangleVine);
	}

	//Extract event mobs
	if (game->missionDef().isHyperMission()) {
		algo::append_all(ExtractedTypes, game::mobspawn::eventmob::getAllHyperMissionEventMobCandidates());
	}
	else if (auto mobGroup = game->missionDef().getEventMobGroup()) {
		game::mobspawn::prepareMobGroup(mobGroup.GetValue());
		ExtractedTypes.Append(ExtractEntitiesFromGroup(mobGroup.GetValue()));
	}

	const auto affectorExtractor = [&] (const affector::AffectorInstance* instance) -> TOptional<EntityType> {
		if (instance) {
			if (auto replaceMobTypeInfo = affector::parse::replaceMobTypeInfo(instance->Data)) {
				return game::mobspawn::NoVariants()(replaceMobTypeInfo->target);
			}
		}
		return {};
	};
	
	if (auto melee = affectorExtractor(game->affectors().Find(affector::ReplaceMeleeMobsType))) {
		ExtractedTypes.Add(melee.GetValue());
	}

	//Find affector replacement mobs:
	if (auto ranged = affectorExtractor(game->affectors().Find(affector::ReplaceMeleeMobsType))) {
		ExtractedTypes.Add(ranged.GetValue());
	}
	
	return ExtractedTypes;
}

TSharedPtr<FStreamableHandle> UMobLoaderComponent::AsyncLoadMobs(const TSet<EntityType>& mobs, FStreamableDelegate DelegateToCall, TAsyncLoadPriority Priority) {
	//We do no early out/removal of already loaded mobs here. The rationale is that the extra overhead incurred by resolving already loaded objects again is fairly small and happens rarely.
	return AsyncLoadMobsExact(AddAdditionalTypesToLoad(mobs), DelegateToCall, Priority);
}

TSharedPtr<FStreamableHandle> UMobLoaderComponent::AsyncLoadMobsExact(const TSet<EntityType>& mobsToLoad, FStreamableDelegate DelegateToCall, TAsyncLoadPriority Priority) {

	auto& typemap = game::TypeMap::singleton();
	const TArray<FSoftObjectPath> paths = algo::map_if_tarray(mobsToLoad.Array(),
		RETLAMBDA(typemap.PathForEntity(it)),
		RETLAMBDA(it.IsValid()));
	
	return UAssetManager::GetStreamableManager().RequestAsyncLoad(paths, FStreamableDelegate::CreateUObject(this, &UMobLoaderComponent::OnMobsLoaded, mobsToLoad, DelegateToCall), Priority);
}

bool UMobLoaderComponent::HasLoadedMob(EntityType type) const {
	return GetClass(type) != nullptr;
}

UClass* UMobLoaderComponent::GetClass(EntityType type) const {
	if(auto* entry = LoadedMobBPs.Find(type)) {
		return *entry;
	}

	return nullptr;
}

TSet<EntityType> UMobLoaderComponent::ExtractEntitiesFromGroup(const io::MobGroup& mobGroup) {
	TSet<EntityType> ExtractedTypes;

	for(const auto& type : mobGroup.types) {
		ensureMsgf(type.types, TEXT("Attempting to extract entities from an unprepared mobgroup. This will result in no mobs being extracted."));
		if(type.types) {
			for(const auto& entityType : type.types.GetValue()) {
				ExtractedTypes.Append(game::mobspawn::AllVariants(entityType));
			}	
		}
	}

	return ExtractedTypes;
}

TSet<EntityType> UMobLoaderComponent::AddAdditionalTypesToLoad(const TSet<EntityType>& mobs) const {
	TSet<EntityType> expandedMobs(mobs);

 	int size;
	do {
		TArray<EntityType> types = expandedMobs.Array();
		size = types.Num();

		for(auto type : types) {
			if(AdditionalTypesToLoad.Contains(type)) {
				expandedMobs.Append(AdditionalTypesToLoad[type]);
			}
		}
	} while(expandedMobs.Num() != size);

	return expandedMobs;
}

AGameBP* UMobLoaderComponent::GetGame() const {
	return Cast<AGameBP>(GetOwner());
}

void UMobLoaderComponent::OnMobsLoaded(TSet<EntityType> types, FStreamableDelegate delegate) {
	auto& typemap = game::TypeMap::singleton();
	
	for(const auto type : types) {
		if(auto* cls = Cast<UClass>(typemap.PathForEntity(type).ResolveObject())) {
			LoadedMobBPs.Add(type, cls);
			//Cache value in typemap as well
			game::TypeMap::singleton().mobClass(type);
		}
	}

	delegate.ExecuteIfBound();
}
