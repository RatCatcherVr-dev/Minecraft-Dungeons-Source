#pragma once

#include "CoreMinimal.h"
#include "DungeonsResavePackagesCommandlet.h"
#include "FixupArrowCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSEDITOR_API UFixupArrowCommandlet : public UDungeonsResavePackagesCommandlet
{
	GENERATED_BODY()
	void PerformAdditionalOperations(UObject* Object, bool& bSavePackage) override;
};