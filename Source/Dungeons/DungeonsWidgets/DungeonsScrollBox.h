#pragma once

#include "CoreMinimal.h"
#include <ScrollBox.h>
#include <SScrollBox.h>
#include "DungeonsScrollBox.generated.h"

UCLASS()
class DUNGEONS_API UDungeonsScrollBox : public UScrollBox
{
	GENERATED_BODY()

public:
	/** Scrolls the ScrollBox to the widget during the next layout pass. */
	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ScrollWidgetIntoViewWithPadding(UWidget* WidgetToFind, bool AnimateScroll = true, EDescendantScrollDestination ScrollDestination = EDescendantScrollDestination::IntoView, float ScrollPadding = 0.0f);

protected:
	TSharedRef<SWidget> RebuildWidget() override;
};