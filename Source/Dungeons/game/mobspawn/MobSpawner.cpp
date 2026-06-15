#include "Dungeons.h"
#include "MobSpawner.h"
#include "MobSpawnTypes.h"
#include "MobGroupUtil.h"
#include "EntityTypeMappers.h"
#include "game/GameTypes.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/drop/MobGroupDropComponent.h"
#include "game/Enchantments/mobs/MobEnchantmentTypes.h"
#include "game/GameBP.h"
#include <Engine/World.h>

namespace game { namespace mobspawn {

void applyEnchantmentsTo(const TArray<FEnchantmentData>& enchantments, AActor& actor) {
	if (auto enchantmentComp = actor.FindComponentByClass<UEnchantmentComponent>()) {
		enchantmentComp->AddEnchantments(enchantments, { &actor });
	}
}

void applyGroupLoot(const TArray<AMobCharacter*>& mobs, float dropChance) {
	// @note: We could have sent in only the non-garbage collected mobs, but
	//        this provides useful data about how many mobs that were once
	//        included in this group. We could also have used a second param,
	//        but we already had this full array at both call sites.
	const auto nonGarbageCollectedMobs = algo::copy_if(mobs, RETLAMBDA(it));
	if (nonGarbageCollectedMobs.Num() == 0) {
		return;
	}

	TSharedPtr<FMobDropGroup> dropGroup = MakeShared<FMobDropGroup>(nonGarbageCollectedMobs);

	for (auto* mob : nonGarbageCollectedMobs) {
		auto* dropComponent = UMobGroupDropComponent::Create(mob, EItemRarityChanceCategory::EnchantedMobGroup, EDropCategory::Gear, dropChance);
		dropComponent->SetMobGroup(dropGroup);
		dropComponent->RegisterComponent();
	}
}

TOptional<FTransform> findTransform(UWorld& world, EntityType type, const TransformProvider& transformProvider, const Config& config) {
	for (int i = 0; i < config.tries; ++i) {
		if (auto maybe = config.positionCorrector(world, type, transformProvider())) {
			return maybe;
		}
	}
	return {};
}

AMobCharacter* startSpawnMobActor(UWorld& world, UClass* mobClass, EntityType type, const FTransform& Transform, const Config& config)
{
	if (auto mob = world.SpawnActorDeferred<AMobCharacter>(mobClass, Transform, nullptr, nullptr, config.adjustPositionIfPossible ? ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn : ESpawnActorCollisionHandlingMethod::AlwaysSpawn)) {
		mob->EntityType = type;
		mob->SetActorLocation(Transform.GetLocation() + FVector::UpVector * (mob->FindComponentByClass<UCapsuleComponent>()->GetScaledCapsuleHalfHeight() + 2), false);

		for (const auto& action : config.actions) { action(*mob); }

		UE_LOG(LogDungeons, Log, TEXT("MobSpawn:: startSpawnMobActor: Mob spawned deferred: %s"), *FString(EntityTypeToString(type).c_str()));
		return mob;
	}
	return nullptr;
}

void endSpawnMobActor(AMobCharacter* pMob, const FTransform& Transform, const TArray<FEnchantmentData>& enchantmentData, const Config& config)
{
	if (pMob)
	{
		UGameplayStatics::FinishSpawningActor(pMob, Transform);
		UE_LOG(LogDungeons, Log, TEXT("MobSpawn:: endSpawnMobActor: Finish mob spawn: %s"), *FString(EntityTypeToString(pMob->EntityType).c_str()));
		if (enchantmentData.Num() > 0) {
			game::mobspawn::applyEnchantmentsTo(enchantmentData, *pMob);
		}

		for (const auto& action : config.postSpawnActions) { action(*pMob); }
	}
}

AMobCharacter* spawnNow(UWorld& world, EntityType type, const TransformProvider& transformProvider, const Config& config) {
	FTransform MobTransform;

	if (!getSpawnData(world, transformProvider, config, type,  MobTransform))
	{
		return nullptr;
	}

	UClass* pMobClass = TypeMap::singleton().mobClass(type);
	
	if (AMobCharacter* pMob = startSpawnMobActor(world, pMobClass, type, MobTransform, config))
	{
		endSpawnMobActor(pMob, MobTransform, {}, config);
		return pMob;
	}
	
	return nullptr;
}

bool getSpawnData(UWorld& world, const TransformProvider& transformProvider, const Config& config, EntityType& type_IN_OUT, FTransform& MobTransform_OUT)
{
	static const EntityTypeMapper removeVariantBaseNames = NoVariants();

	type_IN_OUT = removeVariantBaseNames(config.typeMapper(type_IN_OUT));
	if (!TypeMap::singleton().IsMapped(type_IN_OUT))
	{
		return false;
	}
	auto transform = findTransform(world, type_IN_OUT, transformProvider, config);
	if (!transform) 
	{
		return false;
	}
	
	MobTransform_OUT = transform.GetValue();
	return true;
}


bool getGroupSpawnData(UWorld& world, const SpawnGroup& group, const TransformProvider& transformProvider, const Config& config, EntityType& type_OUT, TArray<FTransform>& MobTransforms_OUT)
{
	static const EntityTypeMapper removeVariantBaseNames = NoVariants();

	MobTransforms_OUT.Reset();
	type_OUT = removeVariantBaseNames(config.typeMapper(group.type));
	if (!TypeMap::singleton().IsMapped(type_OUT))
	{
		return false;
	}

	for (size_t i = 0; i < group.count; ++i) 
	{
		const auto& thisTransformProvider = config.perMobInGroupTransforms ? config.perMobInGroupTransforms(i) : transformProvider;

		auto transform = findTransform(world, type_OUT, thisTransformProvider, config);
		if (transform)
		{
			MobTransforms_OUT.Push(transform.GetValue());
		}
	}

	return true;
}

TArray<AMobCharacter*> spawnGroupNow(UWorld& world, const SpawnGroup& group, const TransformProvider& transform, const Config& config) {
	TArray<AMobCharacter*> out;
	for (size_t i = 0; i < group.count; ++i) {
		const auto& transformProvider = config.perMobInGroupTransforms ? config.perMobInGroupTransforms(i) : transform;
		if (auto mob = spawnNow(world, group.type, transformProvider, config)) {
			applyEnchantmentsTo(group.enchantmentData, *mob);
			out.Add(mob);
		}
	}
	if (group.hasGroupLoot()) {
		applyGroupLoot(out, group.groupLootDropChance);
	}
	return out;
}

void spawnGroupAsync(UWorld& world, const SpawnGroup& group, const TransformProvider& transform, const Config& config, MobSpawnCallback mobCallback, MobGroupSpawnCallback groupCallback, MobSpawnPriority prio) {
	if(auto* game = actorquery::getFirstActor<AGameBP>(&world)) {
		game->RequestMobGroupSpawn(group, transform, config, groupCallback, mobCallback, prio);
	} else {
		auto mobs = spawnGroupNow(world, group, transform, config);
		algo::for_each(mobs, mobCallback);
		groupCallback(mobs);
	}
}

void spawnAsync(UWorld& world, EntityType type, const TransformProvider& transformProvider, const Config& config, MobSpawnCallback callback, MobSpawnPriority prio) {
	if(auto* game = actorquery::getFirstActor<AGameBP>(&world)) {
		game->RequestMobSpawn(type, transformProvider, {}, config, callback, prio);
	} else {
		auto mob = spawnNow(world, type, transformProvider, config);
		if(mob) {
			callback(mob);
		}
	}
}

}}
