#pragma once

#include "CoreMinimal.h"
#include "DungeonsResavePackagesCommandlet.h"
#include "FixupLobbyChestCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSEDITOR_API UFixupLobbyChestCommandlet : public UDungeonsResavePackagesCommandlet
{
	GENERATED_BODY()
	void PerformAdditionalOperations(UObject* Object, bool& bSavePackage) override;
};