#pragma once

#include <CoreMinimal.h>
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "DungeonsGameInstance.h"
#include "PostGameWidget.generated.h"

UCLASS()
class DUNGEONS_API UPostGameWidget : public UUserWidget {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	const FMissionFinishedSummary& GetMissionFinishedSummary() const;

	UFUNCTION(BlueprintCallable)
	bool HasInfoToShow() const;

	UFUNCTION(BlueprintCallable)
	void Reset();
};
