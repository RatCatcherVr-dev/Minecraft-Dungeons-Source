#include "Dungeons.h"
#include "AwardsGeneratorComponent.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/util/ValueFormat.h"
#include "DungeonsGameMode.h"
#include <Runtime/Engine/Classes/GameFramework/PlayerState.h>


#include "util/DungeonsPlayerStatTracker.h"
#include "util/telemetry/Analytics.h"
#include "game/Game.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"


UAwardsGeneratorComponent::UAwardsGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//D11.PS moved this into a function
int UAwardsGeneratorComponent::GetWeightedIndex(const std::vector<int32> &weights, std::mt19937 &randGen) const
{
	//D11.PS see https://stackoverflow.com/questions/21959404/initialising-stddiscrete-distribution-in-vs2013
	std::size_t d(0);
	std::discrete_distribution<size_t> dist(weights.size(),
		0.0, // dummy!
		0.0, // dummy!
		[&weights, &d](double)
	{
		auto w = weights[d];
		++d;
		return w;
	});

	return dist(randGen);
}

TArray<FGameStatsStruct> UAwardsGeneratorComponent::ProcessGameStats() const {
	ADungeonsGameMode* gamemode = Cast<ADungeonsGameMode>(GetOwner());
	check(gamemode);

	TArray<FGameStatsStruct> result;

	// copy for picking stats
	auto trackedStats = gamemode->GetGameStatTracker().GetTrackedStats();
	std::vector<int32> weights(trackedStats.size());
	std::random_device rd;
	std::mt19937 gen(rd());

	// size of tracked stats reserve
	result.Reserve(trackedStats.size());

	for (int i = 0; i < weights.size(); ++i) {
		weights[i] = StatsWeights.Contains(trackedStats[i].Type) ? StatsWeights[trackedStats[i].Type] : DefaultWeight;
	}

	// TODO: get ONLY the COUPLED stats for output (percentage optional)
	for (int i = 0; i< NumberGameStatsDisplayed; ++i)
	{
		//D11.PS - moved to a function
		int weightedIndex = GetWeightedIndex(weights, gen);

		const DungeonsGameStatTracker::DungeonsGameStatPair& st = trackedStats[weightedIndex];

		FGameStatsStruct ss;
		// todo: add better descriptive LOCALIZED output
		ss.Name = DungeonsGameStatTracker::toText(st.Type);
		ss.Id = DungeonsGameStatTracker::toString(st.Type);

		ss.Count = st.GetDisplayCount();
		ss.Total = st.GetDisplayTotal();
		
		ss.HasPercentage = st.CountTotal > 0;
		ss.Percentage = st.GetDisplayPercentageText();

		trackedStats.erase(trackedStats.begin() + weightedIndex);
		weights.erase(weights.begin() + weightedIndex);

		result.Add(ss);

		// out if out of stats
		if (trackedStats.empty())
			break;

	}	

	return result;
}


TArray<FAwardStruct> UAwardsGeneratorComponent::GenerateAwards(ELevelNames level, EGameDifficulty difficulty) const {
	ADungeonsGameMode* gamemode = Cast<ADungeonsGameMode>(GetOwner());
	check(gamemode);

	TArray<FAwardStruct> result;
	result.Reserve(gamemode->GetNumPlayers());

	awards::AwardResult awardResult = awards::GenerateAwards(gamemode->GetPlayerTrackers(), AwardThresholds, level, difficulty);
	
	// telemetry
	for (const auto& award : awardResult.all) {
		analytics::Analytics::GetInstance().FireEventRoundAwards(award);
	}

	std::random_device rd;
	std::mt19937 gen(rd());

	auto& playerAwards = awardResult.best;
	while (playerAwards.size()) {
		std::vector<int32> weights(playerAwards.size());

		for (int i = 0; i < weights.size(); ++i) {
			weights[i] = AwardWeights.Contains(playerAwards[i].type) ? AwardWeights[playerAwards[i].type] : DefaultWeight;
		}
		//D11.PS - added function for weighted index
		int weightIndex = GetWeightedIndex(weights, gen);
		const awards::AwardData& award = playerAwards[weightIndex];
		int playerNumber = award.playerNumber;

		const APlayerCharacter* player = GetPlayerCharacterForNumber(playerNumber);

		if (player) {
			const auto exp = player->FindComponentByClass<UPlayerExperienceComponent>();

			result.Emplace();
			result.Last().Type = award.type;
			result.Last().Score = awards::IsHealthDisplay(award.type) ? valueformat::healthAsDisplayAmount((float)award.score) : (int)award.score; ;
			result.Last().PlayerNumber = playerNumber;
			result.Last().PlayerXp = exp ? exp->CurrentXp() : 0;

			result.Last().PlayerColor = player->GetPlayerColor();
			result.Last().PlayerSkinId = player->GetSkinId();
			result.Last().PlayerLocalDisplayText = player->GetLocalPlayerDisplayText();

			auto state = player->GetDungeonsBasePlayerState();

			result.Last().PlayerPrimaryNameText = state->GetPlayerPrimaryDisplayName();
			result.Last().PlayerSecondaryNameText = state->GetPlayerSecondaryDisplayName();
			result.Last().PlayerPlatform = state->GetPlayerPlatform();

			//Remove all awards of this type - only show one award per type!
			playerAwards.erase(std::remove_if(playerAwards.begin(), playerAwards.end(), [&](const awards::AwardData& v) { return v.type == award.type; }), playerAwards.end());
		}

		//We have found an award for a player, remove all other rewards for this player - even if it was an invalid player
		playerAwards.erase(std::remove_if(playerAwards.begin(), playerAwards.end(), [&](const awards::AwardData& v) { return v.playerNumber == playerNumber; }), playerAwards.end());
	}

	return result;
}

const APlayerCharacter* UAwardsGeneratorComponent::GetPlayerCharacterForNumber(int number) const {
	for (auto it = GetWorld()->GetPlayerControllerIterator(); it; ++it) {
		if (const auto pawn = Cast<const APlayerCharacter>((*it)->GetPawn())) {
			if(pawn->GetDungeonsBasePlayerState()->GetPlayerNumber() == number)
				return pawn;
		}
	}

	return nullptr;
}

TOptional<FString> FMissionFinishedSummary::GetTrialId() const {
	return !trialIdOrBlank.IsEmpty() ? trialIdOrBlank : TOptional<FString>{};
}

FMissionFinishedSummary FMissionFinishedSummary::GetCopyAdaptedToLocalControllers(UWorld* world) const {
	FMissionFinishedSummary copy(*this);

	auto playerNumber = -1;
	if (const auto pawn = world->GetFirstPlayerController()->GetPawn()) {
		if (const auto playerCharacter = Cast<APlayerCharacter>(pawn)) {
			playerNumber = playerCharacter->GetDungeonsBasePlayerState()->GetPlayerNumber();
		}
	}

	for (auto& award : copy.awards) {
		award.IsMe = award.PlayerNumber == playerNumber;
	}

	copy.awards.Sort([](const FAwardStruct& a, const FAwardStruct& b) {
		if (a.IsMe != b.IsMe) {
			return a.IsMe;
		}
		return a.PlayerNumber < b.PlayerNumber;
	});

	return copy;
}

