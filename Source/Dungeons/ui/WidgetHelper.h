#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UserWidget.h"
#include "WidgetHelper.generated.h"

class APlayerControllerBase;

namespace widgethelper {
	typedef TSet<UWidget*> SeenWidgetSet;
	void ForEachFilteredWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<bool(UWidget*)> Filter, TFunctionRef<void(UWidget*)> Predicate, SeenWidgetSet& SeenWidgets);
	void ForEachFilteredWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<bool(UWidget*)> Filter, TFunctionRef<void(UWidget*)> Predicate);
}

UCLASS()
class DUNGEONS_API UWidgetHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	/** Returns true if a widget and all widgets in the above hierarchy are visible. */
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static bool IsVisibleRecursive(UWidget* widget);

	/** Sets owner to widgets recursively.*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void SetOwnerRecursive(UUserWidget *userWidget, APlayerController* owner);
	static void SetOwnerRecursive_Helper(UWidget* widget, APlayerController* owner);

	/** Returns true if a any of its children are visible are visible. */
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static bool IsChildVisible(UPanelWidget* widget);

	/** Returns the user widget associated with the given widget before reaching the UI Root Widget*/
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static UUserWidget* GetUserWidgetBeforeRoot(UWidget* widget);

	/** Returns true if the given visibility is considered as Visible*/
	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static bool IsVisible(ESlateVisibility InVisibility);
	
	static void ForEachWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate);	
	static void ForEachVisibleWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate);
	static void ForEachHiddenWidgetTreeDescendants(UWidgetTree* widgetTree, TFunctionRef<void(UWidget*)> Predicate);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons", meta = (DeterminesOutputType = "widgetClass"))
	static TArray<UUserWidget*> GetAllDescendentWidgetsOfClass(UUserWidget *userWidget, TSubclassOf<UUserWidget> widgetClass);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons", meta = (DeterminesOutputType = "Interface"))
	static TArray<UUserWidget*> GetAllDescendentWidgetsWithInterface(UUserWidget *userWidget, TSubclassOf<UInterface> Interface);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons", meta = (DeterminesOutputType = "Interface"))
	static TArray<UUserWidget*> GetAllVisibleDescendentWidgetsWithInterface(UUserWidget *userWidget, TSubclassOf<UInterface> Interface);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons", meta = (DeterminesOutputType = "Interface"))
	static UUserWidget* GetFirstParentWidgetWithInterface(UWidget* widget, TSubclassOf<UInterface> Interface);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons")
	static UUserWidget* GetFirstUserWidgetDescendentListeningForInputAction(APlayerControllerBase* controller, UUserWidget* userWidget, const FName& inputAction);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons")
	static UUserWidget* GetFirstUserWidgetListeningForInputAction(APlayerControllerBase* controller, const FName& inputAction);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Dungeons")
	static UUserWidget* GetFirstFocusableUserWidgetDescendent(UUserWidget* userWidget);	

	UFUNCTION(BlueprintCallable, Category = "Utilities", meta = (DeterminesOutputType = "Asset"))
	static UClass* LoadClassAsset_Blocking(TSoftClassPtr<UObject> Asset);
};