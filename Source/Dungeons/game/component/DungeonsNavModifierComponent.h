// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "DungeonsNavModifierComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UDungeonsNavModifierComponent : public UNavModifierComponent
{
	GENERATED_BODY()
private:
	void CalcAndCacheBounds() const override;
public:
	UFUNCTION(BlueprintCallable)
	void RecalculateBounds();
};
