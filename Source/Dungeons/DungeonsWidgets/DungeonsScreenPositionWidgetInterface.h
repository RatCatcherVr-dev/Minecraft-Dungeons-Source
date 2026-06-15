#pragma once
#include <ObjectMacros.h>
#include <Interface.h>
#include "DungeonsScreenPositionWidgetInterface.generated.h"

UINTERFACE(Category = "DungeonsWidget", BlueprintType)
class DUNGEONS_API UDungeonsScreenPositionWidgetInterface : public UInterface {
	GENERATED_BODY()
};

class DUNGEONS_API IDungeonsScreenPositionWidgetInterface {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DungeonsWidget")
	void SetOnScreenLocation(const FVector2D& Pos);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DungeonsWidget")
	void ModifyOnScreenLocation(const FVector2D& Diff);

};