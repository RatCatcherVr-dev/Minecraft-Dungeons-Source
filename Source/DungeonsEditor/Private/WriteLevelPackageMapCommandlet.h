#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Logging/LogMacros.h"

#include "Commandlets/Commandlet.h"
#include "Engine/EngineTypes.h"

#include "WriteLevelPackageMapCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSEDITOR_API UWriteLevelPackageMapCommandlet : public UCommandlet
{
	GENERATED_BODY()
	virtual int32 Main(const FString& Params) override;
};
