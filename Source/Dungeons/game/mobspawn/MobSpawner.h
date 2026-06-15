#pragma once

#include "MobSpawnTypes.h"

class UWorld;

namespace game { namespace mobspawn {
enum MobSpawnPriority
{
	ESpawnPriority_High = 0,
	ESpawnPriority_Medium,
	ESpawnPriority_Low,
	ESpawnPriority_END
};
using MobSpawnCallback = std::function< void(AMobCharacter*) >;
using MobGroupSpawnCallback = std::function< void(TArray<AMobCharacter*>&) >;
	

AMobCharacter*         startSpawnMobActor(UWorld& world, UClass* mobClass, EntityType type, const FTransform& Transform, const Config& config);
void                   endSpawnMobActor(AMobCharacter* pMob, const FTransform& Transform, const TArray<FEnchantmentData>& enchantmentData, const Config& config);
AMobCharacter*         spawnNow(UWorld& world, EntityType type, const TransformProvider& transformProvider, const Config& config);
bool                   getSpawnData(UWorld& world, const TransformProvider& transformProvider, const Config& config, EntityType& type_IN_OUT, FTransform& MobTransform_OUT);
bool                   getGroupSpawnData(UWorld& world, const SpawnGroup& group, const TransformProvider& transformProvider, const Config& config, EntityType& type_OUT, TArray<FTransform>& MobTransforms_OUT);
TArray<AMobCharacter*> spawnGroupNow(UWorld&, const SpawnGroup&, const TransformProvider&, const Config&);
void                   spawnGroupAsync(UWorld&, const SpawnGroup&, const TransformProvider&, const Config&, MobSpawnCallback, MobGroupSpawnCallback, MobSpawnPriority);
void                   spawnAsync(UWorld&, EntityType, const TransformProvider&, const Config&, MobSpawnCallback, MobSpawnPriority);

void				   applyEnchantmentsTo(const TArray<FEnchantmentData>& enchantments, AActor& actor);
void				   applyGroupLoot(const TArray<AMobCharacter*>& mobs, float dropChance);

}}
