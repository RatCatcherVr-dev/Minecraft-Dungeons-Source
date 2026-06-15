#include "Dungeons.h"
#include "WidgetHelper.h"
#include "PanelWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Engine.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include "game/actor/character/player/PlayerControllerBase.h"


bool UWidgetHelper::IsVisibleRecursive(UWidget* widget)
{
	// If the widget isn't visible itself, no point in going further up the hierarchy
	if (!widget->IsVisible()) {
		return false;
	}
	// Check if widget has a PanelWidget parent, if so, run the method recursively for the parent
	UPanelWidget* parent = widget->GetParent();
	if (parent) {
		return IsVisibleRecursive(parent);
	}
	// If there is no PanelWidget parent, it could be the last step in the hierarchy with a non-panel (other widget) parent - use GetOuter to check
	else {
		UObject* outerTree = widget->GetOuter();
		if (outerTree) {
			UWidget* outerWidget = Cast<UWidget>(outerTree->GetOuter());
			if (outerWidget) {
				return IsVisibleRecursive(outerWidget);
			}
		}
	}
	// No widget parents, and we got this far while being visible all the way
	return true;
}

void UWidgetHelper::SetOwnerRecursive_Helper(UWidget* widget, APlayerController* owner)
{
	if (UPanelWidget* panelWidget = Cast<UPanelWidget>(widget))
	{
		const auto slots = panelWidget->GetSlots();
		for (auto slot : slots)
		{
			SetOwnerRecursive_Helper(slot->Content, owner);
		}
	}
	else if(UUserWidget* userWidget = Cast<UUserWidget>(widget))
	{
		SetOwnerRecursive(userWidget, owner);
	}
}

void UWidgetHelper::SetOwnerRecursive(UUserWidget *userWidget, APlayerController* owner)
{
	if (auto dungeonsUserWidget = Cast<UDungeonsUserWidget>(userWidget)) {
		dungeonsUserWidget->OnOwnerIsChanging();
	}
	userWidget->SetOwningPlayer(owner);
	// D11.SSN - refresh icons for buttons
	if (auto dungeonsUserWidget = Cast<UDungeonsUserWidget>(userWidget)) {
		dungeonsUserWidget->OnOwnerWasChanged();
	}
	if (UWidgetTree* widgetTree = userWidget->WidgetTree)
	{
		SetOwnerRecursive_Helper(widgetTree->RootWidget, owner);
	}
}


bool UWidgetHelper::IsChildVisible(UPanelWidget* widget)
{
	for (const UPanelSlot* ChildSlot : widget->GetSlots())
	{
		if (ChildSlot->Content->IsVisible())
		{
			return true;
		}
	}
	return false;
}

UUserWidget* UWidgetHelper::GetUserWidgetBeforeRoot(UWidget* widget)
{
	UWidget* Parent = Cast<UWidget>(widget->GetParent());
	UUserWidget* rootWidget = nullptr;
	UUserWidget* beforeRootWidget = nullptr;
	bool bFound = false;
	while (!bFound)
	{
		while (Parent->GetParent())
		{
			Parent = Cast<UWidget>(Parent->GetParent());
		}

		UWidgetTree* widgetTree = Cast<UWidgetTree>(Parent->GetOuter());
		UUserWidget* UserWidget = Cast<UUserWidget>(widgetTree->GetOuter());
		if (widgetTree)
		{
			beforeRootWidget = rootWidget;
			rootWidget = Cast<UUserWidget>(widgetTree->GetOuter());
			if (rootWidget)
			{
				Parent = Cast<UWidget>(rootWidget->GetParent());
			}
			else
			{
				Parent = nullptr;
			}
			bFound = Parent == nullptr;
		}
	}
	return beforeRootWidget;
}

bool UWidgetHelper::IsVisible(ESlateVisibility InVisibility)
{
	return InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible || InVisibility == ESlateVisibility::HitTestInvisible;
}

TArray<UUserWidget*> UWidgetHelper::GetAllDescendentWidgetsWithInterface(UUserWidget *userWidget, TSubclassOf<UInterface> Interface) {
	TArray<UUserWidget*> FoundWidgets;
	if (auto widgetTree = userWidget->WidgetTree) {
		ForEachWidgetTreeDescendants(widgetTree, [&Interface, &FoundWidgets](UWidget* widget){
			if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
				if (foundUserWidget->GetClass()->ImplementsInterface(Interface)) {
					FoundWidgets.Add(foundUserWidget);
				}
			}
		});
	}
	return FoundWidgets;
}

namespace widgethelper {
	void ForEachFilteredWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<bool(UWidget*)> Filter, TFunctionRef<void(UWidget*)> Predicate, SeenWidgetSet& SeenWidgets) {
		if (auto RootWidget = widgetTree->RootWidget) {
			if (!SeenWidgets.Contains(RootWidget) && Filter(RootWidget)) {
				SeenWidgets.Add(RootWidget);
				Predicate(RootWidget);

				widgetTree->ForWidgetAndChildren(RootWidget, [&Predicate, &Filter, &SeenWidgets](UWidget* Child) {
					if (!SeenWidgets.Contains(Child) && Filter(Child)) {
						SeenWidgets.Add(Child);
						Predicate(Child);
						if (UUserWidget* UserWidgetChild = Cast<UUserWidget>(Child)) {
							if (UserWidgetChild->WidgetTree) {
								ForEachFilteredWidgetTreeDescendants(UserWidgetChild->WidgetTree, Filter, Predicate, SeenWidgets);
							}
						}
					}
				});
			}
		}
	}
	void ForEachFilteredWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<bool(UWidget*)> Filter, TFunctionRef<void(UWidget*)> Predicate) {
		widgethelper::SeenWidgetSet SeenWidgets;
		widgethelper::ForEachFilteredWidgetTreeDescendants(widgetTree, Filter, Predicate, SeenWidgets);
	}
}

void UWidgetHelper::ForEachWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate) {	
	widgethelper::ForEachFilteredWidgetTreeDescendants(widgetTree, [](UWidget* widget) { return true; }, Predicate);
}

void UWidgetHelper::ForEachVisibleWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate) {
	return widgethelper::ForEachFilteredWidgetTreeDescendants(widgetTree, [](UWidget* widget) { return IsVisible(widget->GetVisibility()); }, Predicate);
}


void UWidgetHelper::ForEachHiddenWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate) {
	return widgethelper::ForEachFilteredWidgetTreeDescendants(widgetTree, [](UWidget* widget) { return !IsVisible(widget->GetVisibility()); }, Predicate);
}

TArray<UUserWidget*> UWidgetHelper::GetAllDescendentWidgetsOfClass(UUserWidget *userWidget, TSubclassOf<UUserWidget> widgetClass) {
	TArray<UUserWidget*> FoundWidgets;
	if (auto widgetTree = userWidget->WidgetTree) {
		ForEachVisibleWidgetTreeDescendants(widgetTree, [&widgetClass, &FoundWidgets](UWidget* widget) {
			if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
				if (foundUserWidget->IsA(widgetClass)) {
					FoundWidgets.Add(foundUserWidget);
				}
			}
		});
	}
	return FoundWidgets;
}

TArray<UUserWidget*> UWidgetHelper::GetAllVisibleDescendentWidgetsWithInterface(UUserWidget *userWidget, TSubclassOf<UInterface> Interface) {
	TArray<UUserWidget*> FoundWidgets;
	if (auto widgetTree = userWidget->WidgetTree) {		
		ForEachVisibleWidgetTreeDescendants(widgetTree, [&Interface, &FoundWidgets](UWidget* widget) {
			if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
				if (foundUserWidget->GetClass()->ImplementsInterface(Interface)) {
					FoundWidgets.Add(foundUserWidget);
				}
			}
		});
	}
	return FoundWidgets;
}

UUserWidget* UWidgetHelper::GetFirstParentWidgetWithInterface(UWidget *widget, TSubclassOf<UInterface> Interface) {			
	if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
		if (foundUserWidget->GetClass()->ImplementsInterface(Interface)) {
			return foundUserWidget;
		}
	}
	if (auto parentAbove = widget->GetParent()) {
		return GetFirstParentWidgetWithInterface(parentAbove, Interface);
	} else if (auto outerTree = widget->GetOuter()){
		if (UWidget* outerWidget = Cast<UWidget>(outerTree->GetOuter())) {
			return GetFirstParentWidgetWithInterface(outerWidget, Interface);
		}			
	}	
	return nullptr;
}

UUserWidget* UWidgetHelper::GetFirstUserWidgetDescendentListeningForInputAction(APlayerControllerBase* controller, UUserWidget *userWidget, const FName& inputAction) {
	if (auto listeningWidget = controller->GetFirstHandlerForInputActionOfType<UUserWidget>(inputAction)) {
		if (listeningWidget == userWidget) {
			return userWidget;
		}
		if (auto widgetTree = userWidget->WidgetTree) {
			bool foundAmongDescendents = false;
			ForEachWidgetTreeDescendants(widgetTree, [&foundAmongDescendents, &listeningWidget](UWidget* widget){
				if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
					if (foundUserWidget == listeningWidget) {
						foundAmongDescendents = true;
					}
				}
			});
			if(foundAmongDescendents){
				return listeningWidget;
			}
		}
	}
	return nullptr;
}

UUserWidget* UWidgetHelper::GetFirstUserWidgetListeningForInputAction(APlayerControllerBase* controller, const FName& inputAction) {
	return controller->GetFirstHandlerForInputActionOfType<UUserWidget>(inputAction);
}

UUserWidget* UWidgetHelper::GetFirstFocusableUserWidgetDescendent(UUserWidget *userWidget) {
	TArray<UUserWidget*> FoundWidgets;
	if (auto widgetTree = userWidget->WidgetTree) {
		ForEachWidgetTreeDescendants(widgetTree, [&FoundWidgets](UWidget* widget) {			
			if (auto foundUserWidget = Cast<UUserWidget>(widget)) {
				if (foundUserWidget->bIsFocusable && foundUserWidget->GetVisibility() == ESlateVisibility::Visible) {
					FoundWidgets.Add(foundUserWidget);
				}
			}
		});
	}
	if (FoundWidgets.IsValidIndex(0)) {
		return FoundWidgets[0];
	}
	return nullptr;
}

UClass* UWidgetHelper::LoadClassAsset_Blocking(TSoftClassPtr<UObject> Asset)
{
	return Cast<UClass>(Asset.ToSoftObjectPath().TryLoad());
}
