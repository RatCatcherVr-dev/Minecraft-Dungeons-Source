

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SaveSpinnerInterface.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API USaveSpinnerInterface : public UObject
{
	GENERATED_BODY()
	
public:
		
	UFUNCTION(BlueprintCallable, Category = "Spinner")
	static bool GetIsSpinnerVisible();

	static void SetIsSpinnerVisible(bool visible);

	static void InitialiseSaveSpinner();
	
};
