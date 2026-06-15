#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "ItemPowerUtil.generated.h"

UCLASS(BlueprintType)
class UItemPowerUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()	
public:
	static float GetItemPowerDisplayValueFloat(float ItemPower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int GetItemPowerDisplayInt(float ItemPower);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FText GetItemPowerDisplayText(float ItemPower);

	static float GetItemPowerFromDisplayValue(float DisplayItemPower);
};
