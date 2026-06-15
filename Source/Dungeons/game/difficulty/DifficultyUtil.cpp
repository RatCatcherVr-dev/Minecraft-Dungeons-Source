#include "Dungeons.h"
#include "DifficultyUtil.h"
#include "DungeonsGameInstance.h"
#include "Difficulty.h"
#include "game/util/ValueFormat.h"

int UDifficultyUtil::GetStartingLivesForDifficulty(EGameDifficulty Difficulty) {
	return difficultyquery::getLivesForDifficulty(Difficulty);
}

FText UDifficultyUtil::getDifficultyDisplayName(EGameDifficulty Difficulty){
	return difficultyquery::getDifficultyDisplayName(Difficulty);
}

#define LOCTEXT_NAMESPACE "Difficulty"

FText UDifficultyUtil::getDifficultyPlusDisplayName(EGameDifficulty Difficulty) {
	return FText::Format(INVTEXT("{0}+"), getDifficultyDisplayName(Difficulty));
}

FText UDifficultyUtil::getThreatLevelDisplayName(EThreatLevel ThreatLevel) {
	switch (ThreatLevel) {
	case EThreatLevel::Threat_1:
		return INVTEXT("I");
	case EThreatLevel::Threat_2:
		return INVTEXT("II");
	case EThreatLevel::Threat_3:
		return INVTEXT("III");
	case EThreatLevel::Threat_4:
		return INVTEXT("IV");
	case EThreatLevel::Threat_5:
		return INVTEXT("V");
	case EThreatLevel::Threat_6:
		return INVTEXT("VI");
	case EThreatLevel::Threat_7:
		return INVTEXT("VII");
	default:
		checkNoEntry();
		return FText::GetEmpty();
	}
}

FText UDifficultyUtil::getThreatCompletedRequirementText(EThreatLevel ThreatLevel) {
	return FText::Format(LOCTEXT("threat_unlock_requirement", "Complete any mission on at least {0}"), getDifficultyThreatLevelDisplayName(EGameDifficulty::Difficulty_1, ThreatLevel));
}

FText UDifficultyUtil::getEndlessStruggleDisplayName(FEndlessStruggle EndlessStruggle) {
	return FText::Format(INVTEXT("+{0}"), valueformat::asNumber(EndlessStruggle.Value));
}

FText UDifficultyUtil::getDifficultyThreatLevelDisplayName(EGameDifficulty Difficulty, EThreatLevel ThreatLevel) {
	return FText::Format(INVTEXT("{0} {1}"), getDifficultyDisplayName(Difficulty), getThreatLevelDisplayName(ThreatLevel));
}

FText UDifficultyUtil::getDifficultyEndlessStruggleDisplayName(EGameDifficulty Difficulty, FEndlessStruggle EndlessStruggle) {
	return FText::Format(INVTEXT("{0}{1}"), getDifficultyDisplayName(Difficulty), getEndlessStruggleDisplayName(EndlessStruggle));
}

FText UDifficultyUtil::GetUnlockText(EGameDifficulty Difficulty)
{
	switch (Difficulty) {
	case EGameDifficulty::Difficulty_2:
	case EGameDifficulty::Difficulty_3:
	{
		const EGameDifficulty previousDifficulty = difficultyquery::getPreviousDifficulty(Difficulty).Get(EGameDifficulty::Invalid);
		return FText::Format(LOCTEXT("difficulty_unlock_text", "Defeat the Arch-Illager on {0} difficulty to unlock"), UDifficultyUtil::getDifficultyDisplayName(previousDifficulty));
	}
	default:
		return FText::GetEmpty();
	}
}

FText UDifficultyUtil::GetUnlockRequirementText(EGameDifficulty Difficulty)
{
	switch (Difficulty) {
	case EGameDifficulty::Difficulty_2:
	case EGameDifficulty::Difficulty_3:
	{
		const EGameDifficulty previousDifficulty = difficultyquery::getPreviousDifficulty(Difficulty).Get(EGameDifficulty::Invalid);
		return FText::Format(LOCTEXT("difficulty_unlock_requirement_text", "Defeat the Arch-Illager on {0} difficulty"), UDifficultyUtil::getDifficultyDisplayName(previousDifficulty));
	}
	default:
		return FText::GetEmpty();
	}
}

FText UDifficultyUtil::GetUnlockExplainerText(EGameDifficulty Difficulty) {
	switch (Difficulty) {
	case EGameDifficulty::Difficulty_2:
	{
		const EGameDifficulty previousDifficulty = difficultyquery::getPreviousDifficulty(Difficulty).Get(EGameDifficulty::Invalid);
		return FText::Format(LOCTEXT("difficulty_unlock_explainermissions_text", "Complete the mainland missions to reach and defeat the Arch-Illager on {0} difficulty."), UDifficultyUtil::getDifficultyDisplayName(previousDifficulty));
	}
	case EGameDifficulty::Difficulty_3:
	{
		const EGameDifficulty previousDifficulty = difficultyquery::getPreviousDifficulty(Difficulty).Get(EGameDifficulty::Invalid);
		return FText::Format(LOCTEXT("difficulty_unlock_explainersimple_text", "Defeat the Arch-Illager on {0} difficulty."), UDifficultyUtil::getDifficultyDisplayName(previousDifficulty));
	}
	default:
		return FText::GetEmpty();
	}
}

#undef LOCTEXT_NAMESPACE

FText UDifficultyUtil::GetExtraChallengeDispalyName(EExtraChallenge Challenge) {
	return extrachallengequery::getExtraChallengeDisplayName(Challenge);
}

FEndlessStruggleConfiguration UDifficultyUtil::GetEndlessStruggleConfiguration( FEndlessStruggle endlessStruggle ) {
	if( GWorld ) {
		if( auto instance = GWorld->GetGameInstance<UDungeonsGameInstance>() ) {
			if( auto library = instance->GetEndlessStruggleLibrary() ) {
				return library->GetConfiguration( endlessStruggle.Value );
			}
		}
	}
	return FEndlessStruggleConfiguration();
}

int UDifficultyUtil::GetNumThreatLevelsInDifficulty(EGameDifficulty Difficulty) {
	return difficultyquery::getNumThreatLevels(Difficulty);
}

int UDifficultyUtil::GetNumEndlessStrugglesInDifficulty(EGameDifficulty Difficulty) {
	return difficultyquery::getNumEndlessStruggles(Difficulty);
}

int UDifficultyUtil::GetNumThreatLevelsAndEndlessStrugglesInDifficulty(EGameDifficulty Difficulty) {
	return GetNumThreatLevelsInDifficulty(Difficulty) + GetNumEndlessStrugglesInDifficulty(Difficulty);
}

int UDifficultyUtil::GetTotalNumThreatLevels() {
	return NumberOfThreatLevels;
}

int UDifficultyUtil::GetTotalNumEndlessStruggles() {
	return NumberOfEndlessStruggleLevels;
}

int UDifficultyUtil::GetTotalNumThreatLevelsAndEndlessStruggles() {
	return GetTotalNumThreatLevels() + GetTotalNumEndlessStruggles();
}

game::FDifficulty UDifficultyUtil::GetDifficultyWithEmergentDifficulty(const game::FDifficulty& difficulty, TOptional<FEmergentDifficulty> EmergentDifficulty)
{
	return game::FDifficulty(difficulty.chosen(), difficulty.threatLevel(), difficulty.extraChallenge(), difficulty.endlessStruggle(), EmergentDifficulty.Get(FEmergentDifficulty()));
}

