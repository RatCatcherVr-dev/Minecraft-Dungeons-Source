#pragma once

#include "game/item/enchantment/EnchantmentData.h"

class AMobCharacter;
class UWorld;
enum class EntityType : uint32;

namespace game { namespace mobspawn {

struct SpawnGroup {
	SpawnGroup(EntityType, size_t count);
	SpawnGroup(EntityType, size_t count, TArray<FEnchantmentData>, float dropChance);

	void disenchant();

	bool hasGroupLoot() const;

	TArray<FEnchantmentData> enchantmentData;
	EntityType type;
	size_t count;
	float groupLootDropChance;
private:
};


//
// Transform providers
//
// Can be built from fixed values, or a combination of position,
// rotation and scale providers
//
using TransformProvider = std::function<FTransform()>;
// Position provider types for different coordinate systems
using BlockPositionProvider = std::function<Vec3()>;
using UePositionProvider = std::function<FVector()>;
// Rotation provider types
using YawProvider = std::function<float()>;
// Scale provider types
using ScaleProvider = std::function<FVector()>;


//
// Configuration for the mob spawner
//
using PositionCorrector = std::function<TOptional<FTransform>(UWorld&, EntityType, FTransform)>;
using EntityTypeMapper = std::function<EntityType(EntityType)>;
using MobAction = std::function<void(AMobCharacter&)>;

struct Config {
	EntityTypeMapper typeMapper;
	std::vector<MobAction> actions;
	std::vector<MobAction> postSpawnActions;
	PositionCorrector positionCorrector;
	std::function<TransformProvider(size_t)> perMobInGroupTransforms; // index of mob in mob group
	int tries = 10;
	bool adjustPositionIfPossible = true;
};

}}
