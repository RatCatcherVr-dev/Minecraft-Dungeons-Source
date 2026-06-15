#pragma once
#include "ProgressStat.generated.h"

UENUM()
enum class EProgressStat : uint8 {
	WIN_MISSIONS,
	GIVE_GIFTS,
	DEFEAT_ENCHANTED_MOBS,
	DEFEAT_EVENT_MOBS,
	WIN_MISSIONS_DEFAULT,
	WIN_MISSIONS_ADVENTURE,
	WIN_MISSIONS_APOCALYPSE,
	WIN_MISSIONS_APOCALYPSE_PLUS,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER0,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER1,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER2,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER3,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER4,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER5,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER6,
	WIN_MISSIONS_APOCALYPSE_PLUS_TIER7,
	WIN_HYPERMISSIONS,
};
ENUM_NAME(EProgressStat);



UCLASS(BlueprintType)
class UProgressStatUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

	static const FText MissionsWonAtDifficultyTemplate;	

	static const FText SuccessfullyCompleteMissionsAtDifficultyTemplateOne;
	static const FText SuccessfullyCompleteMissionsAtDifficultyTemplateMany;
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetProgressStatText(EProgressStat stat);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetProgressStatExplainerText(EProgressStat stat, int count);
};
