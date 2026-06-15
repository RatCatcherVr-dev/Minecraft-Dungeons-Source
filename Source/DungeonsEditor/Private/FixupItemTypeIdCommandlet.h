#pragma once

#include "CoreMinimal.h"
#include "DungeonsResavePackagesCommandlet.h"
#include "Engine/CollisionProfile.h"
#include "FixupItemTypeIDCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSEDITOR_API UFixupTypeIDCommandlet : public UDungeonsResavePackagesCommandlet
{
	GENERATED_BODY()
	void PerformAdditionalOperations(UObject* Object, bool& bSavePackage) override;
};