#pragma once

#include "CoreMinimal.h"
#include "DungeonsUserWidget.h"
#include "DungeonsVisiblityRootWidget.generated.h"

UCLASS()
class DUNGEONS_API UDungeonsVisiblityRootWidget : public UDungeonsUserWidget
{
	GENERATED_BODY()
private:
	TOptional<bool> mCachedRecursiveVisible;
	static void RefreshRecursiveVisiblity(UWidget* widget);		
	static bool IsVisibleRecursiveCachable(const UWidget* widget, bool allowCaching = true);

public:
	UFUNCTION(BlueprintCallable)
	bool GetVisibleRecursive() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Dungeons")
	void OnChangedVisibility();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Dungeons")
	void OnRecursivelyShown();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Dungeons")
	void OnRecursivelyHidden();

	void SetVisibility(ESlateVisibility InVisibility) override;

};