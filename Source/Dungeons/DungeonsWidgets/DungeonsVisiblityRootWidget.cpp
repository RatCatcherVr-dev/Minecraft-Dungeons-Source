#include "DungeonsVisiblityRootWidget.h"
#include "ui/WidgetHelper.h"
#include <WidgetTree.h>



bool UDungeonsVisiblityRootWidget::GetVisibleRecursive() const {
	return IsVisibleRecursiveCachable(this, true);
}

void UDungeonsVisiblityRootWidget::SetVisibility(ESlateVisibility InVisibility) {
	if (GetVisibility() != InVisibility || (mCachedRecursiveVisible.IsSet() && mCachedRecursiveVisible.GetValue() != UWidgetHelper::IsVisible(InVisibility) )) {
		Super::SetVisibility(InVisibility);

		mCachedRecursiveVisible.Reset();

		OnChangedVisibility();
		
		UWidgetHelper::ForEachWidgetTreeDescendants(WidgetTree, &UDungeonsVisiblityRootWidget::RefreshRecursiveVisiblity);
	}	
}



bool UDungeonsVisiblityRootWidget::IsVisibleRecursiveCachable(const UWidget* widget, bool allowCaching)
{
	if(allowCaching){
		if (auto visWidget = Cast<UDungeonsVisiblityRootWidget>(widget)) {
			if (visWidget->mCachedRecursiveVisible.IsSet()) {
				return visWidget->mCachedRecursiveVisible.GetValue();
			}
		}
	}

	// If the widget isn't visible itself, no point in going further up the hierarchy
	if (!widget->IsVisible()) {
		return false;
	}
	// Check if widget has a PanelWidget parent, if so, run the method recursively for the parent
	UPanelWidget* parent = widget->GetParent();
	if (parent) {
		return IsVisibleRecursiveCachable(parent);
	}
	// If there is no PanelWidget parent, it could be the last step in the hierarchy with a non-panel (other widget) parent - use GetOuter to check
	else {
		UObject* outerTree = widget->GetOuter();
		if (outerTree) {
			UWidget* outerWidget = Cast<UWidget>(outerTree->GetOuter());
			if (outerWidget) {
				return IsVisibleRecursiveCachable(outerWidget);
			}
		}
	}
	// No widget parents, and we got this far while being visible all the way
	return true;
}


void UDungeonsVisiblityRootWidget::RefreshRecursiveVisiblity(UWidget* widget) {
	if (auto* userWidget = Cast<UDungeonsVisiblityRootWidget>(widget)) {		
		const bool visible = IsVisibleRecursiveCachable(widget, false);
		if (!userWidget->mCachedRecursiveVisible.IsSet() || userWidget->mCachedRecursiveVisible.GetValue() != visible) {
			userWidget->mCachedRecursiveVisible = visible;
			if (visible) {
				userWidget->OnRecursivelyShown();
			} else {
				userWidget->OnRecursivelyHidden();
			}
		}		
	}
}