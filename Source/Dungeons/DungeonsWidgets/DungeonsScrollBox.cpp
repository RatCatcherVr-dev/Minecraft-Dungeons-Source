#include <Widget.h>
#include "SDungeonsScrollBox.h"
#include "DungeonsScrollBox.h"
#include <ScrollBoxSlot.h>

void UDungeonsScrollBox::ScrollWidgetIntoViewWithPadding(UWidget* WidgetToFind, bool AnimateScroll /*= true*/, EDescendantScrollDestination ScrollDestination /*= EDescendantScrollDestination::IntoView*/, float ScrollPadding /*= 0.0f*/)
{
	TSharedPtr<SWidget> SlateWidgetToFind;
	if (WidgetToFind)
	{
		SlateWidgetToFind = WidgetToFind->GetCachedWidget();
	}

	if (MyScrollBox.IsValid())
	{
		// NOTE: Pass even if null! This, in effect, cancels a request to scroll which is necessary to avoid warnings/ensures 
		//       when we request to scroll to a widget and later remove that widget!
		MyScrollBox->ScrollDescendantIntoView(SlateWidgetToFind, AnimateScroll, ScrollDestination, ScrollPadding);
	}
}

TSharedRef<SWidget> UDungeonsScrollBox::RebuildWidget() {
	MyScrollBox = SNew(SDungeonsScrollBox)
		.Style(&WidgetStyle)
		.ScrollBarStyle(&WidgetBarStyle)
		.Orientation(Orientation)
		.ConsumeMouseWheel(ConsumeMouseWheel)
		.NavigationDestination(NavigationDestination)
		.NavigationScrollPadding(NavigationScrollPadding)
		.OnUserScrolled(BIND_UOBJECT_DELEGATE(FOnUserScrolled, SlateHandleUserScrolled));

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UScrollBoxSlot* TypedSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyScrollBox.ToSharedRef());
		}
	}

	return MyScrollBox.ToSharedRef();
}

