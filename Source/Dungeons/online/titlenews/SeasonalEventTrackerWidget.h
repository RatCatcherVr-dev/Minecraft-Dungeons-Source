#pragma once 

#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "SeasonalEventTrackerWidget.generated.h"

namespace online {
	namespace liveops {
		struct EventView;
	}
}

enum class EMinecraftAPIConnectionStatus : uint8;

struct FSeasonalEventTrackerState {
	FText trialProgress;
};

/**
 * A specialized widget for seasonal events. The players will be awarded cosmetics
 * if they play a certain number of seasonal trials.
 */
UCLASS(Abstract)
class USeasonalEventTrackerWidget : public UDungeonsUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

	/** Shows the progress text inside the map news component */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ProgressTextBlock = nullptr;

	/** Call to fetch latest objective data and update the progress text */
	UFUNCTION(BlueprintCallable)
	void OnRequestUpdate();

private:
	void SetState(EMinecraftAPIConnectionStatus, const TOptional<online::liveops::EventView>&);
	void UpdateWidgets();
	FDelegateHandle EventViewHandle;

	UPROPERTY()
	UGameInstance* GameInstance = nullptr;

	FSeasonalEventTrackerState state;
};
