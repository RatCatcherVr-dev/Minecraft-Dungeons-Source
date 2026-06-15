#pragma once

#include "game/LevelSettings.h"
#include "game/mission/state/MissionState.h"
#include "lovika/io/IoHyperTypes.h"

struct FItemBulletPoint;
struct FEligibleDLC;
struct ItemArchetypeCounts;

enum class ELevelNames : uint8;

namespace missions { namespace hajper {

struct AncientProbability {
	float probability;
	float probabilityMultiplier() const;
};

AncientProbability getAncientProbability(int ancientDungeonsCount, int sacrificedEnchantmentPoints);
AncientProbability getAncientProbability(const FMissionState&);
float getApproximateChanceForAnyAncient(int submissionsLeft, int ancientDungeonsTypes, int sacrificedEnchantmentPoints);
float getApproximateChanceForAnyAncient(const FMissionState&);
float getApproximateAverageAmountOfAncients(int submissionsLeft, int ancientDungeonsTypes, int sacrificedEnchantmentPoints);
float getApproximateAverageAmountOfAncients(const FMissionState&);

int getNumAncientMobTypes(ELevelNames, const FEligibleDLC&, const ItemArchetypeCounts&);
int getNumAncientMobTypes(const FMissionState&);

int getAncientHuntNumSubMissions();
int getAncientHuntNumSubMissionsRemaining(const FMissionState&);

int getAncientHuntNumLives(const FMissionState&);
int getAncientHuntNumLivesRemaining(const FMissionState&);

bool isAncientHuntDepleted(const FMissionState&);

void populateMissionBulletPoints(const FMissionState&, TArray<FItemBulletPoint>&);
void populateMissionProbabilities(const FMissionState&, TArray<FMissionProbability>&);

bool shouldIncludeInLevelGeneration(DungeonEligibility);
bool shouldIncludeInUI(DungeonEligibility);

io::HyperDungeonPredicate getHyperDungeonLevelGenerationPredicate(const FEligibleDLC&, const ItemArchetypeCounts&);
io::HyperDungeonPredicate getHyperDungeonLevelGenerationPredicate(const FMissionState&);

}}
