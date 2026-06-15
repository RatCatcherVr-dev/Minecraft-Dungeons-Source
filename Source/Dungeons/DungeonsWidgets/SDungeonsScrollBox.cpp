#include "Dungeons.h"
#include "SDungeonsScrollBox.h"
#include "WidgetPath.h"

void SDungeonsScrollBox::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) {
	auto cause = InFocusEvent.GetCause();
	
	switch (cause) {
		case EFocusCause::Mouse:
		case EFocusCause::SetDirectly:
			break;
		case EFocusCause::Navigation:
		case EFocusCause::Cleared:
		case EFocusCause::OtherWidgetLostFocus:
		case EFocusCause::WindowActivate:
		default:
			if (NewWidgetPath.IsValid()) {
				ScrollDescendantIntoView(NewWidgetPath.GetLastWidget(), true, EDescendantScrollDestination::IntoView, NavigationScrollPadding);
			}
	}
}
