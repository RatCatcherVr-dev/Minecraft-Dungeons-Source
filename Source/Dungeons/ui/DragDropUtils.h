#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include <GenericPlatform/ICursor.h>
#include "DragDropUtils.generated.h"

UCLASS()
class DUNGEONS_API UDragDropUtils : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void SetDragDropCursorOverride(EMouseCursor::Type cursor);
}; 