#pragma once

#include "CoreMinimal.h"
#include "Commandlets/ResavePackagesCommandlet.h"
#include "Engine/CollisionProfile.h"
#include "DungeonsResavePackagesCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONSEDITOR_API UDungeonsResavePackagesCommandlet : public UResavePackagesCommandlet
{
	GENERATED_BODY()
public:
	virtual int32 Main(const FString& Params) override;
};