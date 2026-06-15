#pragma once

#include <CoreMinimal.h>
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "SubtitleOverlayWidget.generated.h"

UCLASS()
class DUNGEONS_API USubtitleOverlayWidget : public UUserWidget {
	GENERATED_BODY()
public:
	void NativeConstruct() override;
	void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnSubtitlesText(const FText& text);


private:
	void DisableSubtitleUpdates();
	void EnableSubtitleUpdates();
	FDelegateHandle mSubtitlesBinding;
};
