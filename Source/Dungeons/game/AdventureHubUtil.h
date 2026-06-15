#pragma once
#include "AdventureHubUtil.generated.h"

namespace online {
namespace liveops {
struct SeasonView;
}
}

USTRUCT(BlueprintType)
struct DUNGEONS_API FAdventureHubInfo {
	GENERATED_BODY()
public:
	FAdventureHubInfo();
	FAdventureHubInfo(const online::liveops::SeasonView& seasonView);

	UPROPERTY(BlueprintReadOnly)
	FText SeasonTitle; // "Season 1"

	UPROPERTY(BlueprintReadOnly)
	FText SeasonName; // "Cloudy Climb"

	UPROPERTY(BlueprintReadOnly)
	FText Countdown; // 36 days until Season 2 is available

	UPROPERTY(BlueprintReadOnly)
	int Tier = 0;

	UPROPERTY(BlueprintReadOnly)
	float TierProgressPercentage = 0.f;

	UPROPERTY(BlueprintReadOnly)
	FSlateColor SeasonColor;

private:
	FText FormatCountdown(const FDateTime& NextSeason) const;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FChallengeInfo {
	GENERATED_BODY()
public:
	FChallengeInfo();
	FChallengeInfo(FText displayText, int count, int total);

	UPROPERTY(BlueprintReadOnly)
	FText DisplayText; // "Complete Missions on Adventure"

	UPROPERTY(BlueprintReadOnly)
	int Count = 0; // 50

	UPROPERTY(BlueprintReadOnly)
	int Total = 0; // 70
};

UCLASS(BlueprintType) 
class DUNGEONS_API UAdventureHubUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FSlateColor GetSeasonColor(int seasonNumber);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FAdventureHubInfo GetAdventureHubInfoFromSeasonsAPI(UObject* worldContext);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FChallengeInfo> GetAdventureHubChallengesFromSeasonsAPI(UObject* worldContext);

	static TMap<int, FSlateColor> SeasonColorMap;
};
