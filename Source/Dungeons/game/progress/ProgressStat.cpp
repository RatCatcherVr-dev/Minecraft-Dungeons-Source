#include "Dungeons.h"
#include "ProgressStat.h"
#include "game/util/ValueFormat.h"
#include "game/mob/MobTypeDefs.h"
#include "game/difficulty/DifficultyUtil.h"
#include "world/entity/MobTags.h"
#include "util/Algo.h"
#include "game/difficulty/endless/EndlessStruggleTiers.h"

#define LOCTEXT_NAMESPACE "ProgressStat"


const FText UProgressStatUtil::MissionsWonAtDifficultyTemplate = LOCTEXT("WIN_MISSIONS_DIFFICULTY", "Missions Won on {0} difficulty");
const FText UProgressStatUtil::SuccessfullyCompleteMissionsAtDifficultyTemplateOne = LOCTEXT("WIN_MISSIONS_DIFFICULTY_desc_one", "Successfully complete any mission on {0} difficulty or higher");
const FText UProgressStatUtil::SuccessfullyCompleteMissionsAtDifficultyTemplateMany = LOCTEXT("WIN_MISSIONS_DIFFICULTY_desc_many", "Successfully complete missions on {0} difficulty or higher");

FText UProgressStatUtil::GetProgressStatText(EProgressStat stat) {
	switch (stat) {
	case EProgressStat::WIN_MISSIONS:
		return LOCTEXT("WIN_MISSIONS", "Missions Won");
	case EProgressStat::WIN_HYPERMISSIONS:
		return LOCTEXT("WIN_HYPERMISSIONS", "Ancient Hunts Won");
	case EProgressStat::WIN_MISSIONS_DEFAULT:
		return FText::Format(MissionsWonAtDifficultyTemplate, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_1));
	case EProgressStat::WIN_MISSIONS_ADVENTURE:		
		return FText::Format(MissionsWonAtDifficultyTemplate, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_2));
	case EProgressStat::WIN_MISSIONS_APOCALYPSE:
		return FText::Format(MissionsWonAtDifficultyTemplate, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_3));	
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS:
		return FText::Format(MissionsWonAtDifficultyTemplate, UDifficultyUtil::getDifficultyPlusDisplayName(EGameDifficulty::Difficulty_3));
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER0:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER1:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER2:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER3:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER4:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER5:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER6:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER7:
		return FText::Format(MissionsWonAtDifficultyTemplate, UDifficultyUtil::getDifficultyEndlessStruggleDisplayName(EGameDifficulty::Difficulty_3, endlesstruggle::tier::getTierFromStat(stat)->unlockedEndlessStruggle()));
	case EProgressStat::GIVE_GIFTS:
		return LOCTEXT("GIVE_GIFTS", "Gifts Given");
	case EProgressStat::DEFEAT_ENCHANTED_MOBS:
		return LOCTEXT("DEFEAT_ENCHANTED_MOBS", "Enchanted Mobs Defeated");
	case EProgressStat::DEFEAT_EVENT_MOBS:
		return LOCTEXT("DEFEAT_EVENT_MOBS", "Powerful Mobs Defeated");
	default:
		return FText::GetEmpty();
	}
}

FText UProgressStatUtil::GetProgressStatExplainerText(EProgressStat stat, int count) {
	switch (stat) {
	case EProgressStat::WIN_MISSIONS:
		{
			if(count == 1){
				return LOCTEXT("WIN_MISSIONS_desc_one", "Successfully complete any mission.");
			} else {
				return LOCTEXT("WIN_MISSIONS_desc_many", "Successfully complete missions.");
			}
		}
	case EProgressStat::WIN_HYPERMISSIONS:
		{
			if (count == 1) {
				return LOCTEXT("WIN_HYPERMISSIONS_desc_one", "Successfully complete an ancient hunt.");
			}
			else {
				return LOCTEXT("WIN_HYPERMISSIONS_desc_many", "Successfully complete ancient hunts.");
			}
		}
	case EProgressStat::WIN_MISSIONS_DEFAULT:
		return FText::Format(count == 1 ? SuccessfullyCompleteMissionsAtDifficultyTemplateOne : SuccessfullyCompleteMissionsAtDifficultyTemplateMany, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_1));
	case EProgressStat::WIN_MISSIONS_ADVENTURE:
		return FText::Format(count == 1 ? SuccessfullyCompleteMissionsAtDifficultyTemplateOne : SuccessfullyCompleteMissionsAtDifficultyTemplateMany, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_2));
	case EProgressStat::WIN_MISSIONS_APOCALYPSE:
		return FText::Format(count == 1 ? SuccessfullyCompleteMissionsAtDifficultyTemplateOne : SuccessfullyCompleteMissionsAtDifficultyTemplateMany, UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty::Difficulty_3));
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER0:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER1:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER2:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER3:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER4:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER5:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER6:
	case EProgressStat::WIN_MISSIONS_APOCALYPSE_PLUS_TIER7:
		return FText::Format(count == 1 ? SuccessfullyCompleteMissionsAtDifficultyTemplateOne : SuccessfullyCompleteMissionsAtDifficultyTemplateMany, UDifficultyUtil::getDifficultyPlusDisplayName(EGameDifficulty::Difficulty_3));
	case EProgressStat::GIVE_GIFTS:
		{
			if (count == 1) {
				return LOCTEXT("GIVE_GIFTS_desc_one", "Give a Gift to another player in multiplayer.");
			} else {
				return LOCTEXT("GIVE_GIFTS_desc_many", "Give Gifts to other players in multiplayer.");
			}
		}
	case EProgressStat::DEFEAT_ENCHANTED_MOBS:
		{
			if (count == 1) {
				return LOCTEXT("DEFEAT_ENCHANTED_MOBS_desc_one", "Defeat an enchanted mob, which is more common on higher difficulties.");
			} else {
				return LOCTEXT("DEFEAT_ENCHANTED_MOBS_desc_many", "Defeat enchanted mobs, which are more common on higher difficulties.");
			}
		}		
	case EProgressStat::DEFEAT_EVENT_MOBS:
		{
			auto powerfulMobTypes = algo::copy_if(mob::type::getAllEnabled(), RETLAMBDA(hasMobTag(it, MobTags::HashTag_EventMob)));
			auto listOfMobNames = valueformat::asCommaSeparatedOrList(algo::map_as<TArray<FText>>(powerfulMobTypes, RETLAMBDA(mob::type::get(it).GetName())));
			if (count == 1) {
				return FText::Format(LOCTEXT("DEFEAT_EVENT_MOBS_desc_one", "Defeat a powerful mob such as the {0}."), listOfMobNames);
			} else {
				return FText::Format(LOCTEXT("DEFEAT_EVENT_MOBS_desc_many", "Defeat powerful mobs, such as the {0}."), listOfMobNames);
			}
		}
		{
		}
	default:
		return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE
