#pragma once
#include "game/levels.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "MissionInterest.generated.h"

UENUM(BlueprintType)
enum class EMissionInterest : uint8 {
	UNSET,
	NEW_MISSION,
	NEW_REWARD,
	NEW_MERCHANT,
	NEW_LOCATION,
	NEW_BOSS,
	NEW_ENDEREYE,
	PLAYABLE,
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionInterestCarousel {
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMissionInterest interestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ELevelNames> missions;
};

UCLASS(BlueprintType)
class DUNGEONS_API UMissionInterestUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetMissionInterestText(EMissionInterest interestType, int count = 1);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetMissionInterestCarouselText(const FMissionInterestCarousel& interestCarousel);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetMissionInterestIsNew(EMissionInterest interestType);
};

