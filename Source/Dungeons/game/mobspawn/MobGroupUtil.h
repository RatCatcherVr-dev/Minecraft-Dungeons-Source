#pragma once

#include "MobSpawnTypes.h"

enum class EntityType : unsigned int;

namespace io {
struct MobGroup;
struct StretchMobs;
struct Level;
}

namespace game {
struct FDifficulty;
struct DifficultyStats;

namespace mobspawn {

void prepareMobGroup(io::MobGroup&);
void buildPredicates(io::StretchMobs&);
std::vector<EntityType> evaluateMobExpr(const std::string&);

TArray<EntityType> calculateMobsWithoutDifficultyLimitation(const std::vector<io::MobGroup>&, int count, Random* = nullptr);
TArray<EntityType> calculateMobs(const std::vector<io::MobGroup>&, int count, const FDifficulty&, Random* = nullptr);
TArray<SpawnGroup> calculateMobsWithEnchantments(const std::vector<io::MobGroup>&, int count, const DifficultyStats&, Random* = nullptr);
TArray<SpawnGroup> calculateObjectiveWaveMobsWithEnchantments(const std::vector<io::MobGroup>&, int count, const DifficultyStats&, Random* = nullptr);

io::MobGroup sampleWithProbabilities(const std::vector<EntityType>&, std::initializer_list<float> probabilities, float restProbability = 0);

}}
