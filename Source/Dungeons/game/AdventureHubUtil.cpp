#include "Dungeons.h"
#include "AdventureHubUtil.h"
#include "locale/LocTableFromFile.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"

FSlateColor UAdventureHubUtil::GetSeasonColor(int seasonNumber) {
	if (SeasonColorMap.Contains(seasonNumber)) {
		return SeasonColorMap[seasonNumber];
	}
	return FSlateColor();
}

FAdventureHubInfo UAdventureHubUtil::GetAdventureHubInfoFromSeasonsAPI(UObject* worldContext) {
	const auto gameInstance = worldContext->GetWorld()->GetGameInstance();
	auto liveOps = online::getLiveOps(gameInstance);
	liveOps->RequestSeasonViewUpdateAsync();
	return FAdventureHubInfo(liveOps->GetSeasonView());
}

TArray<FChallengeInfo> UAdventureHubUtil::GetAdventureHubChallengesFromSeasonsAPI(UObject* worldContext) {
	TArray<FChallengeInfo> out;
	const auto gameInstance = worldContext->GetWorld()->GetGameInstance();
	auto liveOps = online::getLiveOps(gameInstance);
	liveOps->RequestChallengesViewUpdateAsync();
	auto challenges = liveOps->GetChallengesView();
	for (auto& challenge : challenges.Challenges) {
		out.Emplace(challenge.Description, challenge.CurrentScore, challenge.RequiredScore);
	}
	return out;
}

TMap<int, FSlateColor> UAdventureHubUtil::SeasonColorMap = {
	TPair<int, FSlateColor>(1, FSlateColor(FColor(111, 131, 255)))
};

FAdventureHubInfo::FAdventureHubInfo() {
}

FAdventureHubInfo::FAdventureHubInfo(const online::liveops::SeasonView& seasonView)
	: SeasonName(seasonView.Name)
	, Tier(seasonView.PlayerLevel)
	, TierProgressPercentage(seasonView.PlayerLevelProgress)
{
	SeasonTitle = FText::Format(LocTableFromFile::Get("Season1Labels.csv", "adventure_hub_numbered_season"), seasonView.Number)
		.ToUpper();
	Countdown = FormatCountdown(seasonView.EndDateTime);
	SeasonColor = UAdventureHubUtil::GetSeasonColor(seasonView.Number);
}

FText FAdventureHubInfo::FormatCountdown(const FDateTime& NextSeason) const {
	const auto timeLeft = NextSeason - FDateTime::UtcNow();

	if (timeLeft.GetDays() < 1) {
		return FText::Format(LocTableFromFile::Get("Season1Labels.csv", "adventure_hub_next_season_countdown_in_hours"), {
			{ TEXT("hours"), FText::FromString(timeLeft.ToString(TEXT("h"))) },
			{ TEXT("minutes"), FText::FromString(timeLeft.ToString(TEXT("m"))) },
			{ TEXT("seconds"), FText::FromString(timeLeft.ToString(TEXT("s"))) }
		});
	}

	return FText::Format(LocTableFromFile::Get("Season1Labels.csv", "adventure_hub_next_season_countdown_in_days"), {
		{ TEXT("days"), FText::FromString(timeLeft.ToString(TEXT("d"))) }
	});
}

FChallengeInfo::FChallengeInfo() {
}

FChallengeInfo::FChallengeInfo(FText displayText, int count, int total)
	: DisplayText(std::move(displayText))
	, Total(total)
{
	Count = FMath::Min(count, total);
}
